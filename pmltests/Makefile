srcdir = .
top_srcdir = ..
subdir = pmltests

include $(top_srcdir)/CONFIGVARS
include $(top_srcdir)/RULES

CFLAGS +=	-I../pmlsrc
LIBS =		../pmlsrc/libpml.a

all :		d2d.tos dd2d.tos c2d.tos c2c.tos cc2c.tos

d2d.tos :	d2d.o
		$(CC) -o d2d.tos d2d.o $(LIBS)

dd2d.tos :	dd2d.o
		$(CC) -o dd2d.tos dd2d.o $(LIBS)
		
c2d.tos :	c2d.o
		$(CC) -o c2d.tos c2d.o $(LIBS)

c2c.tos :	c2c.o
		$(CC) -o c2c.tos c2c.o $(LIBS)
		
cc2c.tos :	cc2c.o
		$(CC) -o cc2c.tos cc2c.o $(LIBS)
		

#
#	Clean up the directory.
#

clean::
	rm -f c2c.tos c2d.tos cc2c.tos d2d.tos dd2d.tos *.BAK *.bak *.tmp nohup.out *.o
