#include "systick.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include "Ports.h"


#define OFF_LED 0xF1
#define GREEN_LED 0x08
#define RED_LED 0x2
#define BLUE_LED 0x04
#define WHITE_LED 0x0E
#define YELLOW_LED 0x0A
#define PURPLE_LED 0x06
#define CYAN_LED 0x0C

void PORT_D_INIT(void){ volatile unsigned int delay;
  SYSCTL_RCGC2_R |= 0x00000008;     // 1) D clock
  delay = SYSCTL_RCGC2_R;           // delay   
  
	GPIO_PORTD_LOCK_R = 0x4C4F434B;   // unlock GPIO Port D
  
  GPIO_PORTD_CR_R = 0x80;           // 2) allow changes to PD7        
  
  GPIO_PORTD_AMSEL_R = 0x00;        // 3) disable analog function
  
  GPIO_PORTD_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  
  GPIO_PORTD_DIR_R = ~0x80;         // 5) Input - PD7    
  
  GPIO_PORTD_AFSEL_R = 0x00;        // 6) no alternate function
  
  GPIO_PORTD_PUR_R = 0x80;          // 7) Disable pullup   resistors on PD1 , PD2 , PD6 , PD7      
  GPIO_PORTD_PDR_R = 0x00;          // 8) Enable pulldown  resistors on PD7 

  GPIO_PORTD_DEN_R = 0x80;          // 9) enable digital pins PD7

 GPIO_PORTD_DATA_R = 0x00;

 Intruppt_INIT_Port_D();
}

void PORT_F_INIT(void){ volatile unsigned int delay;
  
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  
  GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
  
  GPIO_PORTF_CR_R = 0x1F;           // 2) allow changes to PF4-0       
  
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
 
  GPIO_PORTF_DIR_R = 0x0E;          // 5) Output - PF3-1 || Input - PF0 , PF4  
                    
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  
  GPIO_PORTF_PUR_R |= 0x11;          // 7) Enable  pull-up resistors on PF0 , PF4 
  GPIO_PORTF_PDR_R = 0x00;           // 8) Disable pull-down resistors   

  GPIO_PORTF_DEN_R = 0x01F;          // 9) enable digital pins PF4-0
                       
}

/**************************************************
                Intruppt Initilaze Ports
**************************************************/
void Intruppt_INIT_Port_D(void){
  GPIO_PORTD_IS_R &= ~0x80;     // 1) PD7 is edge-sensitive ( Interrupt Sense)

  GPIO_PORTD_IBE_R |= 0x80;     // 2) PD7 Rising AND Falling edges ( Interrupt Both Edges)
  
  //GPIO_PORTD_IEV_R |= 0x11;   // 3) PD7 raising edge event
  
  GPIO_PORTD_ICR_R = 0x80;      // 4) Clear flags PD7 (Interrupt Clear)
  
  GPIO_PORTD_IM_R |= 0x80;      // 5) Arm interrupt on PD7 (Interrupt Mask)
  
  NVIC_PRI0_R |= (NVIC_PRI0_R & 0x1FFFFFFF)|0x00000000; // 6) priority 0 Port D( Proirty Field)
  
  NVIC_EN0_R |= 0x00000008;      // 7) enable IRQ 3 in NVIC ( Interrupt Enable)

}

/**************************************************
                LED Color Change
**************************************************/
void LED(unsigned char color){
  
  if(color == GREEN_LED)
      GPIO_PORTF_DATA_R = GREEN_LED;
  
  if(color == BLUE_LED)
      GPIO_PORTF_DATA_R = BLUE_LED;
    
  if(color == RED_LED)
      GPIO_PORTF_DATA_R = RED_LED;

  if(color == PURPLE_LED)
    GPIO_PORTF_DATA_R = PURPLE_LED;
  
  if(color == OFF_LED)
    GPIO_PORTF_DATA_R = OFF_LED;

  if(color == WHITE_LED)
    GPIO_PORTF_DATA_R = WHITE_LED;
}
