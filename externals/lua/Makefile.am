# Proxy automake file for the Makefile provided by Lua.

LUA_MAKEFILE = $(top_srcdir)/externals/lua/lua.mk

dist_sources = ${shell find . -name "*.c" -or -name "*.h"}
EXTRA_DIST = $(dist_sources) lua.mk COPYRIGHT

# Export the VPATH variable to the Makefile provided by Lua.
export VPATH

all-local:
	$(MAKE) -f $(LUA_MAKEFILE) $(AM_MAKEFLAGS) MYCFLAGS="$(FD_CFLAGS)" liblua.a

clean-local:
	$(MAKE) -f $(LUA_MAKEFILE) $(AM_MAKEFLAGS) clean

