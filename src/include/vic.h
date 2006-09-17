#ifndef VIC_H
#define VIC_H



uint8_t vic_request_vector(uint32_t address, uint8_t inrqnum);

void vic_vector_enable(uint8_t vectornum);

void vic_vector_disable(uint8_t vectornum);

void vic_irq_enable(uint8_t irqnum);

void vic_irq_disable(uint8_t irqnum);


#endif
