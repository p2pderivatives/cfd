noinst_LTLIBRARIES =
lib_LTLIBRARIES = libcfd.la

# for common
if DEBUG
OPT = -g3 -O0
else
OPT = -O3
endif
CFLAGS= -Wall -Wextra $(OPT)
CPPFLAGS= $(CFLAGS) -std=c++11
CXXFLAGS= $(CFLAGS) -std=c++11

include Makefile.srclist

PATH_CFDCORE= ../../cfd-core

if TARGET_WINDOWS
# Windows option
LINK_OPTS= -fstack-protector -static -lstdc++ -lgcc -lwinpthread -lssp
cfd_CFLAGS_OPT=
if SHARED_BUILD_ENABLED
cfd_LDFLAGS_OPT= -shared -no-undefined
else
cfd_LDFLAGS_OPT= -no-undefined
endif
LIB_OPTS = -L$(PATH_CFDCORE)/src/univalue/.libs -lunivalue-0 \
           -L$(PATH_CFDCORE)/src/.libs -lcfdcore-0
else
if TARGET_DARWIN
# mac option
LINK_OPTS= -lstdc++ -lgcc -lpthread
cfd_CFLAGS_OPT=
cfd_LDFLAGS_OPT=
LIB_OPTS = -L$(PATH_CFDCORE)/src/univalue/.libs -lunivalue \
           -L$(PATH_CFDCORE)/src/.libs -lcfdcore
else
# other(linux, etc.) option
LINK_OPTS= -lstdc++ -lgcc -lpthread
cfd_CFLAGS_OPT=
cfd_LDFLAGS_OPT=
LIB_OPTS = -L$(PATH_CFDCORE)/src/univalue/.libs -lunivalue \
           -L$(PATH_CFDCORE)/src/.libs -lcfdcore
endif
endif

# for cfdcore
libcfd_la_LDFLAGS= $(cfd_LDFLAGS_OPT) $(LINK_OPTS)
#libcfd_la_LDADD=$(LIB_OPTS)
libcfd_la_LIBADD=$(LIB_OPTS)
libcfd_la_CFLAGS= -DCFD_BUILD=1 -I"." -I"../include" \
    -I$(PATH_CFDCORE)/include -I$(PATH_CFDCORE)/src/include \
    -I$(PATH_CFDCORE)/src/univalue/include \
    $(cfd_CFLAGS_OPT) $(AM_CFLAGS)
libcfd_la_CXXFLAGS= $(libcfd_la_CFLAGS)
libcfd_la_SOURCES= $(CFD_SOURCES)

