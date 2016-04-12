#ifndef _DLP_QUEUE_H_
#define _DLP_QUEUE_H_

#include "pthread.h"
#include "stdint.h"

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))
#endif

#ifndef CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

#define POINTER_TO_UINT(p)  ((unsigned int)(p))

#if defined (__GNUC__)
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif

#define VIDEO_OUT_MAX_COUNT 4
#define AUDIO_OUT_MAX_COUNT 4
#define SUB_OUT_MAX_COUNT   4

typedef struct _node_ {
    void *data;
    int size;
    struct _node_ *next;
    struct _node_ *prev;
} _node_t;

typedef struct {
    _node_t *head;
    _node_t *tail;
    uint32_t length;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} dt_queue_t;

dt_queue_t *dt_queue_new(void);

typedef void (*free_func)(void *);
void dt_queue_free(dt_queue_t * qu, free_func func);
void dt_queue_flush(dt_queue_t * qu, free_func func);

int wait_on_queue(dt_queue_t * qu);
int wait_on_dt_queue_timeout(dt_queue_t * qu, int timeout);

uint32_t dt_queue_length(dt_queue_t * qu);

void dt_queue_push_head(dt_queue_t * qu, void *data);
void dt_queue_push_tail(dt_queue_t * qu, void *data);
void dt_queue_push_nth(dt_queue_t * qu, void *data, uint32_t n);

void *dt_queue_pop_head(dt_queue_t * qu);
void *dt_queue_pre_pop_head(dt_queue_t * qu);
void *dt_queue_pop_tail(dt_queue_t * qu);
void *dt_queue_pop_nth(dt_queue_t * qu, uint32_t n);

void *dt_queue_peek_head(dt_queue_t * qu);
void *dt_queue_peek_tail(dt_queue_t * qu);
void *dt_queue_peek_nth(dt_queue_t * qu, uint32_t n);

#endif
