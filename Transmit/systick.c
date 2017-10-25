#include "systick.h"
#include "UART.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include "Ports.h"

#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable

#define half_40kHz 999		//.125ms
#define mask_7th_bit 128

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(unsigned long delay){
  NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait 24-bit
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears 24-bit
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x00000000; // priority 0
  NVIC_ST_CTRL_R = 0x00000005;   // enable SysTick with core clock
}

// Time delay using busy wait.
// The delay parameter is in units of the 80 MHz core clock. (12.5 nsec)
void SysTick_Wait(unsigned long delay){
  volatile unsigned long elapsedTime;
  unsigned long startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime - NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}

// Time delay using busy wait.
// 12.5ns(X) = 12,500ns - for IR_FREQ / 2

void SysTick_Wait_IR_half_periods_toggle(unsigned long delay){unsigned long i;

  delay = delay * 40;

  GPIO_PORTD_DATA_R = 0x80;
  for(i=0; i < delay; i++){
    SysTick_Wait(half_40kHz);  // wait 12.5us
    GPIO_PORTD_DATA_R = GPIO_PORTD_DATA_R ^ mask_7th_bit; //tuggle
  }

}
void SysTick_Wait_IR_half_periods_no_toggle(unsigned long delay){unsigned long i;

  delay = delay * 40;

  for(i=0; i < delay; i++){
    SysTick_Wait(half_40kHz);   // wait 12.5us
    GPIO_PORTD_DATA_R = 0x00;   //no tuggle
  }
}
