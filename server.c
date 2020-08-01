//Omar Ejaz
//Cosc 370
//Poker game
//server.c

#include <stdio.h> 
#include <stdlib.h>                                                                                                                                              
#include <strings.h>     
#include <string.h>                                                                                                                                         
#include <sys/types.h>                                                                                                                                           
#include <sys/socket.h>                                                                                                                                          
#include <sys/time.h>                                                                                                                                           
#include <netinet/in.h>                                                                                                                                       
#include <netdb.h>                                                                                                                                                
#include <time.h> 
#include <stdio.h>
#include <stdlib.h> 
  
#define      SA           struct sockaddr /* generic socket address */  
#define      LISTENQ 1024              /* 2nd argument to listen() */ 
#define      MAXLINE      4096         /* max text line length */   

char responsebuffer[MAXLINE];


struct Card
{
int value;
int suite;
char suite_char;
char value_char;
};

//function prototypes
void initializeDeck();
void shuffleDeck();
void initializePlayer();
int checkDuplicate(int temp2[], int n);
void Redraw(int i);
char* checkHand();
void initializeDealer();
int returnsize();
void displayCard(char,const char, char*);

//some variables
struct Card Deck[52];
struct Card playerDeck[10];
struct Card dealerDeck[10];
int balance;

//handle zombie process
void sig_chld(int signo)
{
pid_t pid;
int stat;
pid = wait(&stat);
printf("child %d terminated\n", pid);
return;
}

int main()
{

int len, listenfd, connfd;
/* for server and client address */
struct sockaddr_in servaddr, cliaddr;


/*create a socket for server using IPv4 with TCP */
listenfd = socket(AF_INET, SOCK_STREAM, 0);
/*clear struct sokaddr_in for server */
bzero(&servaddr, sizeof(servaddr));
/* use IPv4 */
servaddr.sin_family = AF_INET;
/* allow clients to connect using any one of the host's IP addresses */
servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
/* set daytime server port as 1313. htons function format port number to short*/
servaddr.sin_port = htons(1313);
/* bind a space for client request */
bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
/* waiting for client request */
listen(listenfd, LISTENQ); /* seqential server need queue */



//allows multiple clients (10)
listen(listenfd,10);
pid_t pid;
    
                                                                      
while(1) 
   {   
len = sizeof(cliaddr);
connfd = accept(listenfd, (SA *) &cliaddr, &len);
     //code to add fork and child processes

    if((pid = fork()) < 0)
    {
		printf("fork error");
		exit(1);
    }
	 
     //if the pid is 0, than it is in the parent process
     else if(pid == 0)
    {
        //if the pid is less than 0, than it is a child process

		if((pid = fork()) < 0)
            {
   			printf("fork error");
			exit(2);
            }
            else if(pid > 0)
			{ 
			exit(0);
			}

        
//this occurs for each child process
sleep(2);	
gameLoop(connfd);
close(connfd); 
exit(0);
}
	  
       
//waits for the child process to exit	  

 if (waitpid(pid, NULL, 0) != pid)
    printf("PID ERROR");
    close(connfd);     
  }



}


int gameLoop(int param1)
{
//the paramter for connection information
int connfd = param1;
balance = 0;

int i,z,j,k,l;
char buff[MAXLINE]; /* for sending message to client */
char string[50];
int bet = 0;
char *displayBuffer = malloc(MAXLINE);

snprintf(buff, sizeof(buff),"Welcome to Omars BlackJack game, do you want to play? y/n\n");
write(connfd, buff, strlen(buff));
z = read(connfd, responsebuffer, MAXLINE);
//responsebuffer[z] = '\0';


if (responsebuffer[0] == 'y')
initializeDeck();

if (responsebuffer[0] == 'n')
return 0;

//credit card number
snprintf(buff, sizeof(buff),"Please enter your credit card number:\n");
write(connfd, buff, strlen(buff));
z = read(connfd, responsebuffer, MAXLINE);
//responsebuffer[z] = '\0';

snprintf(buff, sizeof(buff),"How much money would you like to add to your balance?\n");
write(connfd, buff, strlen(buff));
z = read(connfd, responsebuffer, MAXLINE);
balance = atoi(responsebuffer);

//jump point to play again
play:

snprintf(buff, sizeof(buff),"Your balance is $%d \nHow much money do you want to bet?\n",balance);
write(connfd, buff, strlen(buff));
z = read(connfd, responsebuffer, MAXLINE);
bet = atoi(responsebuffer);

//checks to see if player has enough money, if not prompt to add more money 
if(bet > balance)
{
snprintf(buff, sizeof(buff),"You don't have enough money in your balance to play, add more to your balance?\n");
write(connfd, buff, strlen(buff));
z = read(connfd, responsebuffer, MAXLINE);

if (responsebuffer[0] == 'y')
{
balance = balance + atoi(responsebuffer);
initializeDeck();
goto play;
}

if (responsebuffer[0] == 'n')
{
snprintf(buff, sizeof(buff),"Thanks for playing!\n");
write(connfd, buff, strlen(buff));
return 0;
}
}


snprintf(buff, sizeof(buff),"Here is your current hand:\n");


//display cards
for(j=0;j<returnsize();j++)
{  
displayCard(playerDeck[j].value_char, playerDeck[j].suite_char,displayBuffer);
strcat(buff,displayBuffer);
//z = read(connfd, responsebuffer, MAXLINE);
//responsebuffer[z] = '\0';
}
write(connfd, buff, MAXLINE);


//---------------------------------------------keep drawing until player hits 21-------------------------------------
int position = 0;

for(i = 1; i < 5; i++)
{
snprintf(buff, sizeof(buff),"Would you like to redraw card %d?\n",i+1);
write(connfd, buff, strlen(buff));
z = read(connfd, responsebuffer, MAXLINE);
//responsebuffer[z] = '\0';


if (responsebuffer[0] == 'y')
{
Redraw(i);
}

if (responsebuffer[0] == 'n')
{
// do nothing
}

	}


//sort cards before displaying hand
Sort();

//checkHand(bet);
  
//display sorted hand
snprintf(buff, sizeof(buff),"Here is your final (sorted) hand: \n %s of %c \n %s of %c \n %s of %c \n %s of %c \n %s of %c \n%s\n Play again?\n",playerDeck[0].value_char, playerDeck[0].suite_char,playerDeck[1].value_char, playerDeck[1].suite_char,playerDeck[2].value_char, playerDeck[2].suite_char,playerDeck[3].value_char, playerDeck[3].suite_char,playerDeck[4].value_char, playerDeck[4].suite_char,checkHand(bet));
write(connfd, buff, strlen(buff));
read(connfd, responsebuffer, MAXLINE);
if(responsebuffer[0] == 'y')
{
initializeDeck();
goto play;
}



   }




//initialize the deck of 52 cards in order
void initializeDeck(){
int i,j,temp = 0;

            
         for ( j = 1; j < 5; j++)
         {
             for (i = 1; i < 14; i++)
            {
               Deck[temp].suite = j; 
	       Deck[temp].value = i;
              temp++;
            }
         }
		 		 		 
for ( i = 0; i < 52; i++)
		  {
		   if(Deck[i].suite == 1)
			Deck[i].suite_char = 'C';
		   if (Deck[i].suite == 2)
			Deck[i].suite_char = 'S';
		   if(Deck[i].suite == 3)
			Deck[i].suite_char = 'H';			
		   if(Deck[i].suite == 4)
			Deck[i].suite_char = 'D';			
		   if(Deck[i].value == 1)
			Deck[i].value_char = 'A';
		   if(Deck[i].value == 2)
			Deck[i].value_char = '2';
 		   if(Deck[i].value == 3)
			Deck[i].value_char = '3';
 	       if(Deck[i].value == 4)
			Deck[i].value_char = '4';
 		   if(Deck[i].value == 5)
			Deck[i].value_char = '5';
 		   if(Deck[i].value == 6)
			Deck[i].value_char = '6';
 		   if(Deck[i].value == 7)
			Deck[i].value_char = '7';
 		   if(Deck[i].value == 8)
			Deck[i].value_char = '8';
 		   if(Deck[i].value == 9)
			Deck[i].value_char = '9';
 		   if(Deck[i].value == 10)
			Deck[i].value_char = 'T';
		   if(Deck[i].value == 11)
			Deck[i].value_char = 'J';
		   if (Deck[i].value == 12)
			Deck[i].value_char = 'Q';
		   if(Deck[i].value == 13)
			Deck[i].value_char = 'K';			
		   
		 }
shuffleDeck();            		
}


//this function shuffles the entire deck of cards.  it generates 52 random, non repeating numbers from 0-51 and places them in an array
//called temp and then shuffled the order of the original deck
void shuffleDeck(){
//temp is an array of 52 integers that stores the numbers already swapped
int temp[52];

srand(time(NULL));
//variables needed to shuffle the deck
int i = 0;
int y = 0;
int flag = 0;

for (i = 0; i < 52; i++)
temp[i] = -1;

for (i = 0;i < 52; i++)
{
y = rand()%52;
flag = checkDuplicate(temp,y);
while(flag == 1)
   {
	y = rand()%52;
	flag = checkDuplicate (temp,y);
   }
temp[i] = y;

}

//now swap the entire deck with the random deck
struct Card swap;
int swap_location = -1;
for (i = 0; i < 52; i++)
{
swap_location = temp[i];
swap = Deck[i];
Deck[i] = Deck[swap_location];
Deck[swap_location] = swap;
}

initializePlayer();

}



//this function is used by the shuffle function, it checks for duplicates in the array of swapped numbers
int checkDuplicate(int temp2[], int n){
int i = 0;
for (i = 0 ; i < 52; i++)
{
if (temp2[i] == n)
return 1;
}
return 0;
}



//initializes the player array of cards, assigns cards 1-5 to the player from the shuffled deck
void initializePlayer(){
int i,p;

for (i=0;i<2;i++)
playerDeck[i] = Deck[i];

for (p=2; p<10; p++)
playerDeck[p].value = -1;
}



//this redraw functio n is very simple.  it takes a parameter that is the index of the player deck array to be changed
//it is replaced with 5 + i because cards 0-4 have already been assigned.  5 + 1  is a unique number
void Redraw(int i){
playerDeck[i] = Deck[5+i];
}


//this is a simple sorting function that sorts the cards in order from lowest to highest
Sort()
{
int i,p,q,swap;
 for(p = 0; p < 5; p++)
    for(q = 0; q < 4; q++)
      if(playerDeck[q].value > playerDeck[q+1].value) {
        swap = playerDeck[q+1].value;
        playerDeck[q+1].value = playerDeck[q].value;
        playerDeck[q].value = swap;
      }

for (i=0; i < 5; i++)
	{
 		   if(playerDeck[i].suite == 1)
			playerDeck[i].suite_char = 'C';
		   if (playerDeck[i].suite == 2)
			playerDeck[i].suite_char = 'S';
		   if(playerDeck[i].suite == 3)
			playerDeck[i].suite_char = 'H';			
		   if(playerDeck[i].suite == 4)
			playerDeck[i].suite_char = 'D';			  
		   if(playerDeck[i].value == 1)
			playerDeck[i].value_char = 'A';
		   if(playerDeck[i].value == 2)
			playerDeck[i].value_char = '2';
 		   if(playerDeck[i].value == 3)
			playerDeck[i].value_char = '3';
 	     	   if(playerDeck[i].value == 4)
			playerDeck[i].value_char = '4';
 		   if(playerDeck[i].value == 5)
			playerDeck[i].value_char = '5';
 		   if(playerDeck[i].value == 6)
			playerDeck[i].value_char = '6';
 		   if(playerDeck[i].value == 7)
			playerDeck[i].value_char = '7';
 		   if(playerDeck[i].value == 8)
			playerDeck[i].value_char = '8';
 		   if(playerDeck[i].value == 9)
			playerDeck[i].value_char = '9';
 		   if(playerDeck[i].value == 10)
			playerDeck[i].value_char = "10";
		   if(playerDeck[i].value == 11)
			playerDeck[i].value_char = 'J';
		   if (playerDeck[i].value == 12)
			playerDeck[i].value_char = 'Q';
		   if(playerDeck[i].value == 13)
			Deck[i].value_char = 'K';		

	}


}

//this function is used with the checkHand function to calculate the total sum

//this function checks the hand and then displays the final hand
char* checkHand(int bet){
char *hand;

hand = "";



//pair
if (playerDeck[0].value == playerDeck[1].value  || playerDeck[1].value == playerDeck[2].value || playerDeck[2].value == playerDeck[3].value || playerDeck[3].value == playerDeck[4].value)
{
hand = "You got a pair.";
balance = balance + (bet * 1);
}

//two pair
if((playerDeck[0].value == playerDeck[1].value && playerDeck[2].value == playerDeck[3].value) || (playerDeck[0].value == playerDeck[1].value && playerDeck[3].value == playerDeck[4].value) || (playerDeck[1].value == playerDeck[2].value && playerDeck[3].value == playerDeck[4].value))
{
hand = "You got two pair!";
balance = balance + (bet * 2);
}

//three of a kind
if((playerDeck[0].value == playerDeck[1].value && playerDeck[1].value == playerDeck[2].value) || (playerDeck[1].value == playerDeck[2].value && playerDeck[2].value == playerDeck[3].value)  || (playerDeck[2].value == playerDeck[3].value && playerDeck[3].value == playerDeck[4].value))
{
hand = "You got a triple!";
balance = balance + (bet * 3);
}

//straight
if(playerDeck[0].value+1 == playerDeck[1].value && playerDeck[1].value+1 == playerDeck[2].value && playerDeck[2].value+1 == playerDeck[3].value && playerDeck[3].value+1 == playerDeck[4].value)
{
hand = "You got a straight!";
balance = balance + (bet * 10);
}

//flush 
if (playerDeck[0].suite == playerDeck[1].suite && playerDeck[1].suite == playerDeck[2].suite && playerDeck[2].suite == playerDeck[3].suite && playerDeck[3].suite == playerDeck[4].suite)
{
hand = "You got a flush!";
balance = balance + (bet * 20);
}

//full house
if((playerDeck[0].value == playerDeck[1].value && playerDeck[1].value == playerDeck[2].value && playerDeck[3].value == playerDeck[4].value) || (playerDeck[0].value == playerDeck[1].value && playerDeck[2].value == playerDeck[3].value && playerDeck[3].value == playerDeck[4].value))
{
hand = "You got a full house!";
balance = balance = (bet * 30);
}

//four of a kind
if((playerDeck[0].value == playerDeck[1].value && playerDeck[1].value == playerDeck[2].value && playerDeck[2].value == playerDeck[3].value) || (playerDeck[1].value == playerDeck[2].value && playerDeck[2].value == playerDeck[3].value && playerDeck[3].value == playerDeck[4].value))
{
hand = "You got four of a kind!";
balance = balance + (bet * 40);
}

// straight flush
if((playerDeck[0].value+1 == playerDeck[1].value && playerDeck[1].value+1 == playerDeck[2].value && playerDeck[2].value+1 == playerDeck[3].value && playerDeck[3].value+1 == playerDeck[4].value) && (playerDeck[0].suite == playerDeck[1].suite && playerDeck[1].suite == playerDeck[2].suite && playerDeck[2].suite == playerDeck[3].suite && playerDeck[3].suite == playerDeck[4].suite))
{
hand = "You got a straight flush!";
balance = balance = (bet * 1000);
}

if(hand == "")
{
hand = "Sorry, you didn't get anything";
balance = balance - bet; 
}

return hand;



}

int returnsize()
{
int number = 1;
int i;
for (i=0; i < 10; i++)
{
if (playerDeck[i].value > -1)
number = number + 1;
}

return number;

}

void displayCard(char type, char pic, char* displayBuffer)
{
  
const char spade[]="\xe2\x99\xa0";
const char club[]="\xe2\x99\xa3";
const char heart[]="\xe2\x99\xa5";
const char diamond[]="\xe2\x99\xa6";

char blank;
blank = spade;

//printf("____________\n");
//printf("|%c         |\n",type);
//printf("|           |\n");
//printf("|           |\n");
//printf("|           |\n");
//printf("|     %s    |\n",blank);
//printf("|           |\n");
//printf("|           |\n");
//printf("|           |\n");
//printf("|         %c|\n",type);
//printf("|___________|\n");




snprintf(displayBuffer, MAXLINE,"____________\n|%c          |\n|           |\n|           |\n|           |\n|     %s     |\n|           |\n|           |\n|           |\n |          %c|\n _____________\n\0",type,spade,type);

  
}