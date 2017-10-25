#include "systick.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include "Ports.h"

#define delay 9999 // 10,000 * 12.5ns = 125,000ns 125us 12.5ms
#define half_40kHz 999

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait 24-bit
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears 24-bit
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x1FFFFFFF)|0xE0000000; // priority 7  
 
}

void systick_disable(void){NVIC_ST_CTRL_R = 0x00000000;}
void systick_enable(void){
	NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait 24-bit
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears 24-bit
	NVIC_ST_CTRL_R  = 0x00000007;
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 nsec)
void SysTick_Wait(unsigned long delayh){
  volatile unsigned long elapsedTime;
  unsigned long startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime - NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delayh);
}

void SysTick_Wait_IR_half_periods_no_toggle(unsigned long delayh){unsigned long i;
  
  delayh = delayh * 40;

  for(i=0; i < delayh; i= i + 1){
    SysTick_Wait(half_40kHz);  // wait 12.5us
    GPIO_PORTD_DATA_R = 0x00; //no tuggle
  }
}
