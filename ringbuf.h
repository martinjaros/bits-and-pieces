
#include <stdint.h>
#include <unistd.h>

#define RINGBUF_SIZE 0xFF

struct ringbuf
{
    uint8_t data[RINGBUF_SIZE];
    size_t start, end;
};

void ringbuf_init(struct ringbuf *buf);

size_t ringbuf_write(struct ringbuf *buf, uint8_t *data, size_t len);

size_t ringbuf_read(struct ringbuf *buf, uint8_t *data, size_t len);

