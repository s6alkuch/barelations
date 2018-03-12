#include "main.hpp"
using namespace std;
using namespace lemon;
extern "C" {
#include "bgpdump/bgpdump_lib.h"
}
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
//ListDigraph::NodeMap<int> map_node(g); //change to map
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
int node_id;
int r_border=60; //as set in the paper 
int counter=0;
int source_as;
int dest_as;
//int hops_length=0;
//int hops_maxlength=0;
    void process(BGPDUMP_ENTRY *entry);
    void show_attr(attributes_t *attr);
    void show_prefixes(int count,struct prefix *prefix);
    void graph_build(list<BGPData> bgplist);
#ifdef BGPDUMP_HAVE_IPV6
    void show_v6_prefixes(int count, struct prefix *prefix);
    void calculate_data();
   
#endif

int main(int argc, char **argv, int time) {  
    BGPDUMP *my_dump;
    BGPDUMP_ENTRY *my_entry=NULL;
counter=0;
    if(argc>1) {
	my_dump=bgpdump_open_dump(argv[1]);
    } else {
	my_dump=bgpdump_open_dump("dumps/latest-bview.gz");
    }

    if(my_dump==NULL) {
	printf("Error opening dump file ...\n");
	exit(1);
    }

    do {
//fprintf(stdout, "Offset: %d\n", gztell(my_dump->f));
	my_entry=bgpdump_read_next(my_dump);
	if(my_entry!=NULL) {
	    process(my_entry);
	    bgpdump_free_mem(my_entry);
	}
    } while(my_dump->eof==0);

    bgpdump_close_dump(my_dump);
//fprintf(stderr, "%s: OK=%d, BAD=%d (%f%% OK)\n", my_dump->filename, my_dump->parsed_ok, my_dump->parsed - my_dump->parsed_ok, (float) my_dump->parsed_ok / my_dump->parsed * 100);
	graph_build(bgplist);
	calculate_data();
 return 0;
}

char const*bgp_state_name[] = {
    "Unknown",
    "IDLE",
    "CONNECT",
    "ACTIVE",
    "OPEN_SENT",
    "OPEN_CONFIRM",
    "ESTABLISHED",
    NULL
};

char const*bgp_message_types[] = {
    "Unknown",
    "Open",
    "Update/Withdraw",
    "Notification",
    "Keepalive"
};

char const*notify_codes[] = {
    "Unknown",
    "Message Header Error",
    "OPEN Message Error",
    "UPDATE Message Error",
    "Hold Timer Expired",
    "Finite State Machine Error",
    "Cease"
};

char const *notify_subcodes[][12] = {
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    /* Message Header Error */
    {
	"None",
 	"Connection Not Synchronized",
	"Bad Message Length",
	"Bad Message Type",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL 
    },
    /* OPEN Message Error */
    {
	"None",
	"Unsupported Version Number",
	"Bad Peer AS",
	"Bad BGP Identifier",
	"Unsupported Optional Parameter",
	"Authentication Failure",
	"Unacceptable Hold Time",
	NULL, NULL, NULL, NULL, NULL
    },
    /* UPDATE Message Error */
    {
	"None",
	"Malformed Attribute List",
	"Unrecognized Well-known Attribute",
	"Missing Well-known Attribute",
	"Attribute Flags Error",
	"Attribute Length Error",
	"Invalid ORIGIN Attribute",
	"AS Routing Loop",
	"Invalid NEXT_HOP Attribute",
	"Optional Attribute Error",
	"Invalid Network Field",
	"Malformed AS_PATH"
    },
    /* Hold Timer Expired */
    { "None", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    /* Finite State Machine Error */
    { "None", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    /* Cease */
    { "None", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }

};

void process(BGPDUMP_ENTRY *entry) {
    char prefix[BGPDUMP_ADDRSTRLEN], peer_ip[BGPDUMP_ADDRSTRLEN];
    char source_ip[BGPDUMP_ADDRSTRLEN], destination_ip[BGPDUMP_ADDRSTRLEN];
    struct mp_nlri *mp_announce, *mp_withdraw;
    int i, code, subcode;
	BGPDUMP_TABLE_DUMP_V2_PREFIX *e;

if(entry->type == BGPDUMP_TYPE_ZEBRA_BGP && entry->subtype == BGPDUMP_SUBTYPE_ZEBRA_BGP_MESSAGE && entry->body.zebra_message.type == BGP_MSG_KEEPALIVE) return;
if(entry->type == BGPDUMP_TYPE_ZEBRA_BGP && entry->subtype == BGPDUMP_SUBTYPE_ZEBRA_BGP_MESSAGE && entry->body.zebra_message.type == BGP_MSG_OPEN) return;
if(entry->type == BGPDUMP_TYPE_ZEBRA_BGP && entry->subtype == BGPDUMP_SUBTYPE_ZEBRA_BGP_MESSAGE && entry->body.zebra_message.type == BGP_MSG_NOTIFY) return;
if(entry->type == BGPDUMP_TYPE_ZEBRA_BGP && entry->subtype == BGPDUMP_SUBTYPE_ZEBRA_BGP_STATE_CHANGE && entry->length == 8) return;

    //printf("TIME            : %s",asctime(gmtime(&entry->time)));
    //printf("LENGTH          : %u\n", entry->length);
    switch(entry->type) {
	case BGPDUMP_TYPE_MRTD_TABLE_DUMP:
	    if(entry->subtype == AFI_IP) {
		strcpy(prefix, inet_ntoa(entry->body.mrtd_table_dump.prefix.v4_addr));
		strcpy(peer_ip, inet_ntoa(entry->body.mrtd_table_dump.peer_ip.v4_addr));
#ifdef BGPDUMP_HAVE_IPV6
	    } else if(entry->subtype == AFI_IP6) {
		inet_ntop(AF_INET6, &entry->body.mrtd_table_dump.prefix.v6_addr, prefix,
			  sizeof(prefix));
		inet_ntop(AF_INET6, &entry->body.mrtd_table_dump.peer_ip.v6_addr, peer_ip,
			  sizeof(peer_ip));
#endif
	    } else {
		*prefix = '\0';
		*peer_ip = '\0';
	    }
	    // printf("TYPE            : BGP Table Dump Entry\n");
	   // printf("    VIEW        : %d\n",entry->body.mrtd_table_dump.view);
	   // printf("    SEQUENCE    : %d\n",entry->body.mrtd_table_dump.sequence);
	  //  printf("    PREFIX      : %s/%d\n",prefix,entry->body.mrtd_table_dump.mask);
	  //  printf("    STATUS      : %d\n",entry->body.mrtd_table_dump.status);
	  //  printf("    UPTIME      : %s",asctime(gmtime(&entry->body.mrtd_table_dump.uptime)));
	  //  printf("    PEER IP     : %s\n",peer_ip);
	  //  printf("    PEER AS     : %u\n",entry->body.mrtd_table_dump.peer_as);
		data.source_AS= entry->body.mrtd_table_dump.peer_as;
    	show_attr(entry->attr);
	    break;

	case BGPDUMP_TYPE_TABLE_DUMP_V2:

		e = &entry->body.mrtd_table_dump_v2_prefix;

	    if(e->afi == AFI_IP) {
			strcpy(prefix, inet_ntoa(e->prefix.v4_addr));
#ifdef BGPDUMP_HAVE_IPV6
	    } else if(e->afi == AFI_IP6) {
			inet_ntop(AF_INET6, &e->prefix.v6_addr, prefix, INET6_ADDRSTRLEN);
#endif
	    } else {
			printf("Error: BGP table dump version 2 entry with unknown subtype\n");
			break;
	    }

		for(i = 0; i < e->entry_count; i++){
			if(i){
    		//	printf("\nTIME            : %s",asctime(gmtime(&entry->time)));
    		//	printf("LENGTH          : %u\n", entry->length);
			}


    	//	printf("TYPE            : BGP Table Dump version 2 Entry\n");
    	//	printf("    SEQUENCE    : %d\n",e->seq);
    	//	printf("    PREFIX      : %s/%d\n",prefix,e->prefix_length);

			if(e->entries[i].peer->afi == AFI_IP){
				inet_ntop(AF_INET, &e->entries[i].peer->peer_ip, peer_ip, INET6_ADDRSTRLEN);
#ifdef BGPDUMP_HAVE_IPV6
			} else if (e->entries[i].peer->afi == AFI_IP6){
				inet_ntop(AF_INET6, &e->entries[i].peer->peer_ip, peer_ip, INET6_ADDRSTRLEN);
#endif
			} else {
				sprintf(peer_ip, "N/A, unsupported AF");
			}
    	//	printf("    PEER IP     : %s\n",peer_ip);
    	//	printf("    PEER AS     : %u\n",e->entries[i].peer->peer_as);
			data.source_AS= e->entries[i].peer->peer_as;
   			show_attr(e->entries[i].attr);
			
		}

	    break;

	case BGPDUMP_TYPE_ZEBRA_BGP:
	//    printf("TYPE            : Zebra BGP \n");
		if(entry->body.zebra_message.address_family == AFI_IP) {
		    strcpy(source_ip, inet_ntoa(entry->body.zebra_message.source_ip.v4_addr));
		    strcpy(destination_ip, inet_ntoa(entry->body.zebra_message.destination_ip.v4_addr));
#ifdef BGPDUMP_HAVE_IPV6
		} else if(entry->body.zebra_message.address_family == AFI_IP6) {
		    inet_ntop(AF_INET6, &entry->body.zebra_message.source_ip.v6_addr, source_ip,
			      sizeof(source_ip));
		    inet_ntop(AF_INET6, &entry->body.zebra_message.destination_ip.v6_addr, destination_ip,
			      sizeof(destination_ip));
#endif
		} else {
		    *source_ip = '\0';
		    *destination_ip = '\0';
		}
	    switch(entry->subtype) {
		case BGPDUMP_SUBTYPE_ZEBRA_BGP_MESSAGE:
		case BGPDUMP_SUBTYPE_ZEBRA_BGP_MESSAGE_AS4:
		    printf("SUBTYPE         : Zebra BGP Message");
		   // if(entry->subtype == BGPDUMP_SUBTYPE_ZEBRA_BGP_MESSAGE_AS4) {
		    //  printf(" (32-bit ASN)\n");
		 //   } else {
		     // printf("\n");
		    //}
	//	    printf("    SOURCE_AS   : %u\n",entry->body.zebra_message.source_as);
			data.source_AS=entry->body.zebra_message.source_as;
	//	    printf("    DEST_AS     : %u\n",entry->body.zebra_message.destination_as);
			data.dest_AS=entry->body.zebra_message.destination_as;
	//	    printf("    INTERFACE   : %d\n",entry->body.zebra_message.interface_index);
	//	    printf("    SOURCE_IP   : %s\n",source_ip);
	//	    printf("    DEST_IP     : %s\n",destination_ip);
		
		
		    if(entry->body.zebra_message.type > sizeof(bgp_message_types) / sizeof(bgp_message_types[0]))
			printf("MESSAGE TYPE    : Unknown\n");
		    else
			printf("MESSAGE TYPE    : %s\n", bgp_message_types[entry->body.zebra_message.type]);
			message_type=bgp_message_types[entry->body.zebra_message.type];

		    switch(entry->body.zebra_message.type) {
			case BGP_MSG_UPDATE:
			    printf("WITHDRAW        :\n");
			    show_prefixes(entry->body.zebra_message.withdraw_count,entry->body.zebra_message.withdraw);
#ifdef BGPDUMP_HAVE_IPV6
			    if(entry->attr->mp_info &&
			       (mp_withdraw = MP_IPV6_WITHDRAW(entry->attr->mp_info)) != NULL) {
				show_v6_prefixes(mp_withdraw->prefix_count, mp_withdraw->nlri);
			    }
#endif
			    printf("ANNOUNCE        :\n");
			    show_prefixes(entry->body.zebra_message.announce_count,entry->body.zebra_message.announce);
#ifdef BGPDUMP_HAVE_IPV6
			    if(entry->attr->mp_info &&
			       (mp_announce = MP_IPV6_ANNOUNCE(entry->attr->mp_info)) != NULL) {
				show_v6_prefixes(mp_announce->prefix_count, mp_announce->nlri);
			    }
#endif
			    break;
			case BGP_MSG_KEEPALIVE:
			    /* Nothing to do */
			    break;
			case BGP_MSG_OPEN:
	//		    printf("    VERSION     : %d\n",entry->body.zebra_message.version);
	//		    printf("    MY_ASN      : %u\n",entry->body.zebra_message.my_as);
	//		    printf("    HOLD_TIME   : %d\n",entry->body.zebra_message.hold_time);
	//		    printf("    ROUTER_ID   : %s\n",inet_ntoa(entry->body.zebra_message.bgp_id));
	//		    printf("    OPTION_LEN  : %d\n",entry->body.zebra_message.opt_len);
	//		    printf("    OPTION_DATA :");
			    for(i = 0; i < entry->body.zebra_message.opt_len; i++) {
				printf(" %02x", entry->body.zebra_message.opt_data[i]);
			    }
			   // printf("\n");
			    break;
			case BGP_MSG_NOTIFY:
			    code = entry->body.zebra_message.error_code;
			    subcode = entry->body.zebra_message.sub_error_code;

	//		    printf("    CODE        : %d", code);
			    if(code >= sizeof(notify_codes) / sizeof(notify_codes[0]))
				printf(" (Unknown)\n");
			    else
				printf(" (%s)\n", notify_codes[code]);

			   // printf("    SUBCODE     : %d", subcode);
			    if(code >= sizeof(notify_codes) / sizeof(notify_codes[0]) ||
			       subcode >= sizeof(notify_subcodes[0]) / sizeof(notify_subcodes[0][0]) ||
			       notify_subcodes[code][subcode] == NULL)
				printf(" (Unknown)\n");
			    else
				printf(" (%s)\n", notify_subcodes[code][subcode]);

			    printf("    DATA        :");
			    for(i = 0; i < entry->body.zebra_message.notify_len; i++) {
				printf(" %02x", entry->body.zebra_message.notify_data[i]);
			    }
			   // printf("\n");
			    break;
			default:
			    break;
		    }
		    break;

		case BGPDUMP_SUBTYPE_ZEBRA_BGP_STATE_CHANGE:
	//	    printf("SUBTYPE         : Zebra BGP State Change\n");
	//	    printf("    SOURCE_AS   : %u\n",entry->body.zebra_state_change.source_as);
		source_as=entry->body.zebra_state_change.source_as;
	//	    printf("    DEST_AS     : %u\n",entry->body.zebra_state_change.destination_as);
	//	dest_as=entry->body.zebra_state_change.dest_as;
	//	    printf("    INTERFACE   : %d\n",entry->body.zebra_state_change.interface_index);
	//	    printf("    SOURCE_IP   : %s\n",source_ip);
	//	    printf("    DEST_IP     : %s\n",destination_ip);
	//	    printf("    OLD_STATE   : %s\n",bgp_state_name[entry->body.zebra_state_change.old_state]);
	//	    printf("    NEW_STATE   : %s\n",bgp_state_name[entry->body.zebra_state_change.new_state]);
    		show_attr(entry->attr);
		    break;

		default:
		    printf("SUBTYPE         : Unknown %d\n", entry->subtype);
	    }
    	show_attr(entry->attr);
	    break;
	default:
	    printf("TYPE            : Unknown %d\n", entry->type);
    	show_attr(entry->attr);
	    
    }
    //printf("\n");	
}

void show_attr(attributes_t *attr) {
    int have_nexthop = 0;
    string hop_withdraw;
   // printf("ATTRIBUTES      :\n");
     
    if(attr != NULL) {
       	if(message_type=="Update/Withdraw"&& (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_AS_PATH) ) !=0)
		{
		hop_withdraw=attr->aspath->str;
		it_hops_vector = find(hops_vector.begin(),hops_vector.end(),hop_withdraw);
		if(it_hops_vector!=hops_vector.end())
		{
			iter_swap(it_hops_vector,hops_vector.end()-1);
			hops_vector.erase(hops_vector.end());
		}
		}
      //  if( (attr->flag & ATTR_FLAG_BIT (BGP_ATTR_ORIGIN) ) !=0 )      printf("   ORIGIN       : %d\n",attr->origin);
       // else printf("   ORIGIN       : N/A\n");
 
        if( (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_AS_PATH) ) !=0) 
        {
        //printf("   ATTR_LEN     : %d\n",attr->len);
       // printf("   ASPATH       : %s\n",attr->aspath->str);
            data.hops=attr->aspath->str;
            //hops_length=strlen(data.hops);
            //hops_maxlength=max(hops_length, hops_maxlength);
            //printf("   Length       : %i\n",hops_maxlength);
            //printf("   ASPATH       : %s\n",data.hops);
            //printf("   START       : %i\n",data.source_AS);
            //printf("   DESTINATION       : %i\n",data.dest_AS);
            bgplist.push_front(data);
            it = bgplist.begin();
            hops_vector.push_back(data.hops);
            //it_hops_vector=hops_vector.end();
            //cout<<"Vector_Liste"<<hops_vector[counter]<<'\n';
            //printf("   LISTE_Hops       : %s\n",it->hops);
            //printf("   LISTE_Source     : %i\n",it->source_AS);
            //printf("   LISTE_Dest      : %i\n",it->dest_AS);
            ++counter;
            //cout <<counter << '\n';
            have_nexthop = 1;
        //printf("%s", inet_ntoa(attr->nexthop));
             
             
        }
        //else printf("   ASPATH       : N/A\n");
 
       // printf("   NEXT_HOP     : ");
        if( (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_NEXT_HOP) ) !=0) {
        have_nexthop = 1;
        //printf("%s", inet_ntoa(attr->nexthop));
       }
 
#ifdef BGPDUMP_HAVE_IPV6
        if( (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_MP_REACH_NLRI)) &&
             MP_IPV6_ANNOUNCE(attr->mp_info) != NULL) {
        char addr[INET6_ADDRSTRLEN];
        struct mp_nlri *mp_nlri = MP_IPV6_ANNOUNCE(attr->mp_info);
        u_int8_t len = mp_nlri->nexthop_len;
 
        if(have_nexthop)
           // printf(" ");
 
        have_nexthop = 1;
        //printf("%s", inet_ntop(AF_INET6, &mp_nlri->nexthop, addr, sizeof(addr)));
        //if(len == 32)
         // printf(" %s", inet_ntop(AF_INET6, &mp_nlri->nexthop_local, addr, sizeof(addr)));
        }
#endif
 
        printf(have_nexthop ? "\n" : "N/A\n");
 
       // if( (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_COMMUNITIES) ) !=0)   printf("   COMMUNITIES  : %s\n",attr->community->str);
       // else printf("   COMMUNITIES  : N/A\n");
 
        if( (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_NEW_AS_PATH) ) !=0) {
        //printf("   NEW_ASPATH   : %s\n",attr->new_aspath->str);
        //  printf("   OLD_ASPATH   : %s\n",attr->old_aspath->str);
        }
 
        if( (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_NEW_AGGREGATOR) ) !=0)  printf("   NEW_AGGREGTR : %s AS%u\n",inet_ntoa(attr->new_aggregator_addr),attr->new_aggregator_as);
    }
 
}

void show_prefixes(int count,struct prefix *prefix) {
   // int i;
   //for(i=0;i<count;i++)
		//printf("      %s/%d\n",inet_ntoa(prefix[i].address.v4_addr),prefix[i].len);
}

#ifdef BGPDUMP_HAVE_IPV6
void show_v6_prefixes(int count, struct prefix *prefix) {
    int i;
    char str[INET6_ADDRSTRLEN];

    for(i=0;i<count;i++){
	inet_ntop(AF_INET6, &prefix[i].address.v6_addr, str, sizeof(str));
	//printf("      %s/%d\n",str, prefix[i].len);
    }
}
#endif
void graph_build(list<BGPData> complete_list)
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
	for(int i=0;i<counter; ++i) //create the arcs
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
					it_arc_ID=arc_ID.find(whole);	
					if(it_arc_ID==arc_ID.end())
					{
						arc= g.addArc(g.nodeFromId(start_id), g.nodeFromId(dest_id));
						whole=to_string(start_as)+" "+to_string(dest_as);
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
int h =countArcs(g);
}

void calculate_data()
{
		
	string s,part_1,node_arc,node_arc_rev;
	istringstream iss(s);	
	//vector<string> vstrings;
	//int vstrings_size;	
	int j;
	int arc_val1=0;
	int arc_val2=0;
	int check=0;
	size_t found=-1;
	size_t amount=1;
	int stras=0;
	int node_degree_1=0;
	int node_degree_2=1;
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
		//cout<<"Map degree " <<it_as_ID->second << " <= "<<map_degree[it_as_ID->second]<<'\n';
		if(map_degree[it_as_ID->second]==0) as_ID.erase(it_as_ID);
		++it_as_ID;
	}
	#pragma omp parallel for private(hop_degree, max_as, s, amount,part_1, hop_as,hop_as2,it_as_ID,it_arc_ID, node_arc, found, hop_degree_max, check)
	for(int i=0;i<counter; ++i)
	{	vector<string> sstrings;
		sstrings.clear();	
		s = hops_vector.at(i);// save the hops in a string
		amount=1;
		for(;amount>=0;)
		{
			amount=count(s.begin(),s.end(), ' ');
			found=s.find(' ');//search first space
			part_1=s.substr(0,found); //split the hop-string in two strings			
			s=s.substr(found+1,s.length()); //set as_hops to the rest of the string		
			sstrings.push_back(part_1);
			if (amount==0) break;	
		}
		hop_degree_max=0;
		max_as=0;
		for(int k=0;k<sstrings.size();++k)
		{
			hop_as=atoi(sstrings[k].c_str()); //take an AS from the path
			it_as_ID=as_ID.find(hop_as); //find the AS in the map
			hop_degree=map_degree[it_as_ID->second]; //take the degree from the node map
			//cout<<"Hop degree"<<hop_degree<<'\n';
			if(hop_degree>hop_degree_max)
			{
				hop_degree_max=hop_degree; // save the higher degree
				//cout<<"Max degree"<<hop_degree_max<<'\n';
				max_as=hop_as; //save the corresponding AS
				//cout<<"Max AS"<<max_as<<'\n';
				
			}		
		}
		check=0;
		for(int l=0;l<sstrings.size()-1;++l)
		{
			if(sstrings.size()==1 || sstrings.size()==0)break;
			hop_as=atoi(sstrings.at(l).c_str()); //save the start AS
			hop_as2=atoi(sstrings.at(l+1).c_str()); //save the dest AS
			if(check==0)
			{
				node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
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
	#pragma omp for private(hop_degree, max_as, s, amount,part_1, hop_as,hop_as2,it_as_ID, hop_check,it_arc_ID, node_degree_1, it_arc_ID2, node_degree_2, node_arc, found, hop_degree_max, check)
	for(int m=0;m<counter;m++)
	{
		vector<string> sstrings;
		sstrings.clear();
		hop_degree_max=0;
		check=0;
		max_as=0;
		s = hops_vector.at(m);// save the hops in a string
		amount=1;
		for(;amount>=0;)
		{	
			amount=count(s.begin(),s.end(), ' ');		
			found=s.find(' ');//search first space
			part_1=s.substr(0,found); //split the hop-string in two strings			
			s=s.substr(found+1,s.length()); //set as_hops to the rest of the string		
			sstrings.push_back(part_1);
			if (amount==0) break;
		}
		for(int n=0;n<sstrings.size();++n)
		{
			hop_as=atoi(sstrings.at(n).c_str()); //take the AS
			//cout<<sstrings[n]<<'\n';
			it_as_ID=as_ID.find(hop_as); //search the AS in the map
			hop_degree=map_degree[it_as_ID->second]; //take the degree from the node map
			if(hop_degree>hop_degree_max)
			{
				hop_degree_max=hop_degree; // save the higher degree
				max_as=hop_as; //save the corresponding AS
			}		
		}
		for(int o=0;o<sstrings.size()-2;++o)
		{
			if(sstrings.size()==1 || sstrings.size()==0)break;
			hop_as=atoi(sstrings.at(o).c_str()); //take the start AS
			hop_as2=atoi(sstrings.at(o+1).c_str()); //take the dest AS
			hop_check=atoi(sstrings.at(o+2).c_str()); //check for the AS(highest degree)
			if(check==0)
			{
				node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
				it_arc_ID=arc_ID.find(node_arc); //find the arc in the map
				map_p2p[it_arc_ID->second]=1; //set the map p2p of the arc to 1 
				if(hop_check==max_as) check=-1;
			}
			else	
			{
				it_as_ID=as_ID.find(hop_as);//find the node (the one before the highest degree)
				node_degree_1=map_degree[it_as_ID->second];//get the corresponding degree
				node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
				it_arc_ID=arc_ID.find(node_arc); //search the string in the map
				++o; //increment o
				hop_as=atoi(sstrings.at(o).c_str()); //take the start AS
				hop_as2=atoi(sstrings.at(o+1).c_str()); //take the dest AS
				node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
				it_arc_ID2=arc_ID.find(node_arc); //search the string in the map
				it_as_ID=as_ID.find(hop_as2); //find the node after the highest degree
				node_degree_2=map_degree[it_as_ID->second]; //get the corresponding degree
				it_map_p2c=map_p2c.find(it_arc_ID->second);
				it_map_c2p=map_c2p.find(it_arc_ID->second);
				if(it_map_p2c->second==-1&&it_map_c2p->second==1)
				{
					it_map_p2c=map_p2c.find(it_arc_ID2->second);
					it_map_c2p=map_c2p.find(it_arc_ID2->second);
					if(it_map_p2c->second==-1&&it_map_c2p->second==1)  
					{
						if(node_degree_1>node_degree_2)//if the degree of the node before the Top Provider is higher
						{			
							map_p2p[it_arc_ID->second]=1;//set the arc to the Top Provider equals 1
							check=0;
							//++o; //increment o to ignore the next node
						}
						else
						{
							map_p2p[it_arc_ID2->second]=1;//set the arc from the Top Provider equals 1
							check=0;
							//++o;//increment o to ignore the next node
						}
					}
				}
			}		
		}			
	}
	
	#pragma omp for private(s, amount,part_1, hop_as,hop_as2,it_as_ID, hop_check,it_arc_ID, node_degree_1, node_degree_2, node_arc, found, hop_degree_max, arc_val1, arc_val2, node_result)
	for(int p=0;p<counter;p++)
	{
		vector<string> sstrings;
		sstrings.clear();
		s = hops_vector.at(p);// save the hops in a string
		for(;amount>=0;)
		{
			amount=count(s.begin(),s.end(), ' ');
			found=s.find(' ');//search first space
			part_1=s.substr(0,found); //split the hop-string in two strings			
			s=s.substr(found+1,s.length()); //set as_hops to the rest of the string		
			sstrings.push_back(part_1);
			if (amount==0) break;
		}
		//sstrings_size=sstrings.size(); //get the size of the vector
		for(int q=0;q<sstrings.size()-1;q++)
		{	if(sstrings.size()==1 || sstrings.size()==0) continue;
			hop_as=atoi(sstrings.at(q).c_str()); //take the start AS
			hop_as2=atoi(sstrings.at(q+1).c_str()); //take the dest AS
			node_arc=to_string(hop_as)+" "+to_string(hop_as2); //save them as a string
			it_arc_ID=arc_ID.find(node_arc); //search the string in the map
			it_map_p2p=map_p2p.find(it_arc_ID->second); //search the arc in the map
			//arc_val1=it_map_p2p->second; //get the value of the arc
			if(it_map_p2p==map_p2p.end()) //check if peering is okay, if it is in the map, nonpeering is active
			{
				node_arc_rev=to_string(hop_as2)+" "+to_string(hop_as); //save the reverse arc
				it_arc_ID2=arc_ID.find(node_arc_rev); //search the string in the map
				it_map_p2p=map_p2p.find(it_arc_ID2->second); //search the arc in the map
				//arc_val2=it_map_p2p->second; //get the value of the reverse arc
				it_as_ID=as_ID.find(hop_as); //search the AS in the map
				node_degree_1=map_degree.at(it_as_ID->second); //get the degree of the node
				it_as_ID=as_ID.find(hop_as2); //search the AS in the map
				node_degree_2=map_degree.at(it_as_ID->second);  //get the degree of the node
				if(node_degree_2<1) continue;
				node_result=node_degree_1/node_degree_2;
				if(it_map_p2p==map_p2p.end() && node_result<r_border && node_result>1/r_border)
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
		if(stras==desas)continue;
		if(map_c2p.find(it_arc_ID->second)!=map_c2p.end()) c2p=map_c2p[it_arc_ID->second];
		if(map_p2c.find(it_arc_ID->second)!=map_p2c.end()) p2c=map_p2c[it_arc_ID->second];
		check=0;
		
		if(c2p==2&&p2c==2) //if it is peer to peer
		{
			fileout << stras<< '\t'<<desas<< '\t' <<"p2p"<<'\n';
			check=1;
		}
		if(c2p==1&&p2c==-1) //if it is sibling to sibling
		{
			fileout << stras<< '\t'<<desas<< '\t' <<"s2s"<<'\n';
			check=1;
		}
		if(p2c==-1&&check!=1) //if it is provider to customer
		{
			fileout << stras<< '\t'<<desas<< '\t' <<"p2c"<<'\n';
			check=1;
		}
		if(c2p==1&&check!=1) //if it is customer to provider
		{
			check=1;
			fileout << stras<< '\t'<<desas<< '\t' <<"c2p"<<'\n';
		}		
		if(check!=1) // if it is not clasificable
		{
			fileout << stras<< '\t'<<desas<< '\t' <<"Undefined"<<'\n';
		}	
	}
	fileout.close();	
}	

