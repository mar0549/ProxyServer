/*
Michael Rivera
CSCE 3530 Introduction to Computer Networks
Section 001
Submitted : 09/03/2019
Description: Implementation of a proxy server. Must be executed with one argument. This argument will be the port number that the user would like to use. This proxy will handle http requests from a client that is properly connected to it, and relay the website to the client

MUST BE RAN ON cse01.cse.unt.edu for client to connect properly

This program was built off of a supplied server code provided by Professor Qing Yang

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

//function webConn is where the HTTP request is forwarded to the web server
char* webConn(char* recvurl,char* webRequest);


int main(int argc, char **argv)
{

    char str[200];	//used for the buffer
    int listen_fd, conn_fd;	//connection and listening  file descriptor
    struct sockaddr_in servaddr;	//structure for the server address
    int portNum;  //port number used from argv[1]
    char webRequest[40960]; //hold request to web server
    char* webResponse;  //response from web server
    char recvurl[100];  //user desired url to connect to
  //  int n;

    //error message if pserver is not ran correctly
    if(argc!=2){
	printf("Please re-run the program in the following format... pserver <port_number>\n");
    	exit(1);
    }
    else
    	portNum = atoi(argv[1]);        //assign second command line argument to portNum


    /* AF_INET - IPv4 IP , Type of socket, protocol*/
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    //zero server address info
    bzero(&servaddr, sizeof(servaddr));

    //populate servaddr structure
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(portNum);

    /* Binds the above details to the socket */
	bind(listen_fd,  (struct sockaddr *) &servaddr, sizeof(servaddr));
	/* Start listening to incoming connections */
	listen(listen_fd, 10);

  while(1)//infinite loop
  {
      /* Accepts an incoming connection */
	    conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
      //zer recvurl content
      bzero(&recvurl,100);
      //read url sent from client
      recv(conn_fd, recvurl, sizeof(recvurl),0);
      //read HTTP request from client
      recv(conn_fd, webRequest, sizeof(webRequest),0);

      //webRequest holds return value from weConn, which will be the web server response
      webResponse = webConn(recvurl,webRequest);
      //url is used in webConn function to request page from web server
      send(conn_fd,webResponse,strlen(webResponse),0);


      close (conn_fd); //close the connection
  }//end of while loop

}//end of main function


//function webConn is where the HTTP request is forwarded to the web server
char* webConn(char* recvurl,char* webRequest)
{
  int web_fd; //fd connected to the net
  struct sockaddr_in webaddr;	//structure for the net address
  char* urlDomain;
  struct hostent *hostName; //host hostName
  char* IPaddr; //holds the sites IP address in correct format

  //get hostname
  if((hostName= gethostbyname(recvurl)) == NULL)
  {
    perror("gethost");
    exit(1);
  }
  //get IP address for clients requested url domain
  IPaddr = inet_ntoa(*((struct in_addr*) hostName->h_addr_list[0]));

  //create socket to web server
  web_fd=socket(AF_INET,SOCK_STREAM,0);
  if(web_fd<0)
  {
    perror("web_fd");
    exit(1);
  }

  //remove webaddr content
  bzero(&webaddr,sizeof(webaddr));

  //populate webaddr structure
  webaddr.sin_family=AF_INET;
  webaddr.sin_port=htons(80); // Server port number

  /* Convert IPv4 and IPv6 addresses from text to binary form */
  inet_pton(AF_INET,IPaddr,&(webaddr.sin_addr));

  /* Connect to the server */
  if(connect(web_fd,(struct sockaddr *)&webaddr,sizeof(webaddr))<0)
  {
    perror("Connection");
    exit(1);
  }
  //forward client HTTP request to web server
  if (write(web_fd, webRequest, strlen(webRequest))<0)
  {
      perror("write to web");
      exit(1);
  }
  //empty webRequest content
  bzero(webRequest,40960);
  read(web_fd,webRequest,40960);  //read in web server response to HTTP request

  return webRequest;


} //end of webConn function
