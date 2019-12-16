/*
Michael Rivera
CSCE 3530 Introduction to Computer Networks
Section 001
Submitted : 09/03/2019
Description: TCP client. Sends HTTP requests to pserver.c and is relayed the response from the proxy

This program was built off of a supplied client code provided by Professor Qing Yang
*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

int main(int argc,char **argv)
{
    int sockfd, n;	//socket file descriptor and n is used for the read() return value
    int len = sizeof(struct sockaddr);	//length of the structure sockaddr
    char recvline[40960];	//array for recieved data
    struct sockaddr_in servaddr;	//structure for the server address
    int portNum;	//port number
    char url[500];	//url string
    char* urlDomain;  //url domain
    char* urlPath;  //url path
    char webRequest[1000];  //GET web request

    //error message if the wrong number of command line arguments are used
    if(argc!=2){
    	printf("Please re-run the program in the following format... client <port_number>\n");
    	exit(1);
    }
    else
	    portNum = atoi(argv[1]);        //assign second command line argument to portNum


    /* AF_INET - IPv4 IP , Type of socket, protocol*/
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
	    perror("Socket:");
	    exit(1);
    }
    //remove servaddr content
    bzero(&servaddr,sizeof(servaddr));

    //populate servaddr structure
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(portNum); // Server port number

    /* Convert IPv4 and IPv6 addresses from text to binary form */
    //IP address of cse01.cse.unt.edu
	inet_pton(AF_INET,"129.120.151.94",&(servaddr.sin_addr));

    /* Connect to the server */
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
    {
    	perror("Connection");
    	exit(1);
    }

    //remove url content
    bzero(&url,500);
    //prompt for URL input
    printf("URL: ");
    scanf("%s", url);

    //tokenize url into the url domain and path
    urlDomain = strtok(url,"/");
    urlPath = strtok(NULL,"\n");
    if(urlPath==NULL) //make path empty if it is NULL
      urlPath = "";

    send(sockfd, urlDomain, strlen(urlDomain),0); // send urlDomain to server

    //make http request
    bzero(webRequest,1000);
    sprintf(webRequest,"GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n",urlPath,urlDomain);

    send(sockfd, webRequest, strlen(webRequest),0); // send HTTP request to server

    bzero(recvline,40960);
    //receive HTTP response from proxy server
    recv(sockfd, recvline, sizeof(recvline),0);

    printf("%s\n",recvline);  //print HTTP response
}//end of main function
