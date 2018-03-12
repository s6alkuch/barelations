#include "main_test.hpp"

using namespace std;
using namespace lemon;

struct BGPData data;
list<BGPData> bgplist;
list<BGPData>::iterator it;
string message_type;
vector<string>hops_vector;
vector<string>::iterator it_hops_vector;
vector<vector<string>> all_vectors;
map<int,int> as_ID;
map<string,int> arc_ID;
map<string,int>::iterator it_arc_ID;
map<string,int>::iterator it_arc_ID2;
map<int,int>::iterator it_as_ID;
map<int,int>::iterator it_as_ID_while;
map<int,int>::iterator it_as_ID_as1;
map<int,int>::iterator it_as_ID_as2;
ListDigraph g;
ListDigraph::NodeMap<int> map_node(g); //change to map
map<int,int> map_degree;
map<int,int> map_c2p;
map<int,int>::iterator it_map_c2p;
map<int,int> map_p2c;
map<int,int>::iterator it_map_p2c;
map<int,int> map_p2p;
map<int,int>::iterator it_map_p2p;
//ListDigraph::ArcMap<int> map_c2p(g);
//ListDigraph::ArcMap<int> map_p2c(g);
//ListDigraph::ArcMap<int> map_p2p(g);
ListDigraph::Node node;
ListDigraph::Node node_cal;
ListDigraph::Arc arc;
ListDigraph::Arc arc2;
int small_counter;
int node_id;
int r_border=1; //as set in the paper 
int counter=0;
int source_as;
int dest_as;
void graph_build();
void calculate_data();

int main(int argc, char **argv)
{
	it=bgplist.begin();
		data.hops="1 4 6";
		data.source_AS=1;
		data.dest_AS=6;
		bgplist.push_front(data);
		hops_vector.push_back(data.hops);
		it = bgplist.begin();
  		data.hops="2 4 6";
		data.source_AS=2;
		data.dest_AS=6;
		bgplist.push_front(data);
		hops_vector.push_back(data.hops);
		it = bgplist.begin();
		data.hops="3 5 4 6";
		data.source_AS=3;
		data.dest_AS=6;
		bgplist.push_front(data);
		hops_vector.push_back(data.hops);
		it = bgplist.begin();
		data.hops="3 5 6";
		data.source_AS=3;
		data.dest_AS=6;
		bgplist.push_front(data);
		hops_vector.push_back(data.hops);
		it = bgplist.begin();
		data.hops="2 4 5 6";
		data.source_AS=2;
		data.dest_AS=6;
		bgplist.push_front(data);
		hops_vector.push_back(data.hops);
		it = bgplist.begin();
		data.hops="3 5 4";
		data.source_AS=3;
		data.dest_AS=4;
		bgplist.push_front(data);
		hops_vector.push_back(data.hops);
		it = bgplist.begin();
		graph_build();
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
		calculate_data();
	return 0;
}

void graph_build()
{
	size_t found=-1;
	size_t amount;
	string part_1;
	string whole;
	int start_as;
	int start_id;
	int dest_id;
	//int j=0;
	vector<string> tokens;
	vector<string>::iterator it_token;
	string as_hops;
	small_counter=counter/1000;
	//char* liste_hops; 

	//char* wert;
	
	//list<BGPData> graph_list =complete_list;
	//cout << "Liste kopiert";
	//list<BGPData>::iterator it_graph; 
	//printf("   Counter       : %i\n",counter);
	//int as=0;
	//it_graph = graph_list.begin();
	//it =bgplist.begin();
	it_as_ID=as_ID.begin();	
	it_as_ID_while=as_ID.begin();
	//for(int i=0;false==graph_list.empty(); ++i){ // loop over the whole list
		//as=it_graph->source_AS;		
		//printf("   AS       : %i\n",as);
		//printf("   i       : %i\n",graph_list.size());		
		//graph_list.pop_front();
		//it_graph = graph_list.begin();	
		//it_as_ID_while=as_ID.find(as); //search the AS in the map
		//if (it_as_ID_while == as_ID.end()) // test if AS is already in the graph
		//{
			//node = g.addNode(); //not in the graph
			//node_id=g.id(node);
			//cout<<"Node ID"<< node_id<< '\n';
			//as_ID[as]=node_id;
			//it_as_ID=as_ID.find(as);		
			//map_node[node] = as;
			//cout << it_as_ID->first << " => " << it_as_ID->second << '\n';
			//it_as_ID_while=as_ID.begin();	
		//}
		//else
		//{
			//it_as_ID_while=as_ID.begin();
			//cout<<"Schon als Knoten"<<'\n';
		//}
	//}
	//int all_nodes=countNodes(g);
	//cout<< "Nodes" << all_nodes;
	for(int i=0;i<small_counter; ++i) //create the arcs
	{
		as_hops=hops_vector.at(i);
		found =-1; //refresh while parameter
		//cout << as_hops <<'\n';
		//cout << i <<'\n';
		while(found!=0) //ends when no space is found 
		{			
			found=as_hops.find(' ');//search first space
			part_1=as_hops.substr(0,found); //split the hop-string in two strings
			as_hops=as_hops.substr(found+1,as_hops.length()); //set as_hops to the rest of the string	
			start_as=atoi(part_1.c_str()); // save the number of the AS
			amount=count(as_hops.begin(), as_hops.end(), ' '); //count the spaces int the string
			//cout << "Amount_First" << amount<<'\n';
			while(found!=0)
			{
				if (amount ==0)
				{
					it_as_ID_while=as_ID.begin(); //set back while parameter
					//cout << "AS_Hops" << as_hops<<'\n';
					//cout << "amount ende" << amount<<'\n';
					dest_as=atoi(as_hops.c_str()); // take the last AS
					it_as_ID_while=as_ID.find(start_as); //search the AS in the map
					if (it_as_ID_while == as_ID.end()) 
					{
						node = g.addNode(); //not in the graph
						node_id=g.id(node);
						//cout<<"Node ID"<< node_id<< '\n';
						as_ID[start_as]=node_id;
						it_as_ID=as_ID.find(start_as);		
						//map_node[node] = start_as;
						//cout << it_as_ID->first << " => " << it_as_ID->second << '\n';
						start_id=node_id;
						//it_as_ID_while=as_ID.begin();					
					}
					else 
					{
						start_id=it_as_ID_while->second; //if the node is in the list
					}
					it_as_ID_while=as_ID.find(dest_as); //search the AS in the map
					if (it_as_ID_while == as_ID.end())
					{
						node = g.addNode(); //not in the list
						node_id=g.id(node);
						//cout<<"Node ID"<< node_id<< '\n';
						as_ID[dest_as]=node_id;
						it_as_ID=as_ID.find(dest_as);		
						//map_node[node] = dest_as;
						//cout << it_as_ID->first << " => " << it_as_ID->second << '\n';
						dest_id=node_id;
						//it_as_ID_while=as_ID.begin();					
					}
					else 
					{
						dest_id=it_as_ID_while->second; //if the node is in the list
					}
					if(it_arc_ID==arc_ID.end())
					{
						arc=g.addArc(g.nodeFromId(start_id), g.nodeFromId(dest_id));
						whole=to_string(start_as)+" "+to_string(dest_as);
						cout << whole;
						arc_ID[whole]=g.id(arc);
						it_arc_ID=arc_ID.find(whole);
						//cout << " => " << it_arc_ID->second << '\n';
						found=0;
					}
					else
					{
						found=0;
					}
				}
				else
				{
					amount =count(as_hops.begin(), as_hops.end(), ' ');//count the space in the hops
					//cout << "amount-runde" << amount<<'\n';
					//cout << "start" << start_as<<'\n';
					found=as_hops.find(' ');//search the next space
					part_1=as_hops.substr(0,found); //split the next AS
					dest_as=atoi(part_1.c_str());	//take the next AS
					//cout << "dest" << dest_as<<'\n';
					as_hops=as_hops.substr(found+1,as_hops.length());
					it_as_ID_while=as_ID.find(start_as); //search the AS in the map
					if (it_as_ID_while == as_ID.end()) // if the node is not in the list
					{
						node = g.addNode(); //not in the graph
						node_id=g.id(node);
						//cout<<"Node ID"<< node_id<< '\n';
						as_ID[start_as]=node_id;
						it_as_ID=as_ID.find(start_as);		
						//map_node[node] = start_as;
						//cout << it_as_ID->first << " => " << it_as_ID->second << '\n';
						start_id=node_id;
						//it_as_ID_while=as_ID.begin();					
					}
					else //if the node is in the list
					{
						start_id=it_as_ID_while ->second;
						//it_as_ID_while=as_ID.begin();
					}
					it_as_ID_while=as_ID.find(dest_as); //search the AS in the map
					if (it_as_ID_while == as_ID.end()) 
					{
						node = g.addNode(); //not in the list
						node_id=g.id(node);
						//cout<<"Node ID"<< node_id<< '\n';
						as_ID[dest_as]=node_id;
						it_as_ID=as_ID.find(dest_as);		
						//map_node[node] = dest_as;
						//cout << it_as_ID->first << " => " << it_as_ID->second << '\n';
						dest_id=node_id;
						//it_as_ID_while=as_ID.begin();	
					}
					else 
					{
						dest_id=it_as_ID_while ->second; //if the node is in the list
						//it_as_ID_while=as_ID.begin();
					}
					whole=to_string(start_as)+" "+to_string(dest_as);
					cout << whole;
					it_arc_ID=arc_ID.find(whole);	
					if(it_arc_ID==arc_ID.end())
					{
						arc= g.addArc(g.nodeFromId(start_id), g.nodeFromId(dest_id));
						whole=to_string(start_as)+" "+to_string(dest_as);
						cout << whole;
						arc_ID[whole]=g.id(arc);
						it_arc_ID=arc_ID.find(whole);
						//cout <<it_arc_ID->first<< " => " << it_arc_ID->second << '\n';
						start_as=dest_as;
					}
					else
					{
						start_as=dest_as;
					}	
				}
			}
		}
	}
	cout<<countArcs(g);
}
void calculate_data()
{
		
	string s,part_1,node_arc,node_arc_rev;
	istringstream iss(s);	
	vector<string> vstrings;
	int vstrings_size;	
	int j;
	int arc_val1=0;
	int arc_val2=0;
	int check=0;
	size_t found=-1;
	size_t amount=1;
	int stras=0;
	int node_degree_1=0;
	int node_degree_2=0;
	int node_result=0;
	int desas=0;
	int c2p=0,p2c=0;
	int max_as=0;
	int hop_as=0;
	int hop_as2=0;
	int hop_check=0;
	int hop_degree=0;
	int hop_degree_max=0;
	it_as_ID=as_ID.begin();
	int as_size=as_ID.size();
	
	for(j=0;j<as_size;j++)
	{
		//cout << it_as_ID->first << " => " << it_as_ID->second << '\n';
		
		node_cal=g.nodeFromId(it_as_ID->second);
		map_degree[it_as_ID->second]=countOutArcs(g,node_cal)+countInArcs(g,node_cal);//calculate the degree
		//cout<<"Map degree " <<it_as_ID->second << " <= "<<map_degree[node_cal]<<'\n';
		++it_as_ID;
	}
	 #pragma omp parallel for private(hop_degree, max_as, s, amount,part_1, hop_as,hop_as2,it_as_ID, hop_check,it_arc_ID, node_arc, vstrings, found)
	for(int i=0;i<small_counter; ++i)
	{
		s = hops_vector.at(i);// save the hops in a string
		for(;amount>=0;)
		{
			amount=count(s.begin(),s.end(), ' ');
			found=s.find(' ');//search first space
			part_1=s.substr(0,found); //split the hop-string in two strings			
			s=s.substr(found+1,s.length()); //set as_hops to the rest of the string		
			vstrings.push_back(part_1);
			if (amount==0) break;
		}
		hop_degree_max=0;
		max_as=0;
		//cout<<"V-first" <<vstrings[0]<< '\n';
		for(int k=0;k<vstrings.size();++k)
		{
			hop_as=atoi(vstrings[k].c_str()); //take an AS from the path
			//cout<<vstrings[k]<<'\n';
			it_as_ID=as_ID.find(hop_as); //find the AS in the map
			hop_degree=map_degree[it_as_ID->second]; //take the degree from the node map
			//cout<<hop_degree<<'\n';
			if(hop_degree>hop_degree_max)
			{
				hop_degree_max=hop_degree; // save the higher degree
				max_as=hop_as; //save the corresponding AS
			}		
		}

		for(int l=0;l<vstrings.size()-1;++l)
		{
			check=0;
			hop_as=atoi(vstrings.at(l).c_str()); //save the start AS
			hop_as2=atoi(vstrings.at(l+1).c_str()); //save the dest AS
			if(check==0)
			{
				node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
				//cout <<node_arc<<" Tada"<<'\n';
				it_arc_ID=arc_ID.find(node_arc); //search the string in the map
				if(it_arc_ID==arc_ID.end()) break;
				//arc=g.arcFromId(); //get the corresponding arc
				map_c2p[it_arc_ID->second]=1; //set the map c2p of the arc to 1
				if(hop_as2==max_as) check=1; //check if the highest grade is reached
			}
			else	
			{
				node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
				it_arc_ID=arc_ID.find(node_arc); //search the string in the map
				//arc=g.arcFromId(); //get the corresponding arc
				map_p2c[it_arc_ID->second]=-1; //set the map p2c of the arc to -1
			}		
		}		
	}
	#pragma omp for private(hop_degree, max_as, s, amount,part_1, hop_as,hop_as2,it_as_ID, hop_check,it_arc_ID, node_degree_1, it_arc_ID2, node_degree_2, node_arc, found, vstrings, hop_degree_max)
	for(int m=0;m<small_counter;m++)
	{
		hop_degree_max=0;
		max_as=0;
		s = hops_vector.at(m);// save the hops in a string
		for(;amount>=0;)
		{
			amount=count(s.begin(),s.end(), ' ');
			found=s.find(' ');//search first space
			part_1=s.substr(0,found); //split the hop-string in two strings			
			s=s.substr(found+1,s.length()); //set as_hops to the rest of the string		
			vstrings.push_back(part_1);
			if (amount==0) break;
		}
		for(int n=0;n<vstrings.size();++n)
		{
			hop_as=atoi(vstrings.at(n).c_str()); //take the AS
			//cout<<vstrings[n]<<'\n';
			it_as_ID=as_ID.find(hop_as); //search the AS in the map
			hop_degree=map_degree[it_as_ID->second]; //take the degree from the node map
			if(hop_degree>hop_degree_max)
			{
				hop_degree_max=hop_degree; // save the higher degree
				max_as=hop_as; //save the corresponding AS
			}		
		}
		for(int o=0;o<vstrings.size()-1;++o)
		{
			hop_as=atoi(vstrings.at(o).c_str()); //take the start AS
			hop_as2=atoi(vstrings.at(o+1).c_str()); //take the dest AS
			hop_check=atoi(vstrings.at(o+2).c_str()); //check for the AS(highest degree)
			if(check>=0)
			{
				node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
				it_arc_ID=arc_ID.find(node_arc); //find the arc in the map
				//arc=g.arcFromId(it_arc_ID->second); //get the corresponding arc
				map_p2p[it_arc_ID->second]=1; //set the map p2p of the arc to 1 
				if(hop_check==max_as) check=-1;
			}
			else	
			{
				it_as_ID=as_ID.find(hop_as);//find the node (the one before the highest degree)
				//node = g.nodeFromId(it_as_ID->second); //get the corresponding node
				node_degree_1=map_degree[it_as_ID->second];//get the corresponding degree
				node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
				it_arc_ID=arc_ID.find(node_arc); //search the string in the map
				//arc=g.arcFromId(it_arc_ID->second); //get the corresponding arc
				++o; //increment o
				hop_as=atoi(vstrings.at(o).c_str()); //take the start AS
				hop_as2=atoi(vstrings.at(o+1).c_str()); //take the dest AS
				node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
				it_arc_ID2=arc_ID.find(node_arc); //search the string in the map
				//arc2=g.arcFromId(it_arc_ID->second); //get the second arc
				it_as_ID=as_ID.find(hop_as2); //find the node after the highest degree
				//node=g.nodeFromId(it_as_ID->second);//get the corresponding node
				node_degree_2=map_degree[it_as_ID->second]; //get the corresponding degree
				if(map_p2c[it_arc_ID->second]==-1&&map_c2p[it_arc_ID->second]==1&&map_p2c[it_arc_ID2->second]==-1&&map_c2p[it_arc_ID2->second]==1)
				{
					if(node_degree_1>node_degree_2)//if the degree of the node before the Top Provider is higher
					{				
						map_p2p[it_arc_ID->second]=1;//set the arc to the Top Provider equals 1
						++o; //increment o to ignore the next node
					}
					else
					{
						map_p2p[it_arc_ID2->second]=1;//set the arc from the Top Provider equals 1
						++o;//increment o to ignore the next node
					}
				}
			}		
		}			
	}
	#pragma omp for private(s, amount,part_1, hop_as,hop_as2,it_as_ID, hop_check,it_arc_ID, node_degree_1, node_degree_2, node_arc, found, vstrings, hop_degree_max, arc_val1, arc_val2)
	for(int p=0;p<small_counter;p++)
	{
		s = hops_vector.at(p);// save the hops in a string
		for(;amount>=0;)
		{
			amount=count(s.begin(),s.end(), ' ');
			found=s.find(' ');//search first space
			part_1=s.substr(0,found); //split the hop-string in two strings			
			s=s.substr(found+1,s.length()); //set as_hops to the rest of the string		
			vstrings.push_back(part_1);
			if (amount==0) break;
		}
		vstrings_size=vstrings.size(); //get the size of the vector
		for(int q=0;q<vstrings_size;q++)
		{
			hop_as=atoi(vstrings.at(q).c_str()); //take the start AS
			hop_as2=atoi(vstrings.at(q+1).c_str()); //take the dest AS
			node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
			it_arc_ID=arc_ID.find(node_arc); //search the string in the map
			//arc=g.arcFromId(it_arc_ID->second); //get the corresponding arc
			arc_val1=map_p2p[it_arc_ID->second]; //get the value of the arc
			if(arc_val1!=1)
			{
				node_arc_rev=to_string(hop_as2)+" "+to_string(hop_as); //save the reverse arc
				it_arc_ID2=arc_ID.find(node_arc_rev); //search the string in the map
				//arc2=g.arcFromId(it_arc_ID2->second); //get the corresponding arc
				arc_val2=map_p2p[it_arc_ID2->second]; //get the value of the reverse arc
				it_as_ID=as_ID.find(hop_as); //search the AS in the map
				//node=g.nodeFromId(it_as_ID->second); //get the corresponding node
				node_degree_1=map_degree[it_as_ID->second]; //get the degree of the node
				it_as_ID=as_ID.find(hop_as2); //search the AS in the map
				//node=g.nodeFromId(it_as_ID->second);  //get the corresponding node
				node_degree_2=map_degree[it_as_ID->second];  //get the degree of the node
				node_result=node_degree_1/node_degree_2;
				if(arc_val2!=1 && node_result<r_border && node_result>1/r_border)
				{
					map_c2p[it_arc_ID->second]=2; //save the result in the map
					map_p2c[it_arc_ID->second]=2; //save the result in the map
				}
			}
		}						
	}


	ofstream fileout; //set up a file to save the result
	fileout.open("Relation.txt");
	for(;it_arc_ID!=arc_ID.end();it_arc_ID++)
	{
		s=it_arc_ID->first; 
		found=s.find(' ');
		stras=atoi(s.substr(0,found).c_str());
		desas=atoi(s.substr(found+1,s.length()).c_str());
		//arc=g.arcFromId(it_arc_ID->second);
		c2p=map_c2p[it_arc_ID->second];
		p2c=map_p2c[it_arc_ID->second];
		check=0;
		
		if(c2p==2&&c2p==2) //if it is peer to peer
		{
			fileout << stras<< '\t'<<desas<< '\t' <<"p2p"<<'\n';
		}
		if(c2p==1&&p2c==-1) //if it is sibling to sibling
		{
			fileout << stras<< '\t'<<desas<< '\t' <<"s2s"<<'\n';
			check=1;
		}
		if(c2p==1&&check!=1) //if it is customer to provider
		{
			check=1;
			fileout << stras<< '\t'<<desas<< '\t' <<"c2p"<<'\n';
		}
		if(p2c==-1&&check!=1) //if it is provider to customer
		{
			fileout << stras<< '\t'<<desas<< '\t' <<"p2c"<<'\n';
		}
		else // if it is not clasificable
		{
			fileout << stras<< '\t'<<desas<< '\t' <<"Undefined"<<'\n';
		}	
	}
	fileout.close();	
}	

