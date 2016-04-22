#ifndef DT_EVENT_H
#define DT_EVENT_H

#include "dt_lock.h"

typedef struct event {
    int type;
    unsigned long arg;
    int server_id;
    struct event *next;
} event_t;

#define MAX_EVENT_SERVER_NAME_LEN 1024

typedef struct event_server {
    char name[MAX_EVENT_SERVER_NAME_LEN];
    int id;
    event_t *event;
    dt_lock_t event_lock;
    int event_count;
    struct event_server *next;
} event_server_t;

#define EVENT_SERVER_ID_MAIN   0
#define EVENT_SERVER_NAME_MAIN "SERVER-MAIN"

typedef struct event_server_mgt {
    event_server_t *server;
    dt_lock_t server_lock;
    int server_count;
    int exit_flag;
    pthread_t transport_loop_id;
} dt_server_mgt_t;


/* *
 * Create server manager context
 *
 * @param cache cache context
 * @param size FIFO Size
 * @param mode cache mode
 *
 * @return dt_server_mgt_t pointer for success, NULL otherwise
 *
 */
dt_server_mgt_t *dt_event_server_create();

/* *
 * Release server manager context
 * Just release server manager and main server, guarantee
 * have removed all other server but server-main
 *
 * @param mgt server manager context
 *
 * @return 0 for success, negative errorcode otherwise
 *
 */
int dt_event_server_release(dt_server_mgt_t *mgt);

/* *
 * Alloc server context
 *
 * @param id   User defined server id
 * @param name User defined server name
 *
 * @return event_server_t pointer for success, NULL otherwise
 *
 */
event_server_t *dt_alloc_server(int id, char *name);

/* *
 * Register server to server manager
 *
 * @param mgt    server manager context
 * @param server server to be register
 *
 * @return 0 for success, negative errorcode otherwise
 *
 */
int dt_register_server(dt_server_mgt_t *mgt, event_server_t * server);

/* *
 * Remove server from server manager
 * remove event in server at the same time
 *
 * @param mgt    server manager context
 * @param server server to be removed
 *
 * @return 0 for success, negative errorcode otherwise
 *
 */
int dt_remove_server(dt_server_mgt_t *mgt, event_server_t * server);

/* *
 * Alloc an event_t
 *
 * @param server event server to be sent
 * @param type   event type
 *
 * @return event_t pointer for success, NULL otherwise
 *
 */
event_t *dt_alloc_event(int server, int type);

/* *
 * Send event to server manger, and make sure event
 * transport dest server before return
 *
 * @param mgt   server manager context
 * @param event event to be sent
 *
 * @return 0 for success, negative errorcode otherwise
 *
 */
int dt_send_event_sync(dt_server_mgt_t *mgt, event_t * event);

/* *
 * Send event to server manger - non block
 *
 * @param mgt   server manager context
 * @param event event to be sent
 *
 * @return 0 for success, negative errorcode otherwise
 *
 */
int dt_send_event(dt_server_mgt_t *mgt, event_t * event);

/* *
 * Get event from server
 *
 * @param server server context
 *
 * @return first event pointer for success, NULL if no event in server
 * remove event from server
 *
 */
event_t *dt_get_event(event_server_t * server);

/* *
 * Query event from server
 *
 * @param server server context
 *
 * @return first event pointer for success, NULL if no event in server
 * not remove event from server
 *
 */
event_t *dt_peek_event(event_server_t * server);

#endif
