// UART.h

// this connection occurs in the USB debugging cable
// U0Rx (PA0) connected to serial port on PC
// U0Tx (PA1) connected to serial port on PC
// Ground connected ground in the USB cable


//------------UART_Init------------
// Initialize the UART for 9600 baud rate (assuming 80 MHz clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART0_Init(void);
unsigned int UART0_value(unsigned int);
/**************************************************
                    Rx
**************************************************/
unsigned char UART_In_Char_Blocking_U0(void);

/**************************************************
                    Tx
**************************************************/
void UART_OutChar_U0(unsigned char);
void UART_OutString_U0(unsigned char buffer[]);
