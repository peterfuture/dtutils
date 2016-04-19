/*
 * =====================================================================================
 *
 *    Filename   :  dt_fifo.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2016年04月18日 20时30分01秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

// Import fifo impl From FFMPEG

#ifndef DT_FIFO_H
#define DT_FIFO_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dt_mem.h"

typedef struct dt_fifo {
    uint8_t *buffer;
    uint8_t *rptr, *wptr, *end;
    uint32_t rndx, wndx;
} dt_fifo;

/**
 * Initialize an dt_fifo.
 * @param size of FIFO
 * @return dt_fifo or NULL in case of memory allocation failure
 */
dt_fifo *dt_fifo_alloc(unsigned int size);

/**
 * Initialize an dt_fifo.
 * @param nmemb number of elements
 * @param size  size of the single element
 * @return dt_fifo or NULL in case of memory allocation failure
 */
dt_fifo *dt_fifo_alloc_array(size_t nmemb, size_t size);

/**
 * Free an dt_fifo.
 * @param f dt_fifo to free
 */
void dt_fifo_free(dt_fifo *f);

/**
 * Free an dt_fifo and reset pointer to NULL.
 * @param f dt_fifo to free
 */
void dt_fifo_freep(dt_fifo **f);

/**
 * Reset the dt_fifo to the state right after dt_fifo_alloc, in particular it is emptied.
 * @param f dt_fifo to reset
 */
void dt_fifo_reset(dt_fifo *f);

/**
 * Return the amount of data in bytes in the dt_fifo, that is the
 * amount of data you can read from it.
 * @param f dt_fifo to read from
 * @return size
 */
int dt_fifo_size(const dt_fifo *f);

/**
 * Return the amount of space in bytes in the dt_fifo, that is the
 * amount of data you can write into it.
 * @param f dt_fifo to write into
 * @return size
 */
int dt_fifo_space(const dt_fifo *f);

/**
 * Feed data at specific position from an dt_fifo to a user-supplied callback.
 * Similar as dt_fifo_gereric_read but without discarding data.
 * @param f dt_fifo to read from
 * @param offset offset from current read position
 * @param buf_size number of bytes to read
 * @param func generic read function
 * @param dest data destination
 */
int dt_fifo_generic_peek_at(dt_fifo *f, void *dest, int offset, int buf_size, void (*func)(void*, void*, int));

/**
 * Feed data from an dt_fifo to a user-supplied callback.
 * Similar as dt_fifo_gereric_read but without discarding data.
 * @param f dt_fifo to read from
 * @param buf_size number of bytes to read
 * @param func generic read function
 * @param dest data destination
 */
int dt_fifo_generic_peek(dt_fifo *f, void *dest, int buf_size, void (*func)(void*, void*, int));

/**
 * Feed data from an dt_fifo to a user-supplied callback.
 * @param f dt_fifo to read from
 * @param buf_size number of bytes to read
 * @param func generic read function
 * @param dest data destination
 */
int dt_fifo_generic_read(dt_fifo *f, void *dest, int buf_size, void (*func)(void*, void*, int));

/**
 * Feed data from a user-supplied callback to an dt_fifo.
 * @param f dt_fifo to write to
 * @param src data source; non-const since it may be used as a
 * modifiable context by the function defined in func
 * @param size number of bytes to write
 * @param func generic write function; the first parameter is src,
 * the second is dest_buf, the third is dest_buf_size.
 * func must return the number of bytes written to dest_buf, or <= 0 to
 * indicate no more data available to write.
 * If func is NULL, src is interpreted as a simple byte array for source data.
 * @return the number of bytes written to the FIFO
 */
int dt_fifo_generic_write(dt_fifo *f, void *src, int size, int (*func)(void*, void*, int));

/**
 * Resize an dt_fifo.
 * In case of reallocation failure, the old FIFO is kept unchanged.
 *
 * @param f dt_fifo to resize
 * @param size new dt_fifo size in bytes
 * @return <0 for failure, >=0 otherwise
 */
int dt_fifo_realloc2(dt_fifo *f, unsigned int size);

/**
 * Enlarge an dt_fifo.
 * In case of reallocation failure, the old FIFO is kept unchanged.
 * The new fifo size may be larger than the requested size.
 *
 * @param f dt_fifo to resize
 * @param additional_space the amount of space in bytes to allocate in addition to dt_fifo_size()
 * @return <0 for failure, >=0 otherwise
 */
int dt_fifo_grow(dt_fifo *f, unsigned int additional_space);

/**
 * Read and discard the specified amount of data from an dt_fifo.
 * @param f dt_fifo to read from
 * @param size amount of data to read in bytes
 */
void dt_fifo_drain(dt_fifo *f, int size);

/**
 * Return a pointer to the data stored in a FIFO buffer at a certain offset.
 * The FIFO buffer is not modified.
 *
 * @param f    dt_fifo to peek at, f must be non-NULL
 * @param offs an offset in bytes, its absolute value must be less
 *             than the used buffer size or the returned pointer will
 *             point outside to the buffer data.
 *             The used buffer size can be checked with dt_fifo_size().
 */
static inline uint8_t *dt_fifo_peek2(const dt_fifo *f, int offs)
{
    uint8_t *ptr = f->rptr + offs;
    if (ptr >= f->end) {
        ptr = f->buffer + (ptr - f->end);
    } else if (ptr < f->buffer) {
        ptr = f->end - (f->buffer - ptr);
    }
    return ptr;
}

#endif /* DT_FIFO_H */
