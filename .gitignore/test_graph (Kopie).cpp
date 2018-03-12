#include "main_test.hpp"

using namespace std;
using namespace lemon;

struct BGPData data;
std::list<BGPData> bgplist;
std::list<BGPData>::iterator it;
std::map<int,int> as_ID;
std::map<int,int>::iterator it_as_ID;
std::map<int,int>::iterator it_as_ID_while;
int counter=4;
int hops_length=0;
int hops_maxlength=10;
ListDigraph g;
ListDigraph::NodeMap<int> map_node(g);
int graph_build(list<BGPData>list_all, int zaehler, int dumplength);

int main(int argc, char **argv)
{
	it=bgplist.begin();
		data.hops="1 2 1";
		data.source_AS=1;
		data.dest_AS=2;
		bgplist.push_front(data);
		it = bgplist.begin();
  		data.hops="2 1";
		data.source_AS=2;
		data.dest_AS=1;
		bgplist.push_front(data);
		it = bgplist.begin();
		data.hops="3 2 1";
		data.source_AS=3;
		data.dest_AS=1;
		bgplist.push_front(data);
		it = bgplist.begin();
		data.hops="4 3 4 3 2";
		data.source_AS=4;
		data.dest_AS=2;
		bgplist.push_front(data);
		it = bgplist.begin();
		graph_build(bgplist,counter,hops_maxlength);
		for (ListDigraph::NodeIt n(g); n != INVALID; ++n){
 		 std::cout << g.id(n) << std::endl;
		int count=0;
         	 for (ListDigraph::NodeIt n(g); n!=INVALID; ++n) ++count;
		printf("   Counter       : %i\n",count);	
		}
		int cnt = 0;
		for (ListDigraph::ArcIt a(g); a != INVALID; ++a)
 		 cnt++;
		std::cout << "Number of arcs: " << cnt << std::endl;
	return 0;
}

int graph_build(list<BGPData>list_all, int zaehler, int dumplength)
{
	size_t found=-1;
	size_t amount;
	string part_1;
	int start_as;
	int dest_as;
	int start_id;
	int dest_id;
	int j=0;
	vector<string> tokens;
	vector<string>::iterator it_token;
	string as_hops;
	int node_id;
	char* wert;
	ListDigraph::Node node;
	list<BGPData> graph_list=list_all;
	list<BGPData>::iterator it_graph; 
	//printf("   Counter       : %i\n",counter);
	//printf("   dumplength       : %i\n",hops_maxlength);
	int as=0;
	char* hops_array[counter][dumplength];
	it_graph = graph_list.begin();
	it_as_ID=as_ID.begin();	
	printf("   LISTE_Hops       : %s\n",it_graph->hops);
	for(int i=0;false==graph_list.empty(); i++){ // loop over the whole list
		it_as_ID_while=as_ID.begin();
		as=it_graph->source_AS;	
		printf("   AS       : %i\n",as);		
		hops_array[i][0]=it_graph->hops;
		printf("   Array      : %s\n",hops_array[i][0]);
		graph_list.pop_front();
		while ((it_as_ID_while != as_ID.end()) && (it_as_ID_while->second != as))
		++it_as_ID_while;
		if (it_as_ID_while == as_ID.end()) // test if AS is already in the graph
		{
			node = g.addNode(); //not in the graph
			as_ID.insert(std::pair<int,int>(g.id(node),as));
			++it_as_ID;		
			map_node[node] = as;
			std::cout << it_as_ID->first << " => " << it_as_ID->second << '\n';		
			it_graph = graph_list.begin();
		}
		else
		{
			hops_array[i][0]=it_graph->hops; //in the graph
		}
	}
	for(int i=0;i<counter; i++) //create the arcs
	{
		as_hops=hops_array[i][0];
		found =-1; //refresh while parameter
		cout << as_hops <<'\n';
		while(found!=0) //ends when no space is found 
		{
			amount =count(as_hops.begin(), as_hops.end(), ' ');
			found=as_hops.find(' ');//search first space
			part_1=as_hops.substr(0,found); //split the hop-string in two strings
			as_hops=as_hops.substr(found+1,as_hops.length());
			//cout << "AS_Hops" << as_hops<<'\n';
			start_as=atoi(part_1.c_str()); // save the number of the AS
			while(found!=0)
			{
				if (amount==1||amount ==0)
				{
					it_as_ID_while=as_ID.begin();
					//cout << "AS_Hops" << as_hops<<'\n';
					//cout << "amount" << amount<<'\n';
					dest_as=atoi(as_hops.c_str()); // take the last AS
					while ((it_as_ID_while != as_ID.end()) && (it_as_ID_while->second != start_as)) //search for Node ID
						++it_as_ID_while;
					if (it_as_ID_while == as_ID.end()) // if the node is not in the list
					{
						cout << "Fehler!"<<'\n';				
					}
					else //if the node is in the list
					{
						start_id=it_as_ID_while ->first;
					}
					it_as_ID_while=as_ID.begin();
					while ((it_as_ID_while != as_ID.end()) && (it_as_ID_while->second != start_as)) //search for Node ID
						++it_as_ID_while;
					if (it_as_ID_while == as_ID.end()) // if the node is not in the list
					{
						cout << "Fehler!"<<'\n';				
					}
					else //if the node is in the list
					{
						dest_id=it_as_ID_while ->first;
					}
					g.addArc(g.nodeFromId(start_id), g.nodeFromId(dest_id));
					found=0;
				}
				else
				{
					amount =count(as_hops.begin(), as_hops.end(), ' ');
					it_as_ID_while=as_ID.begin();
					//cout << "amount" << amount<<'\n';
					//cout << "AS_Hops" << as_hops<<'\n';
					found=as_hops.find(' ');//search the next space
					part_1=as_hops.substr(0,found); //split the next AS
					dest_as=atoi(part_1.c_str());	
					as_hops=as_hops.substr(found+1,as_hops.length());
					while ((it_as_ID_while != as_ID.end()) && (it_as_ID_while->second != start_as)) //search for Node ID
						++it_as_ID_while;
					if (it_as_ID_while == as_ID.end()) // if the node is not in the list
					{
						cout << "Fehler!"<<'\n';				
					}
					else //if the node is in the list
					{
						start_id=it_as_ID_while ->first;
					}
					it_as_ID_while=as_ID.begin();
					while ((it_as_ID_while != as_ID.end()) && (it_as_ID_while->second != start_as)) //search for Node ID
						++it_as_ID_while;
					if (it_as_ID_while == as_ID.end()) // if the node is not in the list
					{
						cout << "Fehler!"<<'\n';				
					}
					else //if the node is in the list
					{
						dest_id=it_as_ID_while ->first;
					}
					g.addArc(g.nodeFromId(start_id), g.nodeFromId(dest_id));
					start_as=dest_as;	
				}
			}
		}
	}
return 0;
}
