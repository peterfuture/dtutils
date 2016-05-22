/*
 * =====================================================================================
 *
 *    Filename   :  test_event.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2016年04月22日 13时44分14秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include "dt_event.h"
#include "dt_log.h"

#define TAG "TEST-EVENT"

#define EVENT_SERVER_ID_TEST   0x100
#define EVENT_SERVER_NAME_TEST "SERVER-TEST"

#define EVNET_TEST 0x100

int main(int argc, char **argv)
{
    dt_server_mgt_t *mgt = dt_event_server_create();
    event_server_t *server = dt_alloc_server(EVENT_SERVER_ID_TEST, EVENT_SERVER_NAME_TEST);
    event_t *event = dt_alloc_event(EVENT_SERVER_ID_TEST, EVNET_TEST);
    dt_register_server(mgt, server);
    dt_send_event_sync(mgt, event);

    event_t *event_tmp = dt_get_event(server);
    if (event_tmp) {
        dt_info(TAG, "Get event:%d \n", event_tmp->type);
    }
    dt_remove_server(mgt, server);
    dt_event_server_release(mgt);
    return 0;
}
