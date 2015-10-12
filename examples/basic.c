/* basic.c
** libstrophe XMPP client library -- basic usage example
**
** Copyright (C) 2005-2009 Collecta, Inc.
**
**  This software is provided AS-IS with no warranty, either express
**  or implied.
**
** This program is dual licensed under the MIT and GPLv3 licenses.
*/

#include <stdio.h>
#include <string.h>

#include <mesode.h>


/* define a handler for connection events */
void conn_handler(xmpp_conn_t * const conn, const xmpp_conn_event_t status,
                  const int error, xmpp_stream_error_t * const stream_error,
                  void * const userdata)
{
    xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;
    int secured;

    if (status == XMPP_CONN_CONNECT) {
        fprintf(stderr, "DEBUG: connected\n");
        secured = xmpp_conn_is_secured(conn);
        fprintf(stderr, "DEBUG: connection is %s.\n",
                secured ? "secured" : "NOT secured");
        xmpp_disconnect(conn);
    }
    else {
        fprintf(stderr, "DEBUG: disconnected\n");
        xmpp_stop(ctx);
    }
}

int main(int argc, char **argv)
{
    xmpp_ctx_t *ctx;
    xmpp_conn_t *conn;
    xmpp_log_t *log;
    char *jid, *pass, *host = NULL;
    int i;
    int opt_disable_tls = 0;
    int opt_old_ssl = 0;

    /* take a jid and password on the command line */
    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--disable-tls") == 0)
            opt_disable_tls = 1;
        else if (strcmp(argv[i], "--old-ssl") == 0)
            opt_old_ssl = 1;
        else
            break;
    }
    if ((argc - i) < 2 || (argc - i) > 3) {
        fprintf(stderr, "Usage: basic [options] <jid> <pass> [<host>]\n\n"
                        "Options:\n"
                        "  --disable-tls        Disable TLS.\n"
                        "  --old-ssl            Use old style SSL.\n");
        return 1;
    }

    jid = argv[i];
    pass = argv[i + 1];
    if (i + 2 < argc)
        host = argv[i + 2];

    /* init library */
    xmpp_initialize();

    /* create a context */
    log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG); /* pass NULL instead to silence output */
    ctx = xmpp_ctx_new(NULL, log);

    /* create a connection */
    conn = xmpp_conn_new(ctx);

    /* configure connection properties (optional) */
    if (opt_disable_tls)
        xmpp_conn_disable_tls(conn);
    if (opt_old_ssl)
        xmpp_conn_set_old_style_ssl(conn);

    /* setup authentication information */
    xmpp_conn_set_jid(conn, jid);
    xmpp_conn_set_pass(conn, pass);

    /* initiate connection */
    xmpp_connect_client(conn, host, 0, NULL, conn_handler, ctx);

    /* enter the event loop -
       our connect handler will trigger an exit */
    xmpp_run(ctx);

    /* release our connection and context */
    xmpp_conn_release(conn);
    xmpp_ctx_free(ctx);

    /* final shutdown of the library */
    xmpp_shutdown();

    return 0;
}
