/*
		-- CLK --
	   80 MHz = 12.5ns

		-- Time --
		t = 0.5 ms
				H 	 L
	Start bit : 4t   2t
	Logic '1' : 2t   1t
	Logic '0' : 1t   1t

		-- Frame --
	6 bits
		-  0 - 1 : 2 bits : Address
		-  2 - 5 : 4 bits : commands

		-- PINS --
	PD7 - IR Output
	PA0-1 - UART 0

		-- PINS --
	UART - 9600 Baud

*/

#include "systick.h"
#include "UART.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include "Ports.h"

#define FRAME_LENGTH 6

#define OFF_LED 0xF1
#define GREEN_LED 0x08
#define RED_LED 0x2
#define BLUE_LED 0x04
#define WHITE_LED 0x0E
#define YELLOW_LED 0x0A
#define PURPLE_LED 0x06
#define CYAN_LED 0x0C

#define half_40kHz 999

#define address_clear 0x0F

unsigned char Greetings[] = "\tFinal Project\n\r";
unsigned char NewLine[] = "\n\r";

unsigned char mode;
unsigned char* arrayPTR;

unsigned int userInput , frame , address;

void EnableInterrupts(void);
void DisableInterrupts(void);

/*************************
    Debounce
*************************/
void debounce(void){
   int i = 0;
   for(i = 0 ; i < 1500000 ; i = i + 1){}
}
/*************************
    GPIO ISR used to toggle device mode
      x = 0
    00 xxxx - 0x00
    01 xxxx - 0x01
    10 xxxx - 0x10
    11 xxxx - 0x11
*************************/
// Port F
void GPIOPortF_Handler(void){
  debounce();                 // delay
  userInput = userInput & 15 ;// clear address bits but not the commands
  mode = (mode + 1) % 4;      // Toggles between the 4 modes 0 ~3
	// SW1 - PF4
	if(GPIO_PORTF_RIS_R & 0x10){// switch 1 - PF4 pressed
		if(mode == 0){            // check mode
			address = 0x00;         // update address
			LED(RED_LED);           // toggle on-board led color
		}

		else if(mode == 1){       // check mode
			address = 0x10;         // update address
			LED(GREEN_LED);         // toggle on-board led color
		}
		else if(mode == 2){       // check mode
			address = 0x20;         // update address
			LED(BLUE_LED);          // toggle on-board led color
		}
		else if(mode == 3){       // check mode
			address = 0x30;         // update address
			LED(WHITE_LED);         // toggle on-board led color
		}

	 }
	 /* SW2 - PF0 - start ARM EXTRA CREDIT
	 if(GPIO_PORTF_RIS_R & 0x01){

	 }*/
	GPIO_PORTF_ICR_R = 0x11; // acknowledge flag PF0 , PF4
}
/*************************
    Main
*************************/
int main(void){
  unsigned char i;
  mode = 0;
  frame = 0;

  DisableInterrupts();
  Pll_start();
  SysTick_Init(half_40kHz * 2);	// Initialize systick at 80kHZ (IR spec)

  PORT_D_INIT();	// Intilaze port D
  PORT_F_INIT();	// Intilaze port F and Intruppts ARMED

  UART0_Init();   // Initiazlize UART (communication between terminal and MCU)

  userInput = UART_In_Char_Blocking_U0();
  UART_OutString_U0(Greetings);
  UART_OutString_U0(NewLine);
  LED(RED_LED);
  EnableInterrupts();      // enable after everything initialized
  while(1){

    // Get a charcter from terminal 
    userInput = UART_In_Char_Blocking_U0();
    UART_OutChar_U0(userInput);
    UART_OutString_U0(NewLine);
	  userInput = (address_clear & UART0_value(userInput) ) | address;
	  frame = userInput;

		//while(1){
    // send Startbit
    SysTick_Wait_IR_half_periods_toggle(4); 		// 4 * t(0.5ms) = 2ms of tuggling
    SysTick_Wait_IR_half_periods_no_toggle(2); 	// 2 * t(0.5ms) = 1ms of low

    // Send 6 bits
    for (i = 0; i < FRAME_LENGTH; i = i + 1){
    	// Check Logic '1' or '0'
    	if(userInput & 0x20)
    		SysTick_Wait_IR_half_periods_toggle(2); // 2 * t(0.5ms) = 1.0 ms of tuggling , Logic '1'
    	else
    		SysTick_Wait_IR_half_periods_toggle(1); // 1 * t(0.5ms) = 0.5 ms of tuggling , Logic '0'

    	SysTick_Wait_IR_half_periods_no_toggle(1);// 1 * t(0.5ms) = 0.5 ms of low
    	userInput = userInput << 1;								// Shift data to the left
    }

		userInput = 0;
   // End bit
	 //}
  }
}
