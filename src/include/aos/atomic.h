/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_ATOMIC_H
#define AOS_ATOMIC_H

/**
 * \brief Atomic swap a value with a memory address
 * @param address The memory address
 * @param new_val The value to write to memory
 * @return The value read from memory
 */
uint32_t atomic_xchg(uint32_t* address, uint32_t new_val);

#endif
