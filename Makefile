PREFIX=/usr/local

CC=gcc

#WFLAGS=-Wall

CSRC = aux.c reclinker.c pathparse.c myarray.c get_line_from_file.c

COBJS = ${CSRC:.c=.o}

reclinker: ${COBJS} 
	$(CC) $(CFLAGS) $(WFLAGS) -o $@ ${COBJS}

pwdgrp:
	$(MAKE) -C libpwdgrp

glibc-static: ${COBJS} pwdgrp
	$(CC) $(CFLAGS) $(WFLAGS)  -o reclinker ${COBJS} libpwdgrp/libpwdgrp.a -static

install: 
	ln -sf reclinker recdeleter
	ln -sf reclinker reclinktester
	cp -a reclinker recdeleter reclinktester $(PREFIX)/bin

clean:
	rm -f *.o reclinker recdeleter reclinktester
	$(MAKE) clean -C libpwdgrp
