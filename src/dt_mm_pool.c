/*
 * =====================================================================================
 *
 *    Filename   :  dt_mm_pool.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2016年05月22日 14时54分42秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "dt_lock.h"
#include "dt_mm_pool.h"

#define TAG "MM_POOL"

struct mm_item {
    struct list_head list;
    uint8_t *ptr;
    int size;
};

struct mm_list {
    struct list_head list;
    int64_t size;
    int count;
};

struct dt_mm_pool {
    uint8_t *mem;
    int64_t size;

    struct mm_list mluse;
    struct mm_list mlfree;

    dt_lock_t mutex;
};

struct dt_mm_pool *dt_mm_pool_create(int64_t size)
{
    struct dt_mm_pool *pool = (struct dt_mm_pool *)malloc(sizeof(struct dt_mm_pool));
    if (!pool) {
        return NULL;
    }
    memset(pool, 0, sizeof(struct dt_mm_pool));
    pool->mem = (uint8_t *)malloc(size);
    if (!pool->mem) {
        free(pool);
        return NULL;
    }
    pool->size = size;
    INIT_LIST_HEAD(&pool->mluse.list);
    INIT_LIST_HEAD(&pool->mlfree.list);

    struct mm_item *item = (struct mm_item *)malloc(sizeof(struct mm_item));
    item->ptr = pool->mem;
    item->size = size;
    INIT_LIST_HEAD(&item->list);

    pool->mlfree.size = size;
    pool->mlfree.count++;
    list_add_tail(&item->list, &pool->mlfree.list);

    dt_lock_init(&pool->mutex, NULL);
    return pool;
}

uint8_t *dt_mm_pool_alloc(struct dt_mm_pool *pool, int size)
{
    uint8_t *palloc = NULL;
    dt_lock(&pool->mutex);
    struct mm_list *ul = &pool->mluse;
    struct mm_list *fl = &pool->mlfree;
    struct mm_item *item_free;
    struct list_head *list1, *list2;

    // Find on free item
    list_for_each_safe(list1, list2, &fl->list) {
        item_free = list_entry(list1, struct mm_item, list);
        if (item_free->size >= size) {
            break;
        }
        item_free = NULL;
    }
    if (!item_free) {
        goto end;
    }

    // Create one use item
    struct mm_item *item_use = (struct mm_item *)malloc(sizeof(struct mm_item));
    if (!item_use) {
        goto end;
    }
    item_use->ptr = item_free->ptr;
    item_use->size = size;
    INIT_LIST_HEAD(&item_use->list);
    list_add_tail(&item_use->list, &ul->list);
    ul->count++;
    ul->size += size;

    // Update free list & maybe remove free item
    palloc = item_free->ptr;
    item_free->ptr += size;
    item_free->size -= size;
    fl->size -= size;
    if (item_free->size == 0) {
        list_del(&item_free->list);
        free(item_free);
        fl->count--;
    }
    dt_info(TAG, "alloc %d memory at:%p ok \n", size, palloc);
end:
    dt_unlock(&pool->mutex);
    return palloc;
}

int dt_mm_pool_free(struct dt_mm_pool *pool, uint8_t *ptr)
{
    int ret = -1;
    dt_lock(&pool->mutex);
    struct mm_list *ul = &pool->mluse;
    struct mm_list *fl = &pool->mlfree;
    struct mm_item *item_use;
    struct list_head *list1, *list2;

    // Find item use
    list_for_each_safe(list1, list2, &ul->list) {
        item_use = list_entry(list1, struct mm_item, list);
        if (item_use->ptr == ptr) {
            break;
        }
        item_use = NULL;
    }
    if (!item_use) {
        goto end;
    }

    // Remove use item in use list
    list_del(&item_use->list);
    ul->count--;
    ul->size -= item_use->size;

    // Setup free list
    struct mm_item *item_free = NULL;
    list_for_each_safe(list1, list2, &ul->list) {
        item_free = list_entry(list1, struct mm_item, list);
        if ((item_free->ptr + item_free->size) == item_use->ptr) {
            break;
        }
        item_free = NULL;
    }

    ret = 0;
    if (item_free) {
        item_free->ptr = item_use->ptr;
        item_free->size += item_use->size;
        fl->size += item_use->size;
        free(item_use);
        dt_info(TAG, "free %p memory size:%d ok \n", ptr, item_use->size);
        goto end;
    }

    INIT_LIST_HEAD(&item_use->list);
    list_add(&item_use->list, &fl->list);
    fl->count++;
    fl->size += item_use->size;
    dt_info(TAG, "free %p memory size:%d ok \n", ptr, item_use->size);

end:
    // second check - maybe free list can merge to one item
    if (ret == 0) {
        struct mm_item *header = list_first_entry(&fl->list, struct mm_item, list);
        struct mm_item *next = list_entry(header->list.next, struct mm_item, list);
        if (next && (header->ptr + header->size == next->ptr)) {
            header->size += next->size;
            list_del(&next->list);
            fl->count--;
            free(next);
        }
    }
    dt_unlock(&pool->mutex);
    return ret;
}

void dt_mm_pool_destroy(struct dt_mm_pool *pool)
{
    struct mm_list *ul = &pool->mluse;
    struct mm_list *fl = &pool->mlfree;
    struct mm_item *item_use;
    struct mm_item *item_free;
    struct list_head *list1, *list2;

    // Free item use
    list_for_each_safe(list1, list2, &ul->list) {
        item_use = list_entry(list1, struct mm_item, list);
        if (item_use) {
            free(item_use);
        }
    }

    // Free item free
    list_for_each_safe(list1, list2, &fl->list) {
        item_free = list_entry(list1, struct mm_item, list);
        if (item_free) {
            free(item_free);
        }
    }

    if (pool->mem) {
        free(pool);
    }
    return;
}

void dt_mm_pool_dump(struct dt_mm_pool *pool)
{
    if (!pool) {
        return;
    }
    struct mm_list *ul = &pool->mluse;
    struct mm_list *fl = &pool->mlfree;
    dt_info(TAG, "====================================\n");
    dt_info(TAG, "pool total mm:%lld \n", pool->size);
    dt_info(TAG, "mm use list\n");
    dt_info(TAG, "count:%d size:%lld\n", ul->count, ul->size);
    dt_info(TAG, "mm free list\n");
    dt_info(TAG, "count:%d size:%lld\n", fl->count, fl->size);
    dt_info(TAG, "====================================\n");
    return;
}
