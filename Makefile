PREFIX=/usr/local

CC=gcc

#WFLAGS=-Wall


reclinker: aux.o reclinker.o pathparse.o myarray.o get_line_from_file.o
	$(CC) $(CFLAGS) $(WFLAGS) -o reclinker reclinker.o aux.o pathparse.o myarray.o get_line_from_file.o

aux.o: 
	$(CC) $(CFLAGS) $(WFLAGS) -c -o aux.o aux.c

reclinker.o: 
	$(CC) $(CFLAGS) $(WFLAGS) -c -o reclinker.o reclinker.c

pathparse.o: 
	$(CC) $(CFLAGS) $(WFLAGS) -c -o pathparse.o pathparse.c

myarray.o: 
	$(CC) $(CFLAGS) $(WFLAGS) -c -o myarray.o myarray.c

get_line_from_file.o: 
	$(CC) $(CFLAGS) $(WFLAGS) -c -o get_line_from_file.o get_line_from_file.c

install: 
	ln -sf reclinker recdeleter
	ln -sf reclinker reclinktester
	cp -a reclinker recdeleter reclinktester $(PREFIX)/bin
clean:
	rm -f *.o reclinker recdeleter reclinktester
