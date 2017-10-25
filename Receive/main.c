/*
		-- CLK --
	  80 MHz = 12.5ns

		-- Start Bit --
			 High
     2ms / 12.5ns = 160,000
			 Low
	 	 1ms / 12.5ns = 80,000

		-- '1' --
			 High
	 1ms / 12.5ns = 80,000
	 		 Low
	.5ms / 12.5ns = 40,000

		-- '0' --
			 High
	.5ms / 12.5ns = 40,000
	 		 Low
	.5ms / 12.5ns = 40,000


*/

#include "systick.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include "Ports.h"
#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"

/*Initialize variables*/
#define FRAME_LENGTH 6

#define HIGH 1
#define LOW 0

#define OFF_LED 0xF1
#define GREEN_LED 0x08
#define RED_LED 0x2
#define BLUE_LED 0x04
#define WHITE_LED 0x0E
#define YELLOW_LED 0x0A
#define PURPLE_LED 0x06
#define CYAN_LED 0x0C

#define mask_address 0x30

/*
	edge : if we are high edge or low edge
		   1 - high edge
		   0 - low edge

	high_count: how many ticks we got in high pulse
			0 ~ 160,000

	start_flag: start bit status
			 1 - recieved start bit
			 0 - havent recieved start bit

	recieved_bit: what bit we recieved
			1 - recieved a '1'
			0 - recieved a '0'

	frame : store bits here

	drawFlag: status of draw LCD
			1 - ready to draw to LCD
			0 - not ready to draw to LCD

*/
unsigned char edge = 1 , start_flag = 0 , recieved_bit = 0 ,
 							drawFlag = 1, acknowledge = 0 , edgeCount = 0 ,
							device = 0 , command = 0;
unsigned int frame = 0 , Frame_reg = 0 , x_position = 0 ,
							y_positison = 0 , Frame_reg_debugger;
unsigned long int	high_count = 0;
unsigned char i , j , bit_mask_frame;

/*Initialize funtions*/
void EnableInterrupts(void);
void DisableInterrupts(void);
void DelayWait10ms(uint32_t n);
void draw_startLine(void);
void draw_startbit(void);
void draw_One(void);
void draw_Zero(void);
void draw_lastline(void);

/**************************************************
				systick ISR
		This happens every 12.5ns * 20,000 = 0.125ms
**************************************************/
void SysTick_Handler(void){
	if(edge == 0)										//Check if on low edge
		high_count = high_count + 1; 	//Increase count
	if(high_count > 30){						//Check count
		systick_disable();						//Disable sytick timer
		high_count = 0;								//Reset count
	}
}

/**************************************************
        		GPIO ISR
 		PD7 is edge triggered for both edges
**************************************************/
void GPIOPortD_Handler(void){
	GPIO_PORTD_ICR_R = 0x80; 		// Acknowledge flag
	edge = (edge + 1) % 2; 			// Tuggles edge

	if(edge == 0){							// Enable systick on negedge to check bits received
		systick_enable();
		acknowledge = 0;					// Flag
	}
	else{
		systick_disable();				// Disable systick to check bits recieved
		if( high_count > 12){			// Check start flag
			start_flag = 1;					// Start flag enable
			acknowledge = 0;				// Flag
		}
		else if( high_count < 6){
			recieved_bit = 0;				// Received bit logic '0'
			acknowledge = 1;				// Flag
		}
		else{
			recieved_bit = 1;				// Received bit
			acknowledge = 1;				// Flag
		}
		high_count = 0; 					// reset high counter
	}

}


/**************************************************
    				Main
**************************************************/
int main(void){

  DisableInterrupts();
  Pll_start();
  SysTick_Init();

  PORT_D_INIT();	// Intilaze port D
  PORT_F_INIT();  // Intilaze port F

  ST7735_InitR(INITR_REDTAB);  // set system clock to 80 MHz
  DelayWait10ms(100);
  ST7735_FillScreen(0xFFFF);   // set screen to white
  ST7735_FillScreen(0x0000);   // set screen to black

  LED(RED_LED);
  EnableInterrupts();      	// enable after everything initialized
  systick_disable();				// disable systick

  while(1){ 								// main loop

		while(start_flag == 0){} // Do nothing, wait here till we get a start bit
		systick_disable();
		high_count = 0;					// reset count
		start_flag = 0; 				// reset start flag
		i = 0;									// for frame length compare
		while(1){	// This while loop collects (decodes) data
			if(i == FRAME_LENGTH)					// Break loop once frame is completed
					break;
				systick_disable();	// disable systick collect data
				high_count = 0;			// reset count
				while(acknowledge == 0) {} 	// wait here till we know what bit we got
				acknowledge = 0;						// reset flag
				if(recieved_bit == 1){			// collect data
					frame = (frame << 1);			// shift prior data by one
					frame = frame | 0x01;			// mask (add) received bit '1'
				}
				else{
					frame = (frame << 1);			// shift prior data by one
					frame = frame & ~0x01;		// mask (add) received bit '0'
				}
				drawFlag = 1;
		i = i + 1;											// increase i
		}
		systick_disable();
		high_count = 0;
		Frame_reg = frame;							// for LCD draw
		Frame_reg_debugger = Frame_reg; // for debug
		frame = 0;											// reset frame

		ST7735_FillScreen(0x0000);   // set screen to black
		//ST7735_DrawString(120, 120, "Device: ", ST7735_Color565(255,0,255));
		draw_startbit();						// draw the startbit
		draw_startLine();						// draw the start line

		/* for loop uses first bit position at 0x20 (bit 6),
			 and shifts following bits to its position until
			 complete frame is drawn
		*/
		for(j = 0 ; j < FRAME_LENGTH ; j = j + 1){
			bit_mask_frame = Frame_reg & 0x20;	// bitwise mask
			if(bit_mask_frame == 0x20 )					// check if logical '1'
				draw_One();												// draw '1' on LCD
			else
				draw_Zero();											// draw '0' on LCD
			Frame_reg = Frame_reg << 1;					// shift remaining bits left
		}
		draw_lastline();											// finish trailing gap LCD
		device = (Frame_reg_debugger >> 4);		// device first two bits
		command = Frame_reg_debugger & 0xF;		// command last four bits 
		ST7735_DrawString(0, 0, "Device: ", ST7735_Color565(255,0,255));
		ST7735_DrawString(0, 2, "Command: ", ST7735_Color565(255,0,255));
		ST7735_SetCursor(10 , 0);
		ST7735_OutUDec(device);		// write device #
		ST7735_SetCursor(10 , 2);
		ST7735_OutUDec(command);	// write command #
	}
}

/**************************************************
					Delay
**************************************************/
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
      time--;
    }
    n--;
  }
}

/**************************************************
				Draw Line
		Vertical Line ( x , y , h , color)
			x - vertical start of line
			y - vertical start
			h - vertical add

		Horizonital Line ( x , y , w , color)
			x - horizontal start
			y - verical start
			w - horiztonal add

		50 - Low 			100 - High
**************************************************/
void draw_startLine(void){
	ST7735_DrawFastHLine(0 , 100 , 8 , ST7735_Color565(255,255,0));
}

void draw_startbit(void){
	ST7735_DrawFastVLine(8 , 50  , 50 , ST7735_Color565(0,255,0));
	ST7735_DrawFastHLine(8 , 50 , 20 , ST7735_Color565(0,255,0));
	ST7735_DrawFastVLine(28 , 50  , 50 , ST7735_Color565(0,255,0));
	ST7735_DrawFastHLine(28  , 100 ,  10 , ST7735_Color565(0,255,0));
	x_position = 38; y_positison = 50;
}

void draw_One(void){
	ST7735_DrawFastVLine(x_position , 50  , 50 , ST7735_Color565(0,0,255));
	ST7735_DrawFastHLine(x_position , 50 , 10 , ST7735_Color565(0,0,255));
	ST7735_DrawFastVLine(x_position + 10 , 50  , 50 , ST7735_Color565(0,0,255));
	ST7735_DrawFastHLine(x_position + 10 , 100 , 5 , ST7735_Color565(0,0,255));
	x_position = x_position + 15; y_positison = 50;
}

void draw_Zero(void){
	ST7735_DrawFastVLine(x_position , 50  , 50 , ST7735_Color565(255,0,0));
	ST7735_DrawFastHLine(x_position , 50 , 5 , ST7735_Color565(255,0,0));
	ST7735_DrawFastVLine(x_position + 5 , 50  , 50 , ST7735_Color565(255,0,0));
	ST7735_DrawFastHLine(x_position + 5 , 100 , 5 , ST7735_Color565(255,0,0));
	x_position = x_position + 10; y_positison = 50;
}
void draw_lastline(void){
	if(x_position != 128)
		ST7735_DrawFastHLine(x_position, 100 , 128 - x_position , ST7735_Color565(255,255,0));
}
