
#include "ringbuf.h"

void ringbuf_init(struct ringbuf *rbuf)
{
    rbuf->start = 0;
    rbuf->end = 0;
}

int ringbuf_write(struct ringbuf *rbuf, struct buffer *buf)
{
    // Check if there is enough free space
    if((rbuf->start > rbuf->end ? rbuf->start - rbuf->end : RINGBUF_SIZE - rbuf->end + rbuf->start) < buf->len + 3)
        return 0;

    // Write length
    rbuf->data[rbuf->end++] = (buf->len & 0x00FF);
    rbuf->data[rbuf->end++] = (buf->len & 0xFF00) >> 8;

    // Copy data
    uint16_t i;
    for(i = 0; i < buf->len; i++)
    {
        rbuf->data[rbuf->end++] = buf->data[i];
        if(rbuf->end == RINGBUF_SIZE) rbuf->end = 0;
    }

    return 1;
}

int ringbuf_read(struct ringbuf *rbuf, struct buffer *buf)
{
    // Check if there is any data
    if(rbuf->start == rbuf->end)
        return 0;

    // Read length
    buf->len  = rbuf->data[rbuf->start++];
    buf->len |= rbuf->data[rbuf->start++] << 8;

    // Copy data
    uint16_t i;
    for(i = 0; i < buf->len; i++)
    {
        buf->data[i] = rbuf->data[rbuf->start++];
        if(rbuf->start == RINGBUF_SIZE) rbuf->start = 0;
    }

    return 1;
}

