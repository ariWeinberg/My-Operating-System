#include "../../defs.h"
#include "ring_buffer.h"
#include "../../../MEMORY_MANAGMENT/memory_managment.h"

RingBuffer *ring_buffer_init(RingBuffer *buf, uint16_t size)
{
    if (buf == NULL || size == 0)
        return NULL;

    buf->data = malloc(size);
    if (buf->data == NULL)
        return NULL;

    buf->head = 0;
    buf->tail = 0;
    buf->capacity = size;
    buf->count = 0;

    return buf;
}

bool ring_buffer_push(RingBuffer *buf, uint8_t element)
{
    if (buf == NULL || buf->data == NULL || buf->count == buf->capacity )
        return false;
    
    buf->count++;
    buf->data[buf->tail] = element;
    buf->tail = (buf->tail + 1) % buf->capacity;
    return true;
}

bool ring_buffer_pop(RingBuffer *buf, uint8_t *out)
{
    if (buf == NULL || buf->data == NULL ||
        out == NULL || buf->count == 0)
        return false;
    
    buf->count--;
    *out = buf->data[buf->head];
    buf->head = (buf->head + 1) % buf->capacity;
    return true;
}

bool ring_buffer_is_empty(RingBuffer *buf)
{
    return buf->count == 0;
}

bool ring_buffer_is_full(RingBuffer *buf)
{
    return buf->count == buf->capacity;
}
