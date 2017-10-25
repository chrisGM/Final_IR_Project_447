/* Register names:
  
  UARTX_DR_R - Data Register for UART X , Shared for Tx and Rx

  UARTX_FR_R -  UART FLAGS
    UART_FR_RXFE - Uart X Rx FIFO Full( 0 - Full , 1 - Empty)
    UART_FR_TXFE - UART X Tx FIFO Full( 0 - FUll , 1 - Empty)
*/
#include "systick.h"
#include "UART.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include "Ports.h"



char adc_U1[10];

/**************************************************
                    UART0 Config
               PA0 - Rx || PA1 - Tx 
**************************************************/
// Initialize the UART for 9600 baud rate (assuming 80 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART0_Init(void){
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; // activate UART0
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // activate port A
  
  UART0_CTL_R &= ~UART_CTL_UARTEN;       // disable UART
  UART0_IBRD_R = 521;                    // IBRD = int(80,000,000 / (16 * 9600)) = int(520.83)
  UART0_FBRD_R = 54;                     // FBRD = round( .83 * 64 + .5) = 53.83
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);  // 8 bit word length (no parity bits, one stop bit, FIFOs)
	UART0_CTL_R |= UART_CTL_UARTEN;       // enable UART

  GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1,PA0
  GPIO_PORTA_DEN_R   |= 0x03;           // enable digital I/O on PA1,PA0
                                        // configure PA1,PA0 as UART0
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog functionality on PA1,PA0
  
	
}
unsigned int UART0_value(unsigned int Input){
  unsigned char x;
    if(Input == '0')
      x = 0x0;
    else if(Input == '1')
      x = 0x1;
    else if(Input == '2')
      x = 0x2;    
    else if(Input == '3')
      x = 0x3;
    else if(Input == '4')
      x = 0x4;
    else if(Input == '5')
      x = 0x5;
    else if(Input == '6')
      x = 0x6;
    else if(Input == '7')
      x = 0x7;
    else if(Input == '8')
      x = 0x8;    
    else if(Input == '9')
      x = 0x9;
    else if(Input == 'a' || Input == 'A')
      x = 0xa;
    else if(Input == 'b' || Input == 'B')
      x = 0xb;
    else if(Input == 'c' || Input == 'C')
      x = 0xc;
    else if(Input == 'd' || Input == 'D')
      x = 0xd;
		else if(Input == 'e' || Input == 'E')
      x = 0xe;
    else if(Input == 'f' || Input == 'F')
      x = 0xf; 
    return x;
}


/**************************************************
                    Rx
                 blocking
  WHILE Uart 0 , Rx FIFO is empty ( = 1)
    then keep looping
  else 
    return Rx FIFO 
**************************************************/

// UART 0
unsigned char UART_In_Char_Blocking_U0(void){ unsigned char temp;
  while( (UART0_FR_R & UART_FR_RXFE) != 0 ){}
  temp = (unsigned char)(UART0_DR_R&0xFF); 
  return(temp);  
}

/**************************************************
                    Tx
  If Uart 0 , Tx FIFO is Empty ( = 1)
    then transmit Tx FIFO Data
  else 
**************************************************/

// UART 0
void UART_OutChar_U0(unsigned char data){
  while( (UART0_FR_R & UART_FR_TXFF) != 0 ){}
  UART0_DR_R = data;
}
//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART_OutString_U0(unsigned char buffer[]){
  int i=0;
  while(buffer[i]!='\0'){
    UART_OutChar_U0(buffer[i]);
    i++;
  }
}

