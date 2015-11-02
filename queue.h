/*
 * Copyright (C) 2015 - Martin Jaros <xjaros32@stud.feec.vutbr.cz>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef QUEUE_H
#define QUEUE_H

#if defined(__ATOMIC_ACQUIRE)
#define atomic_test_and_set(ptr) __atomic_test_and_set(ptr, __ATOMIC_ACQUIRE)
#elif defined(__x86_64__)
char atomic_test_and_set(char*);
asm("atomic_test_and_set:\n"
    "    mov $1,%eax\n"
    "    xchg %al,(%rdi)\n"
    "    retq");
#endif

#if defined(__ATOMIC_RELEASE)
#define atomic_clear(ptr) __atomic_clear(ptr, __ATOMIC_RELEASE)
#elif defined(__x86_64__)
void atomic_clear(char*);
asm("atomic_clear:\n"
    "    movb $0,(%rdi)\n"
    "    retq");
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

struct list
{
    struct list *next, *prev;
};

typedef struct
{
    struct list node;
    char data[BUFFER_SIZE];
}
buffer_t;

typedef struct
{
    struct list head;
    char lock;
}
queue_t;

static inline void queue_push(queue_t *q, buffer_t *buf)
{
    while(atomic_test_and_set(&q->lock));
    buf->node.prev = q->head.prev;
    buf->node.next = &q->head;
    q->head.prev->next = &buf->node;
    q->head.prev = &buf->node;
    atomic_clear(&q->lock);
}

static inline buffer_t* queue_pull(queue_t *q)
{
    if(q->head.next == &q->head)
        return NULL;

    while(atomic_test_and_set(&q->lock));
    struct list *node = q->head.next;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    atomic_clear(&q->lock);
    return (buffer_t*)node;
}

static inline void queue_init(queue_t *q, buffer_t *bufs, size_t count)
{
    q->lock = 0;
    q->head.next = &q->head;
    q->head.prev = &q->head;
    while(count--) queue_push(q, &bufs[count]);
}

#endif /* QUEUE_H */
