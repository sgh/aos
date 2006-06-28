#ifndef VIC_H
#define VIC_H



uint8 vic_request_vector(uint32 address, uint8 inrqnum);

void vic_vector_enable(uint8 vectornum);

void vic_vector_disable(uint8 vectornum);

void vic_irq_enable(uint8 irqnum);

void vic_irq_disable(uint8 irqnum);


#endif
