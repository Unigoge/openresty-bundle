OPENRESTY_PREFIX=/usr/local/openresty

PREFIX ?=          /usr/local/openresty
LUA_LIB_DIR ?=     $(PREFIX)/lib/lua/$(LUA_VERSION)
INSTALL ?= install

.PHONY: all test install

all: ;

install: all
	$(INSTALL) -d $(LUA_LIB_DIR)
	$(INSTALL) router.lua $(LUA_LIB_DIR)

test: all
	PATH=$(OPENRESTY_PREFIX)/nginx/sbin:$$PATH prove -I../test-nginx/lib -r t
