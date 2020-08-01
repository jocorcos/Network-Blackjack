//Omar Ejaz
//Cosc 370
//Poker game, part 1
//client.c


#include <stdio.h> 
#include <stdlib.h>                                                                                                                                              
#include <strings.h>                                                                                                                                             
#include <sys/types.h>                                                                                                                                           
#include <sys/socket.h>                                                                                                                                          
#include <sys/time.h>                                                                                                                                           
#include <netinet/in.h>                                                                                                                                       
#include <netdb.h>                                                                                                                                                
#include <time.h> 
  
#define      SA           struct sockaddr /* generic socket address */  
#define      LISTENQ 1024              /* 2nd argument to listen() */ 
#define      MAXLINE      4096         /* max text line length */    

char responsebuffer[MAXLINE];

void err_sys(char *);
int main(int argc, char **argv)
{
int sockfd, n; /*socket file descriptor */

char recvline[MAXLINE + 1];

struct sockaddr_in servaddr;/*server address */



if (argc != 2)
err_sys("argument error");

/*create a socket to receive data from server */
if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
err_sys("socket error");

/* shall place n zero-valued bytes in the area pointed to by serveraddr */
bzero(&servaddr, sizeof(servaddr));

/* server using IPv4 */
servaddr.sin_family = AF_INET; 

/* port number used by server.  htons function format port number to short*/
servaddr.sin_port   = htons(1313);

/* converts the character string argv[i] into a network address structure 
/* in the IPv4 address family, then copies  to servaddr.sin_addr.  */
if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
err_sys("inet_pton error");

/*try to connect to server */
if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
err_sys("connect error");



/* receive information from the daytime server*/

while ( (n = read(sockfd, recvline, MAXLINE)) > 0) 
{
recvline[n] = 0; /* null terminate */
if (fputs(recvline, stdout) == EOF)
err_sys("fputs error");

fgets(responsebuffer,MAXLINE,stdin);
if((write(sockfd, responsebuffer, strlen(responsebuffer) )) < 0) 
err_sys("fgets error");

}
if (n < 0)
err_sys("read error");


close(sockfd);
exit(0);
}

void err_sys(char *str)
{
printf ("%s \n",str);
exit (1);
}
