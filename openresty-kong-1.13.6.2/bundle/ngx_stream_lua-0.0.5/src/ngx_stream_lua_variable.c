
/*
 * !!! DO NOT EDIT DIRECTLY !!!
 * This file was automatically generated from the following template:
 *
 * src/subsys/ngx_subsys_lua_variable.c.tt2
 */


/*
 * Copyright (C) Xiaozhe Wang (chaoslawful)
 * Copyright (C) Yichun Zhang (agentzh)
 */


#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"


#include "ngx_stream_lua_variable.h"
#include "ngx_stream_lua_util.h"


static int ngx_stream_lua_var_get(lua_State *L);
static int ngx_stream_lua_var_set(lua_State *L);


void
ngx_stream_lua_inject_variable_api(lua_State *L)
{
    /* {{{ register reference maps */
    lua_newtable(L);    /* ngx.var */

    lua_createtable(L, 0, 2 /* nrec */); /* metatable for .var */
    lua_pushcfunction(L, ngx_stream_lua_var_get);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, ngx_stream_lua_var_set);
    lua_setfield(L, -2, "__newindex");
    lua_setmetatable(L, -2);

    lua_setfield(L, -2, "var");
}


/**
 * Get nginx internal variables content
 *
 * @retval Always return a string or nil on Lua stack. Return nil when failed
 * to get content, and actual content string when found the specified variable.
 * @seealso ngx_stream_lua_var_set
 * */
static int
ngx_stream_lua_var_get(lua_State *L)
{
    ngx_stream_lua_request_t    *r;
    u_char                      *p, *lowcase;
    size_t                       len;
    ngx_uint_t                   hash;
    ngx_str_t                    name;

    ngx_stream_variable_value_t         *vv;

    r = ngx_stream_lua_get_req(L);
    if (r == NULL) {
        return luaL_error(L, "no request object found");
    }

    ngx_stream_lua_check_fake_request(L, r);


    if (lua_type(L, -1) != LUA_TSTRING) {
        return luaL_error(L, "bad variable name");
    }

    p = (u_char *) lua_tolstring(L, -1, &len);

    lowcase = lua_newuserdata(L, len);

    hash = ngx_hash_strlow(lowcase, p, len);

    name.len = len;
    name.data = lowcase;

    vv = ngx_stream_get_variable(r->session, &name, hash);

    if (vv == NULL || vv->not_found) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, (const char *) vv->data, (size_t) vv->len);
    return 1;
}


/**
 * Set nginx internal variable content
 *
 * @retval Always return a boolean on Lua stack. Return true when variable
 * content was modified successfully, false otherwise.
 * @seealso ngx_stream_lua_var_get
 * */
static int
ngx_stream_lua_var_set(lua_State *L)
{
    u_char                      *p, *lowcase, *val;
    size_t                       len;
    ngx_str_t                    name;
    ngx_uint_t                   hash;
    ngx_stream_lua_request_t    *r;
    int                          value_type;
    const char                  *msg;

    ngx_stream_variable_t               *v;
    ngx_stream_variable_value_t         *vv;
    ngx_stream_core_main_conf_t         *cmcf;

    r = ngx_stream_lua_get_req(L);
    if (r == NULL) {
        return luaL_error(L, "no request object found");
    }

    ngx_stream_lua_check_fake_request(L, r);

    /* we skip the first argument that is the table */

    /* we read the variable name */

    if (lua_type(L, 2) != LUA_TSTRING) {
        return luaL_error(L, "bad variable name");
    }

    p = (u_char *) lua_tolstring(L, 2, &len);

    lowcase = lua_newuserdata(L, len + 1);

    hash = ngx_hash_strlow(lowcase, p, len);
    lowcase[len] = '\0';

    name.len = len;
    name.data = lowcase;

    /* we read the variable new value */

    value_type = lua_type(L, 3);
    switch (value_type) {
    case LUA_TNUMBER:
    case LUA_TSTRING:
        p = (u_char *) luaL_checklstring(L, 3, &len);

        val = ngx_palloc(r->connection->pool, len);
        if (val == NULL) {
            return luaL_error(L, "memory allocation error");
        }

        ngx_memcpy(val, p, len);

        break;

    case LUA_TNIL:
        /* undef the variable */

        val = NULL;
        len = 0;

        break;

    default:
        msg = lua_pushfstring(L, "string, number, or nil expected, "
                              "but got %s", lua_typename(L, value_type));
        return luaL_argerror(L, 1, msg);
    }

    /* we fetch the variable itself */

    cmcf = ngx_stream_lua_get_module_main_conf(r, ngx_stream_core_module);

    v = ngx_hash_find(&cmcf->variables_hash, hash, name.data, name.len);

    if (v) {
        if (!(v->flags & NGX_STREAM_VAR_CHANGEABLE)) {
            return luaL_error(L, "variable \"%s\" not changeable", lowcase);
        }

        if (v->set_handler) {

            dd("set variables with set_handler");

            vv = ngx_palloc(r->connection->pool,
                            sizeof(ngx_stream_variable_value_t));
            if (vv == NULL) {
                return luaL_error(L, "no memory");
            }

            if (value_type == LUA_TNIL) {
                vv->valid = 0;
                vv->not_found = 1;
                vv->no_cacheable = 0;
                vv->data = NULL;
                vv->len = 0;

            } else {
                vv->valid = 1;
                vv->not_found = 0;
                vv->no_cacheable = 0;

                vv->data = val;
                vv->len = len;
            }

            v->set_handler(r->session, vv, v->data);

            return 0;
        }

        if (v->flags & NGX_STREAM_VAR_INDEXED) {
            vv = &r->session->variables[v->index];

            dd("set indexed variable");

            if (value_type == LUA_TNIL) {
                vv->valid = 0;
                vv->not_found = 1;
                vv->no_cacheable = 0;

                vv->data = NULL;
                vv->len = 0;

            } else {
                vv->valid = 1;
                vv->not_found = 0;
                vv->no_cacheable = 0;

                vv->data = val;
                vv->len = len;
            }

            return 0;
        }

        return luaL_error(L, "variable \"%s\" cannot be assigned a value",
                          lowcase);
    }

    /* variable not found */

    return luaL_error(L, "variable \"%s\" not found for writing; "
                      "maybe it is a built-in variable that is not changeable "
                      "or you forgot to use \"set $%s '';\" "
                      "in the config file to define it first",
                      lowcase, lowcase);
}


#ifndef NGX_LUA_NO_FFI_API
int
ngx_stream_lua_ffi_var_get(ngx_stream_lua_request_t *r, u_char *name_data,
    size_t name_len, u_char *lowcase_buf, int capture_id, u_char **value,
    size_t *value_len, char **err)
{
    ngx_uint_t                   hash;
    ngx_str_t                    name;

    ngx_stream_variable_value_t         *vv;

    if (r == NULL) {
        *err = "no request object found";
        return NGX_ERROR;
    }

    if ((r)->connection->fd == (ngx_socket_t) -1) {
        *err = "API disabled in the current context";
        return NGX_ERROR;
    }

    hash = ngx_hash_strlow(lowcase_buf, name_data, name_len);

    name.data = lowcase_buf;
    name.len = name_len;

    dd("variable name: %.*s", (int) name_len, lowcase_buf);

    vv = ngx_stream_get_variable(r->session, &name, hash);

    if (vv == NULL || vv->not_found) {
        return NGX_DECLINED;
    }

    *value = vv->data;
    *value_len = vv->len;
    return NGX_OK;
}


int
ngx_stream_lua_ffi_var_set(ngx_stream_lua_request_t *r, u_char *name_data,
    size_t name_len, u_char *lowcase_buf, u_char *value, size_t value_len,
    u_char *errbuf, size_t *errlen)
{
    u_char                      *p;
    ngx_uint_t                   hash;

    ngx_stream_variable_t               *v;
    ngx_stream_variable_value_t         *vv;
    ngx_stream_core_main_conf_t         *cmcf;

    if (r == NULL) {
        *errlen = ngx_snprintf(errbuf, *errlen, "no request object found")
                  - errbuf;
        return NGX_ERROR;
    }

    if ((r)->connection->fd == (ngx_socket_t) -1) {
        *errlen = ngx_snprintf(errbuf, *errlen,
                               "API disabled in the current context")
                  - errbuf;
        return NGX_ERROR;
    }

    hash = ngx_hash_strlow(lowcase_buf, name_data, name_len);

    dd("variable name: %.*s", (int) name_len, lowcase_buf);

    /* we fetch the variable itself */

    cmcf = ngx_stream_lua_get_module_main_conf(r, ngx_stream_core_module);

    v = ngx_hash_find(&cmcf->variables_hash, hash, lowcase_buf, name_len);

    if (v) {
        if (!(v->flags & NGX_STREAM_VAR_CHANGEABLE)) {
            dd("variable not changeable");
            *errlen = ngx_snprintf(errbuf, *errlen,
                                   "variable \"%*s\" not changeable",
                                   name_len, lowcase_buf)
                      - errbuf;
            return NGX_ERROR;
        }

        if (v->set_handler) {

            dd("set variables with set_handler");

            if (value != NULL && value_len) {
                vv = ngx_palloc(r->connection->pool,
                                sizeof(ngx_stream_variable_value_t)
                                + value_len);
                if (vv == NULL) {
                    goto nomem;
                }

                p = (u_char *) vv + sizeof(ngx_stream_variable_value_t);
                ngx_memcpy(p, value, value_len);
                value = p;

            } else {
                vv = ngx_palloc(r->connection->pool,
                                sizeof(ngx_stream_variable_value_t));
                if (vv == NULL) {
                    goto nomem;
                }
            }

            if (value == NULL) {
                vv->valid = 0;
                vv->not_found = 1;
                vv->no_cacheable = 0;
                vv->data = NULL;
                vv->len = 0;

            } else {
                vv->valid = 1;
                vv->not_found = 0;
                vv->no_cacheable = 0;

                vv->data = value;
                vv->len = value_len;
            }

            v->set_handler(r->session, vv, v->data);
            return NGX_OK;
        }

        if (v->flags & NGX_STREAM_VAR_INDEXED) {
            vv = &r->session->variables[v->index];

            dd("set indexed variable");

            if (value == NULL) {
                vv->valid = 0;
                vv->not_found = 1;
                vv->no_cacheable = 0;

                vv->data = NULL;
                vv->len = 0;

            } else {
                p = ngx_palloc(r->connection->pool, value_len);
                if (p == NULL) {
                    goto nomem;
                }

                ngx_memcpy(p, value, value_len);
                value = p;

                vv->valid = 1;
                vv->not_found = 0;
                vv->no_cacheable = 0;

                vv->data = value;
                vv->len = value_len;
            }

            return NGX_OK;
        }

        *errlen = ngx_snprintf(errbuf, *errlen,
                               "variable \"%*s\" cannot be assigned "
                               "a value", name_len, lowcase_buf)
                  - errbuf;
        return NGX_ERROR;
    }

    /* variable not found */

    *errlen = ngx_snprintf(errbuf, *errlen,
                           "variable \"%*s\" not found for writing; "
                           "maybe it is a built-in variable that is not "
                           "changeable or you forgot to use \"set $%*s '';\" "
                           "in the config file to define it first",
                           name_len, lowcase_buf, name_len, lowcase_buf)
              - errbuf;
    return NGX_ERROR;

nomem:

    *errlen = ngx_snprintf(errbuf, *errlen, "no memory") - errbuf;
    return NGX_ERROR;
}
#endif /* NGX_LUA_NO_FFI_API */


/* vi:set ft=c ts=4 sw=4 et fdm=marker: */