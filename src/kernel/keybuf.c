// #include <kernel.h>
#include <aos.h>
#include <semaphore.h>
#include <irq.h>
#include <assert.h>

struct aosfifo {
	unsigned char nr;
	char* txbuf;
	unsigned int buf_size;
	
	unsigned int putidx;
	unsigned int getidx;
	unsigned int txbuf_size;
};


struct aosfifo keybuf;

static DECLARE_MUTEX_UNLOCKED(keybuf_lock);
static semaphore_t keyready_sem;

static unsigned int put_fifo(struct aosfifo* f, const unsigned char* src, unsigned int size) {
	unsigned int num = 0;

	while (size--) {

		assert( f->txbuf_size > 0 ) // assert on full

		assert( f->txbuf != NULL );
		f->txbuf[f->putidx++] = *src;

		if (f->putidx == f->buf_size)
			f->putidx = 0;

		src++;
		num++;

		f->txbuf_size--;
	}
	return num;
}

static unsigned int get_fifo(struct aosfifo* f, unsigned char* dst, unsigned int size) {
	unsigned int num = 0;

	while (size--) {
		if ( f->txbuf_size == f->buf_size ) // if empty, return //->getidx == m->putidx)
			return 0;

		
		*dst = f->txbuf[f->getidx++];

		if (f->getidx == f->buf_size)
			f->getidx = 0;
		
		num++;
		dst++;
	}
	f->txbuf_size += num;

	assert( f->txbuf_size <= f->buf_size );
	return num;
}


uint32_t aos_get_keybuf(void) {
	uint32_t key = 0;
	sem_down( &keyready_sem );
	
	mutex_lock(&keybuf_lock);
	get_fifo(&keybuf, &key, sizeof(key));
	mutex_unlock(&keybuf_lock);
	
	return key;
}


void aos_put_keybuf(uint32_t key ) {

	mutex_unlock(&keybuf_lock);
	if (!put_fifo(&keybuf, &key, sizeof(key) )) {
		mutex_unlock(&keybuf_lock);
		return;
	}
	mutex_unlock(&keybuf_lock);
	sem_up( &keyready_sem );
}


static void keybuf_init(void) {
	sem_init( &keyready_sem, 0);
}

AOS_MODULE_INIT(keybuf_init);
