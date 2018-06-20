# Boiler plate
ACLOCAL_AMFLAGS = -I m4 --install

# General settings
AM_LDFLAGS = -export-symbols-regex '^luaopen_' -module -avoid-version

EXTRA_DIST = LICENSE

# binaries to create
bin_PROGRAMS = ldms_monitor 
check_PROGRAMS = test_ldms_monitor

# per-binary settings
ldms_monitor_SOURCES = src/ldms_monitor.c
ldms_monitor_CFLAGS = $(LUA_INCLUDE) ${CZMQ_CFLAGS} ${ZMQ_CFLAGS}
ldms_monitor_LDFLAGS = $(LUA_LIB) ${CZMQ_LIBS} ${ZMQ_LIBS}

test_ldms_monitor_SOURCES = test/test_ldms_monitor.c ./Unity/src/unity.c
test_ldms_monitor_CFLAGS = -I./Unity/src -I./lib

TESTS = $(check_PROGRAMS)
