#Makefile for client for AvpDaemon for Lunix

PROG = AvpDaemonClient
SRCS = AvpDaemonClient.cpp
OBJS = AvpDaemonClient.o
GCPP  = egcs++ #g++
CXXFLAGS = -Wall -Wstrict-prototypes -O2

%.o: %.cpp
	$(GCPP) $(CXXFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

all:    $(PROG)

$(PROG): $(OBJS) 
	$(GCPP) $(CXXFLAGS) $(OBJS) -o $(PROG)
	strip $(PROG)

clean:
	rm -f $(PROG)
	rm -f *.core
	rm -f *.o
