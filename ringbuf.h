
#include <stdint.h>

#define RINGBUF_SIZE 256

struct ringbuf
{
    uint8_t data[RINGBUF_SIZE];
    uint16_t start, end;
};

struct buffer
{
    uint8_t *data;
    uint16_t len;
};

/* Initializes the ring buffer structure */
void ringbuf_init(struct ringbuf *rbuf);

/* Writes the buffer into the ring buffer, returns 0 if there is not enough space */
int ringbuf_write(struct ringbuf *rbuf, struct buffer *buf);

/* Reads the first buffer from the ring buffer, returns 0 if there are no buffers */
int ringbuf_read(struct ringbuf *rbuf, struct buffer *buf);

