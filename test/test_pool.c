/*
 * =====================================================================================
 *
 *    Filename   :  test_pool.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2016年05月22日 15时01分07秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include "dt_mm_pool.h"
#include "dt_log.h"

#define TAG "TEST-POOL"

int main(int argc, char **argv)
{
    int64_t total = 10 * 1024 * 1024;
    int i;
    struct dt_mm_pool *pool = dt_mm_pool_create(total);
    uint8_t *tmp;
    int size = 10240;
    tmp = dt_mm_pool_alloc(pool, size);
    dt_mm_pool_dump(pool);
    dt_mm_pool_free(pool, tmp);
    dt_mm_pool_dump(pool);
    dt_mm_pool_destroy(pool);
    return 0;
}
