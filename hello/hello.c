

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"


#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "driverlib/interrupt.h"

#include "inc/tm4c123gh6pm.h"
#include "driverlib/timer.h"
// global integers for edge 1 and edge 2
volatile double edge1, edge2;

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//
// Interrupt handler on rising edge
//
//*****************************************************************************
void IR_Handler (void) {
		UARTprintf("hit");
		GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_6);
		if(edge1==0){
				TimerLoadSet(TIMER0_BASE,TIMER_A, 0xFFFFFFFF);
				edge1=TimerLoadGet(TIMER0_BASE,TIMER_A);
		}
		else if(edge2==0)
				edge2=TimerLoadGet(TIMER0_BASE,TIMER_A);
		//else if edge2==0, get time stamp for edge 2
		
}	

int
main(void)
{	
		ROM_FPULazyStackingEnable();//from hello
		ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
		
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		ConfigureUART();
		ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);//leds
		ROM_GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
		double bitValues[32];
		double pulseWidth=0;
		int arrayPos=0;
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_6);
		ROM_GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_BOTH_EDGES);
		GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_6);
		ROM_IntEnable(INT_GPIOB);
		ROM_IntMasterEnable();
		edge1=0; edge2=0;
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_1);//red led
	
		while(1)
    {
			UARTprintf("hits");
			ROM_SysCtlSleep();
			//after interrupt completes, sleep again
			ROM_SysCtlSleep();
			//after interrupt for edge 2, do edge1-edge2, find numerical value, map it to 1 or 0 bit
			pulseWidth=edge1-edge2; //counting down, so edge 1-edge2= width
			bitValues[arrayPos]=pulseWidth;
			arrayPos++;
			if(pulseWidth!=0){
					UARTprintf("%f",pulseWidth);
			}
			pulseWidth=0;
			

			// make array of 1's and 0's (size 32)
			// if statements to map to character
			//reset edge1 and edge2=0;
			
    }
}
