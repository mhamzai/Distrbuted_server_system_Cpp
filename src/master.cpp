/////////////////////////////////////////////////////////MASTER SERVER////////////////////////////////////////////////////////////
//Code developed and contributed by Muhammad Hamza (mhamzai) can be used in opensource project development by giving due credits//
#include <stdio.h> 
#include <sys/socket.h> 
#include <sys/time.h>
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <string>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <bits/stdc++.h>
#include <unistd.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h> 
#include <fstream>
#include <iostream>
using namespace std;
#include <cxxtools/arg.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/json/rpcclient.h>
#include <thread> 
#include <mutex>

std::mutex global_mutex;
int jobdone=0;
char IPs[100][100];
int isalive[100] = {0};
int sock_fd[100];
int sizes[100];

int callfunc(char* IP, int* wc, int* insc, string fn, string src, int beg, int end,int argc, char **argv)
{

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 8080);
    cxxtools::json::RpcClient client(IP, port);
    cxxtools::RemoteProcedure<int, string, int, int> wordc(client, "wordc");
    cxxtools::RemoteProcedure<int, string, string, int, int> instancec(client, "instancec");
    *wc = wordc(fn, beg, end);
	cout<<"ec : " <<*wc;
    *insc = instancec(fn , src,beg,end);
    return 1;
}

int alivecheck(int c_count, int temp_tm)
{

  fd_set readfds;

	char msg[100] ="isalive";
	char rd[100] = {"\n"};

struct timeval tv;
tv.tv_sec = 1;
tv.tv_usec = 0;

int all = c_count;
while(1){
if (jobdone==1){break;}
global_mutex.lock();
for (int i=0;i<c_count;i++){
	strncpy(rd,"\n",1);
	if (all == 0){printf("All slaves died...\n");return 2;}
	if(isalive[i]==0){continue;}
	send(sock_fd[i] , msg , strlen(msg) , 0 ); 
	printf("master to %s: %s\n",IPs[i],msg ); 
	printf("sz %d",sizes[i]);
	FD_ZERO(&readfds);          /* initialize the fd set */
        FD_SET(sock_fd[i], &readfds); /* add socket fd */
    	FD_SET(0, &readfds);        /* add stdin fd (0) */
    	if (select(sock_fd[i]+1, &readfds, NULL, NULL, &tv)) 
	{read( sock_fd[i] , rd, 100); 
	printf("master read: %s\n",rd ); }
	if(strncmp(rd,"yes",3))
	{isalive[i]=isalive[i]-1; }
	else {isalive[i]=temp_tm;}
	if(isalive[i]==0){printf("Slave %s dead\n",IPs[i]);all=all-1;}
	}
global_mutex.unlock();
sleep(1);
}
return 0;
}

int main(int argc, char **argv) 
{ 
	char         *docname;
	  xmlDocPtr    doc;
	  xmlNodePtr   cur;
	  xmlChar      *uri;
	char temp[100];
	int start[100];
	int done[100] = {0};
	int wordcount[100];
	int inscount[100];
	struct sockaddr_in address[100]; 
	
	int i=0;
if (argc <= 1) {
    printf("Usage: %s docname\n", argv[0]);
    return(0);
  }

  docname = argv[1];
 string filen;
  doc = xmlParseFile(docname);
  cur = xmlDocGetRootElement(doc);
int temp_tm=0;
int c_count=0;
  cur = cur->xmlChildrenNode;
  while (cur != NULL) {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"IP"))) {
        uri = xmlGetProp(cur, (const xmlChar *)"uri");
	strcpy(IPs[c_count],(char*)uri);
	xmlFree(uri);
	uri = xmlGetProp(cur, (const xmlChar *)"sz");
	strcpy(temp,(char*)uri);
	sizes[c_count] = atoi(temp);
	xmlFree(uri);
	uri = xmlGetProp(cur, (const xmlChar *)"st");
	strcpy(temp,(char*)uri);
	start[c_count] = atoi(temp);
	xmlFree(uri);
	c_count=c_count+1;
        
      }
	else if ((!xmlStrcmp(cur->name, (const xmlChar *)"timeout")))
	{
	uri = xmlGetProp(cur, (const xmlChar *)"tm");
	temp_tm = atoi((char*)uri);
	printf("Time out after %d tries...\n",temp_tm);
        xmlFree(uri);
	}
	else if ((!xmlStrcmp(cur->name, (const xmlChar *)"file")))
	{
	uri = xmlGetProp(cur, (const xmlChar *)"fl");
	cout <<uri <<"is uri"<<endl;
	filen = (char*)uri;
	cout << filen;
        xmlFree(uri);
	}
      cur = cur->next;
  }
  xmlFreeDoc(doc);
for (i=0;i<c_count;i++){
	if (sock_fd[i] = socket(AF_INET, SOCK_STREAM, 0)) 
	{ 
		printf("master socket created\n");
	} 
	else
	{		printf("\n master creation error \n"); 
			return -1; 
	}
	isalive[i]=temp_tm;
}
for (i=0;i<c_count;i++){
	address[i].sin_family = AF_INET; 
	address[i].sin_port = 8080; 
	inet_pton(AF_INET, IPs[i], &address[i].sin_addr);
	printf("%s",IPs[i]);
	connect(sock_fd[i], (struct sockaddr *)&address[i], sizeof(address[i]));
}
cout<<endl<<"Enter word to count : ";
string srch;
cin >> srch;

std::thread th1(alivecheck, c_count, temp_tm);
      for (i =0 ;i<c_count;i++)
	{
	   done[i] = callfunc(IPs[i], &wordcount[i], &inscount[i], filen, srch, start[i], sizes[i],argc,argv);
	}
int totalw=0;
int totalins=0;

for (i = 0; i<c_count;i++)
{
	if (done[i]==1)
	{
	totalw+=wordcount[i];
	totalins+=inscount[i];
	}
	else if(isalive[i]!=0)
	{
		for (int x = 0;x<c_count;x++)
		{
			if (done[x]==1 && isalive[x] != 0)
			{
				done[i] = callfunc(IPs[x], &wordcount[i], &inscount[i], filen, srch, start[i], sizes[i],argc,argv);
				cout << "Lazy client tast reschudled to client "<<x<<endl;
				i--;
				break;
			}
		}
	}
	else if (isalive[i]==0)
	{
		for (int x = 0;x<c_count;x++)
		{
			if (done[x]==1 && isalive[x] != 0)
			{
				done[i] = callfunc(IPs[x], &wordcount[i], &inscount[i], filen, srch, start[i], sizes[i],argc,argv);
				cout << "Dead client tast reschudled to client "<<x<<endl;
				i--;
				break;
			}
		}
	}
}
ofstream MyFile("output.txt");
  MyFile << "Words : " << totalw << "   Search instances : " <<totalins<<endl;
  MyFile.close();
jobdone=1;
th1.join();
cout<<endl<<endl<<"JOB DONE SUCCESSFULLY... EXITING NOW"<<endl<<endl;
return 0; 
} 
