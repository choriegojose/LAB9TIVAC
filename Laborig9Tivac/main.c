#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"


// jose Choriego 16523
// Electronica Digital
//
/// Iniciamos las variables y prototipos de funciones
    uint32_t ui32Period;
    uint32_t ui32Ustatus; // variable para el status de las interrupciones del UART
    uint8_t controlvar; // variable para controlar color
    uint8_t Active;  // Variable para controlar activacion

    uint8_t LED_RED = 2;
    uint8_t LED_GREEN = 8;
    uint8_t LED_BLUE= 4;

int main(void)
{
    void InitUART(void);

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); // Se configuro un reloj de 40MHz

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Habilito el puerto F

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // Declaro como output los leds  del  puerto F

    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_4MA,GPIO_PIN_TYPE_STD_WPU); // Habilitamos los Pads del puerto F

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Habilitamos el periferico del TIMER0

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // Configuramos  el timer 0

    ui32Period = (SysCtlClockGet()/2) ; // El periodo del reloj lo ponemos para que sea 0.5Hz

    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1); //Cargo el valor al reloj

    IntEnable(INT_TIMER0A); // Habilitamos el bloque A del timer0

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // Habilitamos la interrupcion del timer.

    IntMasterEnable();  // Habilitamos la interrupcion

    TimerEnable(TIMER0_BASE, TIMER_A); // Habilito el timer 0

    InitUART();  // Iniciamos UART

    while (1)
    {

    }
}

void Timer0IntHandlerA(void)
{
    // Borramos la interrupcion del timer
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Leemos el estado actual del pin GPIO y
    // lo escribimos de vuelta en el estado contrario

    // Rutina para seleccionar led
    if (controlvar == 0x01)
    {
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
                         0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, LED_BLUE);
        }
    }

    if (controlvar == 0x02)
    {
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3))
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
                         0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, LED_GREEN);
        }
    }

    if (controlvar == 0x03)
    {
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1))
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
                         0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, LED_RED);
        }
    }
}


    void UARTIntHandler(void)
{
    ui32Ustatus = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ui32Ustatus);
    while (UARTCharsAvail(UART0_BASE))
    {
        if (UARTCharGet(UART0_BASE) == 'b')
        {
            controlvar = 0x01;

        }

        if (UARTCharGet(UART0_BASE) == 'g')
        {
            controlvar = 0x02;

        }

        if (UARTCharGet(UART0_BASE) == 'r')
        {
            controlvar = 0x03;

        }

    }

}

void InitUART(void)
{  // Funcion para habilitar UART

    /*Habilitamos el puerto A en el UART*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /*Habilitamos el periferico en el puerto A*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /* UART es controlado por los perifericos */
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Ponemos la config del UART. */
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Habilitamos las interrupciones para el UART

    IntMasterEnable();
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    UARTCharPut(UART0_BASE,'I');
    UARTCharPut(UART0_BASE,'n');
    UARTCharPut(UART0_BASE,'i');
    UARTCharPut(UART0_BASE,'c');
    UARTCharPut(UART0_BASE,'i');
    UARTCharPut(UART0_BASE,'a');
    UARTCharPut(UART0_BASE,' ');
    UARTCharPut(UART0_BASE,'U');
    UARTCharPut(UART0_BASE,'A');
    UARTCharPut(UART0_BASE,'R');
    UARTCharPut(UART0_BASE,'T');

}

