
void timer_interrupt();

void init_timer_interrupt(uint32_t timer_refclk);

void enable_timer_interrupt();

void disable_timer_interrupt();

void reset_timer_interrupt();

uint32_t time_slice_elapsed();
