SHELL = /bin/sh

prefix = ..
exec_prefix = $(prefix)
includedir = $(exec_prefix)/include
bindir = $(exec_prefix)/bin
srcdir = $(exec_prefix)/src
objdir = $(exec_prefix)/obj

macportslibdir = /opt/local/lib
macportsincludedir = /opt/local/include/graphviz
gvlibdir = /usr/local/lib/ 
gvincludedir = /usr/local/include/graphviz

# Flags, includes and libraries
CC = gcc
CFLAGS = -O2 \
	 -Wall

INCLUDES = -I$(includedir) -I$(macportsincludedir) -I$(gvincludedir)
LFLAGS =  -L/System/Library/Frameworks/OpenGL.framework/Libraries -L$(macportslibdir) -L$(gvlibdir)
LIBS = -lGL -lGLU -framework GLUT -framework OpenGL -lm -lgvc -lgraph -lcdt -lpathplan

QWVIZOBJS = $(addprefix $(objdir)/,qwViz.o \
	bitmap.o \
	pauls.o \
	menus.o \
	misc.o \
	qw_readfiles.o \
	qw_graphlayout.o \
	qw_malloc.o \
	qw_compute.o \
	qw_render.o \
	qw_writefiles.o)

QWVIZ = $(bindir)/qwViz

all: $(QWVIZ)

# Create the executable qwViz.
$(QWVIZ): $(QWVIZOBJS)
	$(CC) $(INCLUDES) $(CFLAGS) -o $(QWVIZ) $(QWVIZOBJS) $(LFLAGS) $(LIBS)

$(objdir)/%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(objdir)/bitmap.o: $(includedir)/bitmap.h 
$(objdir)/pauls.o: $(includedir)/bitmap.h $(includedir)/pauls.h $(objdir)/bitmap.o
$(objdir)/menus.o: $(includedir)/qwViz.h 
$(objdir)/misc.o: $(includedir)/qwViz.h $(includedir)/interfacestring.h $(includedir)/optionstring.h
$(objdir)/qw_readfiles.o: $(includedir)/qwViz.h $(objdir)/qw_malloc.o
$(objdir)/qw_graphlayout.o: $(includedir)/qwViz.h $(objdir)/qw_malloc.o
$(objdir)/qw_compute.o: $(includedir)/qwViz.h $(objdir)/qw_malloc.o $(objdir)/qw_readfiles.o
$(objdir)/qw_render.o: $(includedir)/qwViz.h $(objdir)/pauls.o $(objdir)/qw_malloc.o
$(objdir)/qw_writefiles.o: $(includedir)/qwViz.h
$(objdir)/qwViz.o: $(includedir)/qwViz.h $(objdir)/pauls.o $(objdir)/qw_malloc.o \
	$(objdir)/qw_render.o $(objdir)/qw_writefiles.o $(objdir)/qw_readfiles.o $(objdir)/qw_graphlayout.o \
	$(objdir)/qw_readfiles.o $(objdir)/misc.o $(objdir)/menus.o 
.PHONY: clean uninstall
clean:
	rm -vf $(objdir)/*.o $(srcdir)/*~ $(includedir)/*~
uninstall:
	rm -vf $(objdir)/*.o $(srcdir)/*~ $(includedir)/*~ $(QWVIZ)