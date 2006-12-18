#ifndef VIC_H
#define VIC_H



/**
 * \brief Request a vector for a interrupt
 * @param address The function-address to handle the interrupt
 * @param inrqnum The irq we want to use
 * @return A vector-number to use in vector_enable/disable-calls
 */
uint8_t request_vector(uint32_t address, uint8_t inrqnum);

/**
 * \brief Enable the interrupt-vector
 * @param vectornum The vector-num to enable
 */
void vector_enable(uint8_t vectornum);

/**
 * \brief Disable the interrupt-vector
 * @param vectornum The vector-num to disable
 */
void vector_disable(uint8_t vectornum);

/**
 * \brief Enable an irq
 * @param irqnum The irq to enable
 */
void irq_enable(uint8_t irqnum);

/**
 * \brief Disable and irq
 * @param irqnum The irq-to disable
 */
void irq_disable(uint8_t irqnum);

/**
 * \brief Syscall to unmask all interrupts
 */
void enable_irqs();

/**
 * \brief Syscall to unmask all interrupts
 */
void disable_irqs();

#endif
