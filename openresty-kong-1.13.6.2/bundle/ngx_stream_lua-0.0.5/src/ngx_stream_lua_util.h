
/*
 * !!! DO NOT EDIT DIRECTLY !!!
 * This file was automatically generated from the following template:
 *
 * src/subsys/ngx_subsys_lua_util.h.tt2
 */


/*
 * Copyright (C) Xiaozhe Wang (chaoslawful)
 * Copyright (C) Yichun Zhang (agentzh)
 */


#ifndef _NGX_STREAM_LUA_UTIL_H_INCLUDED_
#define _NGX_STREAM_LUA_UTIL_H_INCLUDED_


#include "ngx_stream_lua_common.h"


#ifndef NGX_UNESCAPE_URI_COMPONENT
#define NGX_UNESCAPE_URI_COMPONENT  0
#endif


#ifndef NGX_LUA_NO_FFI_API
typedef struct {
    int          len;
    /* this padding hole on 64-bit systems is expected */
    u_char      *data;
} ngx_stream_lua_ffi_str_t;


typedef struct {
    ngx_stream_lua_ffi_str_t         key;
    ngx_stream_lua_ffi_str_t         value;
} ngx_stream_lua_ffi_table_elt_t;
#endif /* NGX_LUA_NO_FFI_API */


/* char whose address we use as the key in Lua vm registry for
 * user code cache table */
extern char ngx_stream_lua_code_cache_key;


/* key in Lua vm registry for all the "ngx.ctx" tables */
#define ngx_stream_lua_ctx_tables_key  "ngx_lua_ctx_tables"


/* char whose address we use as the key in Lua vm registry for
 * regex cache table  */
extern char ngx_stream_lua_regex_cache_key;

/* char whose address we use as the key in Lua vm registry for
 * socket connection pool table */
extern char ngx_stream_lua_socket_pool_key;

/* char whose address we use as the key for the coroutine parent relationship */
extern char ngx_stream_lua_coroutine_parents_key;

/* coroutine anchoring table key in Lua VM registry */
extern char ngx_stream_lua_coroutines_key;

/* key to the metatable for ngx.req.get_headers() and ngx.resp.get_headers() */
extern char ngx_stream_lua_headers_metatable_key;


#ifndef ngx_str_set
#define ngx_str_set(str, text)                                               \
    (str)->len = sizeof(text) - 1; (str)->data = (u_char *) text
#endif




#if defined(nginx_version) && nginx_version < 1000000
#define ngx_memmove(dst, src, n)   (void) memmove(dst, src, n)
#endif


#define ngx_stream_lua_context_name(c)                                       \
    ((c) == NGX_STREAM_LUA_CONTEXT_CONTENT ? "content_by_lua*"               \
     : (c) == NGX_STREAM_LUA_CONTEXT_LOG ? "log_by_lua*"                     \
     : (c) == NGX_STREAM_LUA_CONTEXT_TIMER ? "ngx.timer"                     \
     : (c) == NGX_STREAM_LUA_CONTEXT_INIT_WORKER ? "init_worker_by_lua*"     \
     : (c) == NGX_STREAM_LUA_CONTEXT_BALANCER ? "balancer_by_lua*"           \
     : (c) == NGX_STREAM_LUA_CONTEXT_PREREAD ? "preread_by_lua*"             \
     : (c) == NGX_STREAM_LUA_CONTEXT_SSL_CERT ? "ssl_certificate_by_lua*"    \
     : "(unknown)")


#define ngx_stream_lua_check_context(L, ctx, flags)                          \
    if (!((ctx)->context & (flags))) {                                       \
        return luaL_error(L, "API disabled in the context of %s",            \
                          ngx_stream_lua_context_name((ctx)->context));      \
    }


static ngx_inline ngx_int_t
ngx_stream_lua_ffi_check_context(ngx_stream_lua_ctx_t *ctx,
    unsigned flags, u_char *err, size_t *errlen)
{
    if (!(ctx->context & flags)) {
        *errlen = ngx_snprintf(err, *errlen,
                               "API disabled in the context of %s",
                               ngx_stream_lua_context_name((ctx)->context))
                  - err;

        return NGX_DECLINED;
    }

    return NGX_OK;
}


#define ngx_stream_lua_check_fake_request(L, r)                              \
    if ((r)->connection->fd == (ngx_socket_t) -1) {                          \
        return luaL_error(L, "API disabled in the current context");         \
    }


#define ngx_stream_lua_check_fake_request2(L, r, ctx)                        \
    if ((r)->connection->fd == (ngx_socket_t) -1) {                          \
        return luaL_error(L, "API disabled in the context of %s",            \
                          ngx_stream_lua_context_name((ctx)                  \
                          ->context));                                       \
    }


#define ngx_stream_lua_ssl_get_ctx(ssl_conn)                                 \
    SSL_get_ex_data(ssl_conn, ngx_stream_lua_ssl_ctx_index)

#if (OPENSSL_VERSION_NUMBER < 0x10100001L)
#define SSL_CTX_up_ref(ctx)                                                  \
    CRYPTO_add(&(ctx)->references, 1, CRYPTO_LOCK_SSL_CTX)
#endif

lua_State *ngx_stream_lua_init_vm(lua_State *parent_vm, ngx_cycle_t *cycle,
    ngx_pool_t *pool, ngx_stream_lua_main_conf_t *lmcf, ngx_log_t *log,
    ngx_pool_cleanup_t **pcln);

lua_State *ngx_stream_lua_new_thread(ngx_stream_lua_request_t *r, lua_State *l,
    int *ref);

u_char *ngx_stream_lua_rebase_path(ngx_pool_t *pool, u_char *src, size_t len);

ngx_int_t ngx_stream_lua_send_header_if_needed(ngx_stream_lua_request_t *r,
    ngx_stream_lua_ctx_t *ctx);

ngx_int_t ngx_stream_lua_send_chain_link(ngx_stream_lua_request_t *r,
    ngx_stream_lua_ctx_t *ctx, ngx_chain_t *cl);

void ngx_stream_lua_discard_bufs(ngx_pool_t *pool, ngx_chain_t *in);

ngx_int_t ngx_stream_lua_add_copy_chain(ngx_stream_lua_request_t *r,
    ngx_stream_lua_ctx_t *ctx, ngx_chain_t ***plast, ngx_chain_t *in,
    ngx_int_t *eof);

void ngx_stream_lua_reset_ctx(ngx_stream_lua_request_t *r, lua_State *L,
    ngx_stream_lua_ctx_t *ctx);

void ngx_stream_lua_generic_phase_post_read(ngx_stream_lua_request_t *r);

void ngx_stream_lua_request_cleanup(ngx_stream_lua_ctx_t *ctx, int foricible);

void ngx_stream_lua_request_cleanup_handler(void *data);

ngx_int_t ngx_stream_lua_run_thread(lua_State *L, ngx_stream_lua_request_t *r,
    ngx_stream_lua_ctx_t *ctx, volatile int nret);

ngx_int_t ngx_stream_lua_wev_handler(ngx_stream_lua_request_t *r);

u_char *ngx_stream_lua_digest_hex(u_char *dest, const u_char *buf,
    int buf_len);

void ngx_stream_lua_set_multi_value_table(lua_State *L, int index);

void ngx_stream_lua_unescape_uri(u_char **dst, u_char **src, size_t size,
    ngx_uint_t type);

uintptr_t ngx_stream_lua_escape_uri(u_char *dst, u_char *src,
    size_t size, ngx_uint_t type);

void ngx_stream_lua_inject_req_api(ngx_log_t *log, lua_State *L);

void ngx_stream_lua_process_args_option(ngx_stream_lua_request_t *r,
    lua_State *L, int table, ngx_str_t *args);

ngx_int_t ngx_stream_lua_open_and_stat_file(u_char *name,
    ngx_open_file_info_t *of, ngx_log_t *log);

ngx_chain_t *ngx_stream_lua_chain_get_free_buf(ngx_log_t *log, ngx_pool_t *p,
    ngx_chain_t **free, size_t len);

void ngx_stream_lua_create_new_globals_table(lua_State *L, int narr, int nrec);

int ngx_stream_lua_traceback(lua_State *L);

ngx_stream_lua_co_ctx_t *ngx_stream_lua_get_co_ctx(lua_State *L,
    ngx_stream_lua_ctx_t *ctx);

ngx_stream_lua_co_ctx_t *ngx_stream_lua_create_co_ctx(
    ngx_stream_lua_request_t *r, ngx_stream_lua_ctx_t *ctx);

ngx_int_t ngx_stream_lua_run_posted_threads(ngx_connection_t *c, lua_State *L,
    ngx_stream_lua_request_t *r, ngx_stream_lua_ctx_t *ctx, ngx_uint_t nreqs);

ngx_int_t ngx_stream_lua_post_thread(ngx_stream_lua_request_t *r,
    ngx_stream_lua_ctx_t *ctx, ngx_stream_lua_co_ctx_t *coctx);

void ngx_stream_lua_del_thread(ngx_stream_lua_request_t *r, lua_State *L,
    ngx_stream_lua_ctx_t *ctx, ngx_stream_lua_co_ctx_t *coctx);

void ngx_stream_lua_rd_check_broken_connection(ngx_stream_lua_request_t *r);

ngx_int_t ngx_stream_lua_test_expect(ngx_stream_lua_request_t *r);

ngx_int_t ngx_stream_lua_check_broken_connection(ngx_stream_lua_request_t *r,
    ngx_event_t *ev);

void ngx_stream_lua_finalize_request(ngx_stream_lua_request_t *r, ngx_int_t rc);

void ngx_stream_lua_finalize_fake_request(ngx_stream_lua_request_t *r,
    ngx_int_t rc);

void ngx_stream_lua_close_fake_connection(ngx_connection_t *c);

void ngx_stream_lua_free_fake_request(ngx_stream_lua_request_t *r);

void ngx_stream_lua_release_ngx_ctx_table(ngx_log_t *log, lua_State *L,
    ngx_stream_lua_ctx_t *ctx);

void ngx_stream_lua_cleanup_vm(void *data);

ngx_connection_t *ngx_stream_lua_create_fake_connection(ngx_pool_t *pool);

ngx_stream_lua_request_t *
    ngx_stream_lua_create_fake_request(ngx_stream_session_t *s);

ngx_stream_session_t *ngx_stream_lua_create_fake_session(ngx_connection_t *c);

ngx_int_t ngx_stream_lua_report(ngx_log_t *log, lua_State *L, int status,
    const char *prefix);

int ngx_stream_lua_do_call(ngx_log_t *log, lua_State *L);



void ngx_stream_lua_cleanup_free(ngx_stream_lua_request_t *r,
    ngx_stream_lua_cleanup_pt *cleanup);


#define ngx_stream_lua_check_if_abortable(L, ctx)                            \
    if ((ctx)->no_abort) {                                                   \
        return luaL_error(L, "attempt to abort with pending subrequests");   \
    }


static ngx_inline void
ngx_stream_lua_init_ctx(ngx_stream_lua_request_t *r, ngx_stream_lua_ctx_t *ctx)
{
    ngx_memzero(ctx, sizeof(ngx_stream_lua_ctx_t));
    ctx->ctx_ref = LUA_NOREF;
    ctx->entry_co_ctx.co_ref = LUA_NOREF;
    ctx->resume_handler = ngx_stream_lua_wev_handler;
    ctx->request = r;
}


static ngx_inline ngx_stream_lua_ctx_t *
ngx_stream_lua_create_ctx(ngx_stream_session_t *r)
{
    lua_State                           *L;
    ngx_stream_lua_ctx_t                *ctx;
    ngx_pool_cleanup_t                  *cln;
    ngx_stream_lua_loc_conf_t           *llcf;
    ngx_stream_lua_main_conf_t          *lmcf;

    ngx_stream_lua_request_t               *sreq;

    ctx = ngx_palloc(r->connection->pool, sizeof(ngx_stream_lua_ctx_t));
    if (ctx == NULL) {
        return NULL;
    }

    sreq = ngx_stream_lua_create_request(r);

    if (sreq == NULL) {
        return NULL;
    }

    ngx_stream_lua_init_ctx(sreq, ctx);

    ngx_stream_set_ctx(r, ctx, ngx_stream_lua_module);

    llcf = ngx_stream_get_module_srv_conf(r, ngx_stream_lua_module);

    if (!llcf->enable_code_cache && r->connection->fd != (ngx_socket_t) -1) {
        lmcf = ngx_stream_get_module_main_conf(r, ngx_stream_lua_module);

        dd("lmcf: %p", lmcf);

        /*
         * caveats: we need to move the vm cleanup hook to the list end
         * to ensure it will be executed *after* the request cleanup
         * hook registered by ngx_stream_lua_create_request to preserve
         * the correct semantics.
         */

        L = ngx_stream_lua_init_vm(lmcf->lua, lmcf->cycle, sreq->pool, lmcf,
                                   r->connection->log, &cln);

        while (cln->next != NULL) {
            cln = cln->next;
        }

        cln->next = sreq->pool->cleanup;

        cln = sreq->pool->cleanup;
        sreq->pool->cleanup = cln->next;
        cln->next = NULL;

        if (L == NULL) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                          "failed to initialize Lua VM");
            return NULL;
        }

        if (lmcf->init_handler) {
            if (lmcf->init_handler(r->connection->log, lmcf, L) != NGX_OK) {
                /* an error happened */
                return NULL;
            }
        }

        ctx->vm_state = cln->data;

    } else {
        ctx->vm_state = NULL;
    }

    return ctx;
}


static ngx_inline lua_State *
ngx_stream_lua_get_lua_vm(ngx_stream_lua_request_t *r,
    ngx_stream_lua_ctx_t *ctx)
{
    ngx_stream_lua_main_conf_t          *lmcf;

    if (ctx == NULL) {
        ctx = ngx_stream_lua_get_module_ctx(r, ngx_stream_lua_module);
    }

    if (ctx && ctx->vm_state) {
        return ctx->vm_state->vm;
    }

    lmcf = ngx_stream_lua_get_module_main_conf(r, ngx_stream_lua_module);
    dd("lmcf->lua: %p", lmcf->lua);
    return lmcf->lua;
}


#define ngx_stream_lua_req_key  "__ngx_req"


static ngx_inline ngx_stream_lua_request_t *
ngx_stream_lua_get_req(lua_State *L)
{
    ngx_stream_lua_request_t    *r;

    lua_getglobal(L, ngx_stream_lua_req_key);
    r = lua_touserdata(L, -1);
    lua_pop(L, 1);

    return r;
}


static ngx_inline void
ngx_stream_lua_set_req(lua_State *L, ngx_stream_lua_request_t *r)
{
    lua_pushlightuserdata(L, r);
    lua_setglobal(L, ngx_stream_lua_req_key);
}


static ngx_inline void
ngx_stream_lua_get_globals_table(lua_State *L)
{
    lua_pushvalue(L, LUA_GLOBALSINDEX);
}


static ngx_inline void
ngx_stream_lua_set_globals_table(lua_State *L)
{
    lua_replace(L, LUA_GLOBALSINDEX);
}


#define ngx_stream_lua_hash_literal(s)                                       \
    ngx_stream_lua_hash_str((u_char *) s, sizeof(s) - 1)


static ngx_inline ngx_uint_t
ngx_stream_lua_hash_str(u_char *src, size_t n)
{
    ngx_uint_t  key;

    key = 0;

    while (n--) {
        key = ngx_hash(key, *src);
        src++;
    }

    return key;
}




static ngx_inline void
ngx_stream_lua_cleanup_pending_operation(ngx_stream_lua_co_ctx_t *coctx)
{
    if (coctx->cleanup) {
        coctx->cleanup(coctx);
        coctx->cleanup = NULL;
    }
}


static ngx_inline ngx_chain_t *
ngx_stream_lua_get_flush_chain(ngx_stream_lua_request_t *r,
    ngx_stream_lua_ctx_t *ctx)
{
    ngx_chain_t  *cl;

    cl = ngx_stream_lua_chain_get_free_buf(r->connection->log, r->pool,
                                           &ctx->free_bufs, 0);
    if (cl == NULL) {
        return NULL;
    }

    cl->buf->flush = 1;

    return cl;
}


#if (nginx_version < 1011002)
static ngx_inline in_port_t
ngx_inet_get_port(struct sockaddr *sa)
{
    struct sockaddr_in   *sin;
#if (NGX_HAVE_INET6)
    struct sockaddr_in6  *sin6;
#endif

    switch (sa->sa_family) {

#if (NGX_HAVE_INET6)
    case AF_INET6:
        sin6 = (struct sockaddr_in6 *) sa;
        return ntohs(sin6->sin6_port);
#endif

#if (NGX_HAVE_UNIX_DOMAIN)
    case AF_UNIX:
        return 0;
#endif

    default: /* AF_INET */
        sin = (struct sockaddr_in *) sa;
        return ntohs(sin->sin_port);
    }
}
#endif


extern ngx_uint_t  ngx_stream_lua_location_hash;
extern ngx_uint_t  ngx_stream_lua_content_length_hash;


#endif /* _NGX_STREAM_LUA_UTIL_H_INCLUDED_ */

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
