/*
 * =====================================================================================
 *
 *    Filename   :  test_list.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2016年04月12日 18时32分13秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include "list.h"
#include <stdio.h>

struct item {
    struct list_head list;
    int value;
};

int main(int argc, char **argv)
{
    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);

    struct item item1;
    item1.value = 1;
    INIT_LIST_HEAD(&item1.list);
    struct item item2;
    item2.value = 2;
    INIT_LIST_HEAD(&item2.list);
    struct item item3;
    item3.value = 3;
    INIT_LIST_HEAD(&item3.list);

    list_add_tail(&item1.list, &tmp);
    list_add_tail(&item2.list, &tmp);
    list_add_tail(&item3.list, &tmp);

    struct item *item;
    struct list_head *list1, *list2;
    list_for_each_safe(list1, list2, &tmp) {
        item = list_entry(list1, struct item, list);
        printf("item value:%d \n", item->value);
    }

    return 0;
}
