CC = mpic++ -fPIC
CFLAGS	= -g  -Wall -Wsystem-headers -Wno-format-y2k -Wno-sign-compare -Wpointer-arith -Wreturn-type -Wswitch -Wshadow -g -fopenmp -std=c++11 -Wcast-align -Wl,--copy-dt-needed-entries -Wl,--no-as-needed
COMPILE = $(CC) $(CFLAGS) $(INCLUDES)

LIB_H	 = bgpdump/bgpdump_attr.h bgpdump/bgpdump_formats.h bgpdump/bgpdump_lib.h bgpdump/bgpdump_mstream.h 
SYS_LIBS= -lbz2 -lz -lemon -lboost_serialization -lboost_mpi

all: main.o 

main.o: main.cpp
	$(COMPILE) -o main.out main.cpp bgpdump/libbgpdump.a $(LIB_H) $(SYS_LIBS)

test_graph.o: test_graph.cpp
	$(COMPILE)  -o test_graph.out test_graph.cpp bgpdump/libbgpdump.a $(LIB_H) $(SYS_LIBS)


clean:
	 rm all
