

#include "httpd.h"
#include "mgos_http_server.h"


static struct httpd_ops _ops;

static void mg_ev_handler(struct mg_connection* nc, int ev, void *p, void* user_data) {
    switch (ev)     {
        case MG_EV_ACCEPT: {
            if (_ops.accept) {
                _ops.accept(user_data, nc);
            }

            char addr[32];
            mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                                MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
            printf("[HTTPD] Connection %p from %s\n", nc, addr);
            break;
        }
        case MG_EV_RECV: {
            printf("[HTTPD] MG_EV_RECV\n");
            if (_ops.recv) {
                _ops.recv(user_data, nc, nc->recv_mbuf.buf, nc->recv_mbuf.len);
            }
            break;
        }
        case MG_EV_POLL: {
            break;
        }
        case MG_EV_CLOSE: {
            printf("[HTTPD] MG_EV_CLOSE");
            printf("Connection %p closed\n", nc);
            if (_ops.close) {
                _ops.close(user_data, nc);
            }
            break;
        }
        case MG_EV_SEND: {
            printf("[HTTPD] MG_EV_SEND. %d\n", *((int*)user_data));
            break;
        }
        default: {
            printf("[HTTPD] DEFAULT:%d\n", ev);
            break;
        }
    }
}

void httpd_register(struct httpd_ops* ops, void* user_data) {
    _ops = *ops;
    mgos_register_http_endpoint("/", mg_ev_handler, user_data);
}
