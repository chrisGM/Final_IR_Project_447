// Initialize SysTick_Interrupt at bus clock.
void SysTick_Init(void);
void systick_disable(void);
void systick_enable(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait_IR_half_periods_no_toggle(unsigned long delay);
