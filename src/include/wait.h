/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef WAIT_H
#define WAIT_H

#include <mutex.h>
#include <list.h>

struct condition {
	mutex_t lock;
	struct list_head waiting;
};

void cond_init(struct condition* cond);
		
void cond_wait(struct condition* cond, uint32_t timeout);

void cond_signal(struct condition* cond);

void cond_broadcast(struct condition* cond);


#endif
