/*
 * =====================================================================================
 *
 *    Filename   :  dt_fifo.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2016年04月18日 20时32分34秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include "dt_fifo.h"

static dt_fifo *fifo_alloc_common(void *buffer, size_t size)
{
    dt_fifo *f;
    if (!buffer) {
        return NULL;
    }
    f = dt_mallocz(sizeof(dt_fifo));
    if (!f) {
        dt_free(buffer);
        return NULL;
    }
    f->buffer = buffer;
    f->end    = f->buffer + size;
    dt_fifo_reset(f);
    return f;
}

dt_fifo *dt_fifo_alloc(unsigned int size)
{
    void *buffer = dt_malloc(size);
    return fifo_alloc_common(buffer, size);
}

dt_fifo *dt_fifo_alloc_array(size_t nmemb, size_t size)
{
    void *buffer = dt_malloc_array(nmemb, size);
    return fifo_alloc_common(buffer, nmemb * size);
}

void dt_fifo_free(dt_fifo *f)
{
    if (f) {
        dt_freep(&f->buffer);
        dt_free(f);
    }
}

void dt_fifo_freep(dt_fifo **f)
{
    if (f) {
        dt_fifo_free(*f);
        *f = NULL;
    }
}

void dt_fifo_reset(dt_fifo *f)
{
    f->wptr = f->rptr = f->buffer;
    f->wndx = f->rndx = 0;
}

int dt_fifo_size(const dt_fifo *f)
{
    return (uint32_t)(f->wndx - f->rndx);
}

int dt_fifo_space(const dt_fifo *f)
{
    return f->end - f->buffer - dt_fifo_size(f);
}

int dt_fifo_realloc2(dt_fifo *f, unsigned int new_size)
{
    unsigned int old_size = f->end - f->buffer;

    if (old_size < new_size) {
        int len          = dt_fifo_size(f);
        dt_fifo *f2 = dt_fifo_alloc(new_size);

        if (!f2) {
            return DTERROR(ENOMEM);
        }
        dt_fifo_generic_read(f, f2->buffer, len, NULL);
        f2->wptr += len;
        f2->wndx += len;
        dt_free(f->buffer);
        *f = *f2;
        dt_free(f2);
    }
    return 0;
}

int dt_fifo_grow(dt_fifo *f, unsigned int size)
{
    unsigned int old_size = f->end - f->buffer;
    if (size + (unsigned)dt_fifo_size(f) < size) {
        return DTERROR(EINVAL);
    }

    size += dt_fifo_size(f);

    if (old_size < size) {
        return dt_fifo_realloc2(f, FFMAX(size, 2 * size));
    }
    return 0;
}

/* src must NOT be const as it can be a context for func that may need
 * updating (like a pointer or byte counter) */
int dt_fifo_generic_write(dt_fifo *f, void *src, int size,
                          int (*func)(void *, void *, int))
{
    int total = size;
    uint32_t wndx = f->wndx;
    uint8_t *wptr = f->wptr;

    do {
        int len = FFMIN(f->end - wptr, size);
        if (func) {
            len = func(src, wptr, len);
            if (len <= 0) {
                break;
            }
        } else {
            memcpy(wptr, src, len);
            src = (uint8_t *)src + len;
        }
        // Write memory barrier needed for SMP here in theory
        wptr += len;
        if (wptr >= f->end) {
            wptr = f->buffer;
        }
        wndx    += len;
        size    -= len;
    } while (size > 0);
    f->wndx = wndx;
    f->wptr = wptr;
    return total - size;
}

int dt_fifo_generic_peek_at(dt_fifo *f, void *dest, int offset, int buf_size, void (*func)(void*, void*, int))
{
    uint8_t *rptr = f->rptr;

    dt_assert2(offset >= 0);

    /*
     * *ndx are indexes modulo 2^32, they are intended to overflow,
     * to handle *ndx greater than 4gb.
     */
    dt_assert2(buf_size + (unsigned)offset <= f->wndx - f->rndx);

    if (offset >= f->end - rptr) {
        rptr += offset - (f->end - f->buffer);
    } else {
        rptr += offset;
    }

    while (buf_size > 0) {
        int len;

        if (rptr >= f->end) {
            rptr -= f->end - f->buffer;
        }

        len = FFMIN(f->end - rptr, buf_size);
        if (func) {
            func(dest, rptr, len);
        } else {
            memcpy(dest, rptr, len);
            dest = (uint8_t *)dest + len;
        }

        buf_size -= len;
        rptr     += len;
    }

    return 0;
}

int dt_fifo_generic_peek(dt_fifo *f, void *dest, int buf_size,
                         void (*func)(void *, void *, int))
{
    // Read memory barrier needed for SMP here in theory
    uint8_t *rptr = f->rptr;

    do {
        int len = FFMIN(f->end - rptr, buf_size);
        if (func) {
            func(dest, rptr, len);
        } else {
            memcpy(dest, rptr, len);
            dest = (uint8_t *)dest + len;
        }
        // memory barrier needed for SMP here in theory
        rptr += len;
        if (rptr >= f->end) {
            rptr -= f->end - f->buffer;
        }
        buf_size -= len;
    } while (buf_size > 0);

    return 0;
}

int dt_fifo_generic_read(dt_fifo *f, void *dest, int buf_size,
                         void (*func)(void *, void *, int))
{
    // Read memory barrier needed for SMP here in theory
    do {
        int len = FFMIN(f->end - f->rptr, buf_size);
        if (func) {
            func(dest, f->rptr, len);
        } else {
            memcpy(dest, f->rptr, len);
            dest = (uint8_t *)dest + len;
        }
        // memory barrier needed for SMP here in theory
        dt_fifo_drain(f, len);
        buf_size -= len;
    } while (buf_size > 0);
    return 0;
}

/** Discard data from the FIFO. */
void dt_fifo_drain(dt_fifo *f, int size)
{
    dt_assert2(dt_fifo_size(f) >= size);
    f->rptr += size;
    if (f->rptr >= f->end) {
        f->rptr -= f->end - f->buffer;
    }
    f->rndx += size;
}

#ifdef TEST

int main(void)
{
    /* create a FIFO buffer */
    dt_fifo *fifo = dt_fifo_alloc(13 * sizeof(int));
    int i, j, n;

    /* fill data */
    for (i = 0; dt_fifo_space(fifo) >= sizeof(int); i++) {
        dt_fifo_generic_write(fifo, &i, sizeof(int), NULL);
    }

    /* peek at FIFO */
    n = dt_fifo_size(fifo) / sizeof(int);
    for (i = -n + 1; i < n; i++) {
        int *v = (int *)dt_fifo_peek2(fifo, i * sizeof(int));
        printf("%d: %d\n", i, *v);
    }
    printf("\n");

    /* peek_at at FIFO */
    n = dt_fifo_size(fifo) / sizeof(int);
    for (i = 0; i < n; i++) {
        dt_fifo_generic_peek_at(fifo, &j, i * sizeof(int), sizeof(j), NULL);
        printf("%d: %d\n", i, j);
    }
    printf("\n");

    /* read data */
    for (i = 0; dt_fifo_size(fifo) >= sizeof(int); i++) {
        dt_fifo_generic_read(fifo, &j, sizeof(int), NULL);
        printf("%d ", j);
    }
    printf("\n");

    /* test *ndx overflow */
    dt_fifo_reset(fifo);
    fifo->rndx = fifo->wndx = ~(uint32_t)0 - 5;

    /* fill data */
    for (i = 0; dt_fifo_space(fifo) >= sizeof(int); i++) {
        dt_fifo_generic_write(fifo, &i, sizeof(int), NULL);
    }

    /* peek_at at FIFO */
    n = dt_fifo_size(fifo) / sizeof(int);
    for (i = 0; i < n; i++) {
        dt_fifo_generic_peek_at(fifo, &j, i * sizeof(int), sizeof(j), NULL);
        printf("%d: %d\n", i, j);
    }

    dt_fifo_free(fifo);

    return 0;
}

#endif
