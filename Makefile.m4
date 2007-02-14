PREFIX=/usr/local

CC=gcc

CFLAGS=-Wall -g -O

CSRC = misc.c reclinker.c pathparse.c myarray.c get_line_from_file.c
INCL = config.h reclinker.h 

COBJS = ${CSRC:.c=.o}

ifelse(OS, Linux,
ifdef USE_BB_LIBPWDGRP
LIBPWDGRP=	libpwdgrp.a
endif
)

reclinker: ${COBJS} 
	$(CC) $(CFLAGS) $(WFLAGS) -o $@ ${COBJS} ${LIBPWDGRP}

${COBJS}: ${INCL}

install: 
	ln -sf reclinker recdeleter
	ln -sf reclinker reclinktester
	cp -a reclinker recdeleter reclinktester $(PREFIX)/bin

clean:
	rm -f *.o reclinker recdeleter reclinktester
