/////////////////////////////////////////////////////SLAVE DISTRIBUTED SERVER/////////////////////////////////////////////////////
//Code developed and contributed by Muhammad Hamza (mhamzai) can be used in opensource project development by giving due credits//
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <arpa/inet.h> 
#include <unistd.h>  
#include <iostream>
#include <fstream>
using namespace std;
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/json/rpcserver.h>
#include <cxxtools/eventloop.h>
#include <thread> 
#include <string>

static int total=0;
int wordc(string fname,int start,int end)
{
    ifstream file; 

	cout << fname;
    string word, t, q, filename; 
    int wc=0;
    filename = fname; 
  
    file.open(fname); 
	cout << fname;
    file.seekg(start, ios::beg);
    while (file.tellg() < start+end) 
    {
	file >> word;
        wc++; 
    } 
  cout << wc << " word count" <<endl;
    return wc; 
}

int instancec(string fname,string srch,int start,int end)
{   ifstream file; 

	cout << fname;
    string word, t, q, filename; 
    int wc=0;
    filename = fname; 
  
    file.open(fname); 
	cout << fname;
    file.seekg(start, ios::beg);
    while (file.tellg() < start+end) 
    {
	file >> word;
	if (word.compare(srch)==0)
        {wc++;} 
    } 
  cout <<wc<<" inst count"<<endl;
    return wc; 
}


void run(int argc, char* argv[])
{
    cxxtools::Arg<std::string> ip(argc, argv, 'i');

    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 8080);

    cxxtools::EventLoop loop;

    cxxtools::json::RpcServer jsonServer(loop, "127.0.0.2", port);

    jsonServer.registerFunction("wordc", wordc);
    jsonServer.registerFunction("instancec", instancec);

    loop.run();
}
int main(int argc, char* argv[])
{
	int sock_fd, client;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	if (sock_fd = socket(AF_INET, SOCK_STREAM, 0))
	{
		printf("Slave socket created\n");
	}
	else
	{
		printf("socket creation failed");
		return 1;	
	}
 	thread th1(run, argc, argv);
	int enable = 1;
	setsockopt(sock_fd, SOL_SOCKET,SO_REUSEPORT, &enable,sizeof(enable));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.2", &address.sin_addr);
	address.sin_port = 8080;
	bind(sock_fd, (const sockaddr*)& address,sizeof(address));
	listen(sock_fd, 1);
	client = accept(sock_fd, (sockaddr*)&address,(socklen_t*)&addrlen);
	char msg[100] = {"\n"};
	char rep[100] = "yes";
while(1){
	read(client, msg, 100);
	printf("slave read: %s\n", msg);
	send(client, rep, strlen(rep), 0);
	printf("slave replied: %s\n", rep);
}
	return 0;
}
