
#include "ringbuf.h"

void ringbuf_init(struct ringbuf *buf)
{
    buf->start = 0;
    buf->end = 0;
}

size_t ringbuf_write(struct ringbuf *buf, uint8_t *data, size_t len)
{
    int i;
    for(i = 0; len--; i++)
    {
        size_t tmp = buf->end + 1 < RINGBUF_SIZE ? buf->end + 1 : 0;
        if(buf->start == tmp) break;
        buf->data[buf->end] = data[i];
        buf->end = tmp;
    }
    return i;
}

size_t ringbuf_read(struct ringbuf *buf, uint8_t *data, size_t len)
{
    int i;
    for(i = 0; (buf->start != buf->end) && len--; i++)
    {
        data[i] = buf->data[buf->start];
        buf->start = buf->start + 1 < RINGBUF_SIZE ? buf->start + 1 : 0;
    }
    return i;
}

