#include <fstream>
#include <time.h>
#include <list>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <algorithm> 
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <lemon/list_graph.h>
#include <map>
#include <boost/mpi.hpp>
#include "boost_1_66_0/boost/serialization/serialization.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <lemon/concepts/digraph.h>
#include <mpi.h>
struct BGPData{
		int source_AS;
		int dest_AS;
		char* hops;
};
