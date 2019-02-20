
OPENRESTY_PREFIX=/usr/local/openresty

PREFIX ?=          /usr/local/openresty
LUA_LIB_DIR ?=     $(PREFIX)/lib/lua/$(LUA_VERSION)
INSTALL ?= install

.PHONY: all install

all: ;

install: all
        $(INSTALL) -d $(LUA_LIB_DIR)/resty
        $(INSTALL) lib/resty/*.lua $(LUA_LIB_DIR)/resty

