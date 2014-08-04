# $Id: Makefile,v 1.18 2002/03/02 21:17:51 sverrehu Exp $
PROG		= snake4
DIST		= $(PROG)
VERMAJ		= 1
VERMIN		= 0
VERPAT		= 12
VERSION		= $(VERMAJ).$(VERMIN).$(VERPAT)
COMPILED_DATE	= `date '+%Y-%m-%d %H:%M:%S'`
COMPILED_BY	= `whoami`

###########################################################################

# Where are shhmsg, shhopt, Xpm and X11?

# GNU/Linux at home
INCDIR		= -I/local/include -I/usr/X11R6/include -I/local/include/X11
LIBDIR		= -L/local/lib -L/usr/X11R6/lib -L/local/lib/X11
EXTRA_LIBS	= -lXmu -lICE -lSM -lXext

# Irix at Ifi
ifeq ($(HOSTTYPE),sgi)
INCDIR		= -I/usr/include/X11 -I$$HOME/include
LIBDIR		= -L/usr/lib/X11 -L$$HOME/lib/$$HOSTTYPE
endif

# Solaris at Ifi
ifeq ($(HOSTTYPE),sol)
INCDIR		= -I/usr/openwin/include -I$$HOME/include
LIBDIR		= -L/usr/openwin/lib -L$$HOME/lib/$$HOSTTYPE
EXTRA_LD_OPT	= -R$(LD_RUN_PATH):/usr/openwin/lib
EXTRA_LIBS	= -lXmu
endif

# SunOS at Ifi
ifeq ($(HOSTTYPE),sun4)
INCDIR		= -I/local/X11R5/include -I$$HOME/include
LIBDIR		= -L/local/X11R5/lib -L$$HOME/lib/$$HOSTTYPE
EXTRA_LIBS	= -liberty -lXext -lXmu
endif

###########################################################################

# Where do you want to install the program and the highscore file?
INSTLIBDIR	= /var/local/lib/games
INSTBINDIR	= /usr/local/games
INSTMANDIR	= /usr/local/man/man6
#INSTLIBDIR	= /hom/sverrehu/lib
#INSTBINDIR	= /hom/sverrehu/bin/$$HOSTTYPE
#INSTMANDIR	= /hom/sverrehu/man/man6

# Game user. Program runs suid, so this must not be root.
OWNER		= games
GROUP		= games
#OWNER		= sverrehu
#GROUP		= sverrehu

SCOREBASEFILE	= $(PROG).score
SCOREFILE	= $(INSTLIBDIR)/$(SCOREBASEFILE)
DEFINES		= -DVERSION=\"$(VERSION)\" \
		  "-DCOMPILED_DATE=\"$(COMPILED_DATE)\"" \
		  "-DCOMPILED_BY=\"$(COMPILED_BY)\"" \
		  -DSCOREFILE=\"$(SCOREFILE)\" \
		  -DXK_MISCELLANY -DXK_LATIN1 

###########################################################################

CC		= gcc

OPTIM		= -s -O2
CCOPT		= -Wall $(OPTIM) $(INCDIR) $(DEFINES) $(CFLAGS)
LDOPT		= -s $(LIBDIR) $(LDFLAGS) $(EXTRA_LD_OPT)

LIBS		= -lshhopt -lshhmsg -lXaw -lXt -lXpm -lX11 $(EXTRA_LIBS)

OBJS		= board.o fruit.o game.o gameobject.o headbanger.o \
		  mushroom.o score.o scull.o slimpill.o snake.o \
		  snake4.o suid.o win.o

###########################################################################

all: $(PROG)

$(PROG): $(OBJS)

.o: $(OBJS)
	$(CC) $(CCOPT) -o $@ $(OBJS) $(LDOPT) $(LIBS)

.c.o:
	$(CC) -o $@ -c $(CCOPT) $<

clean:
	rm -f *.o core depend *~

install: $(PROG)
	install -d -m 755 $(INSTBINDIR) $(INSTLIBDIR) $(INSTMANDIR)
	install -s -m 4755 -o $(OWNER) -g $(GROUP) $(PROG) $(INSTBINDIR)
	ln -sf $(PROG) $(INSTBINDIR)/snake4scores
	install -m 644 $(DIST).6 $(INSTMANDIR)
	if test ! -f $(SCOREFILE); then \
	  install -m 644 -o $(OWNER) -g $(GROUP) \
	          $(SCOREBASEFILE) $(INSTLIBDIR); fi

depend dep:
	$(CC) $(INCDIR) -MM *.c >depend

###########################################################################

# To let the author make a distribution. The rest of the Makefile
# should be used by the author only.
LSMFILE		= $(DIST)-$(VERSION).lsm
DISTDIR		= $(DIST)-$(VERSION)
DISTFILE	= $(DIST)-$(VERSION).tar.gz
DISTFILES	= README INSTALL CREDITS TODO $(LSMFILE) $(DIST).6 \
		  $(DIST).lsm.in ChangeLog Makefile $(SCOREBASEFILE) \
		  $(SCOREBASEFILE).ifi \
		  board.c fruit.c game.c gameobject.c headbanger.c \
		  mushroom.c score.c scull.c slimpill.c snake.c \
		  snake4.c suid.c win.c \
		  board.h fruit.h game.h gameobject.h headbanger.h \
		  mushroom.h score.h scull.h slimpill.h snake.h \
		  suid.h win.h \
		  pixmaps
BINDISTDIR	= $(DIST)-$(VERSION)-bin
BINDISTFILE	= $(DIST)-$(VERSION)-bin.tar.gz
BINDISTFILES	= README.static $(PROG).static $(PROG).6 \
		  Makefile.static $(SCOREBASEFILE)

# Linux executable with my libraries and Xpm statically linked.
static $(PROG).static: $(OBJS)
	$(CC) $(CCOPT) -o $(PROG).static $(OBJS) $(LDOPT) \
	/local/lib/libshhopt.a /local/lib/libshhmsg.a \
	/local/lib/X11/libXpm.a \
	-lXaw -lXt -lX11 $(EXTRA_LIBS)

$(LSMFILE): $(DIST).lsm.in
	VER=$(VERSION); \
	DATE=`date "+%d%b%y"|tr '[a-z]' '[A-Z]'`; \
	sed -e "s/VER/$$VER/g;s/DATE/$$DATE/g" $(DIST).lsm.in > $(LSMFILE)

$(SCOREBASEFILE): $(SCOREBASEFILE).ifi
	echo "1" > $(SCOREBASEFILE)
	grep "^sverrehu," $(SCOREBASEFILE).ifi >> $(SCOREBASEFILE)

chmod:
	chmod -R a+rX *

veryclean: clean
	rm -f $(PROG) $(PROG).static $(DISTFILE) $(BINDISTFILE) $(LSMFILE)

dist: $(LSMFILE) $(SCOREBASEFILE) chmod
	mkdir $(DISTDIR)
	chmod a+rx $(DISTDIR)
	for q in $(DISTFILES); do \
	  if test -r $$q; then \
	    ln -s ../$$q $(DISTDIR); \
	  else echo "warning: no file $$q"; fi; \
	  done
	tar -cvhzf $(DISTFILE) --exclude \*/CVS $(DISTDIR)
	chmod a+r $(DISTFILE)
	rm -rf $(DISTDIR)

bindist: $(PROG).static $(SCOREBASEFILE) chmod
	mkdir $(BINDISTDIR)
	chmod a+rx $(BINDISTDIR)
	for q in $(BINDISTFILES); do \
	  if test -r $$q; then \
	    ln -s ../$$q $(BINDISTDIR)/`basename $$q .static`; \
	  else echo "warning: no file $$q"; fi; \
	  done
	tar -cvhzf $(BINDISTFILE) --exclude \*/CVS $(BINDISTDIR)
	chmod a+r $(BINDISTFILE)
	rm -rf $(BINDISTDIR)

ifeq (depend,$(wildcard depend))
include depend
endif
