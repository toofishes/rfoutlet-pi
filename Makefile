CPPFLAGS=-D_FORTIFY_SOURCE=2
CFLAGS=-O2 -pipe -fstack-protector --param=ssp-buffer-size=4 -Wall
LDFLAGS=-Wl,-O1,--sort-common,--as-needed,-z,relro

all: rfoutlet sniff

rfoutlet: rfoutlet.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi

sniff: sniff.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi

clean:
	$(RM) *.o rfoutlet sniff
