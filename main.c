/* 
 * File:   main.c
 * Description: Converts any amount from one currency to other
 * Usage: currency PKR 500 USD
 * Author: danish
 * Email: dasatti@gmail.com
 * fb: www.facebook.com/danish.satti
 *
 * Created on January 14, 2015, 1:00 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>



#define BUFFSIZE 4096
#define HOST "www.google.com"


/*
    Get ip from domain name
 */
 
int hostname_to_ip(char * hostname , char* ip)
{
    // src: http://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
     
    return 1;
}

void print_usage(){
    printf("Usage  : currency from amount to\n"
           "Example: currency PKR 500 USD\n");
}

int main(int argc, char** argv) {
    
    char *from, *to;
    float amount;
    
    if(argc >=4 ){
        from = argv[1];
        amount = atof(argv[2]);
        to = argv[3];
    } else {
        print_usage();
        exit(1);
    }
    
    
    
    int soc, msg_size ;
    struct sockaddr_in server;
    char message[150], server_response[BUFFSIZE];
    
    soc = socket(AF_INET,SOCK_STREAM,0);
    if(soc==-1){
        printf("Could not create socket");
        exit(1);
    }
    
    
    char ip[100];
    hostname_to_ip(HOST,ip);
    
    
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    
    if(connect(soc,(struct sockaddr * )&server, sizeof (server))<0){
        printf("Could not connect to server");
        exit(1);
    }
    //printf("Connected to server\n");
    
    //message = "GET /finance/converter?a=20&from=PKR&to=USD HTTP/1.1\r\n\r\n";
    sprintf(message,"GET /finance/converter?a=%f&from=%s&to=%s HTTP/1.1\r\n\r\n",amount,from,to);
    
    if(send(soc,message,strlen(message),0)<0){
        printf("Could not send request");
        exit(1);
    }
    
    do {
        msg_size = recv (soc, server_response, BUFFSIZE,0);
        //msg_size = read (soc, server_response, BUFFSIZE);
        
        if(strstr(server_response,"result>")!=NULL){
            char * start = strstr(server_response,"result>");
            char * end = strstr(start,"</span>");

            if (end == NULL || start == NULL){
                printf("Invalid input parameters\n");
                exit(1);
            }

            *end = '\0';

            start = start+7;
            char * res = strstr(start,"=bld>");
            char * to_end = strstr(start,"=");
            *to_end = '\0';
            printf("%s = %s\n",start,res+5);
            
            break;
        }
        
        if(strstr(server_response,"</html>")!=NULL){
            printf("Invalid input parameters\n");
            break;
        }
    } while ( msg_size > 0 );
    
    close(soc);
    

    return (EXIT_SUCCESS);
}

