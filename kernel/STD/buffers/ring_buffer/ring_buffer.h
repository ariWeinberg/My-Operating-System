#ifndef RING_BUFFER
#define RING_BUFFER

#include "../../int.h"
#include "../../bool.h"

typedef struct
{
    uint8_t *data;
    uint16_t head;
    uint16_t tail;
    uint16_t count;
    uint16_t capacity;
} RingBuffer;

RingBuffer *ring_buffer_init(RingBuffer *buf, uint16_t size);
bool ring_buffer_push(RingBuffer *buf, uint8_t element);
bool ring_buffer_pop(RingBuffer *buf, uint8_t *out);
bool ring_buffer_is_empty(RingBuffer *buf);
bool ring_buffer_is_full(RingBuffer *buf);

#endif
