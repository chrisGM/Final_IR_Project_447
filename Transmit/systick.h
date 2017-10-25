// Initialize SysTick_Interrupt at bus clock.
void SysTick_Init(unsigned long delay);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait_IR_half_periods_toggle(unsigned long delay);
void SysTick_Wait_IR_half_periods_no_toggle(unsigned long delay);
