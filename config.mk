# st version
VERSION = 0.5

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# includes
INCS-${configGLIB} += -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include
INCS = -I. -I/usr/include -I${X11INC} ${INCS-1} \
       `pkg-config --cflags fontconfig` \
       `pkg-config --cflags freetype2`

# libs
LIBS-${configGD} += -lgd
LIBS-${configGLIB} += -lglib-2.0
LIBS = -L/usr/lib -lc -L${X11LIB} -lm -lrt -lX11 -lutil -lXext -lXft ${LIBS-1} \
       `pkg-config --libs fontconfig`  \
       `pkg-config --libs freetype2`

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" -D_BSD_SOURCE -D_XOPEN_SOURCE=600
CFLAGS += -g -std=c99 -pedantic -Wall -Wvariadic-macros -Os ${INCS} ${CPPFLAGS}
LDFLAGS += -g ${LIBS}

# compiler and linker
CC ?= cc

