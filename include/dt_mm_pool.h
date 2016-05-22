/*
 * =====================================================================================
 *
 *    Filename   :  dt_mm_pool.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2016年05月22日 14时57分46秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#ifndef DT_POOL_H
#define DT_POOL_H

#include <stdint.h>

struct dt_mm_pool;

struct dt_mm_pool *dt_mm_pool_create(int64_t size);
uint8_t *dt_mm_pool_alloc(struct dt_mm_pool *pool, int size);
int dt_mm_pool_free(struct dt_mm_pool *pool, uint8_t *ptr);
void dt_mm_pool_destroy(struct dt_mm_pool *pool);
void dt_mm_pool_dump(struct dt_mm_pool *pool);

#endif
