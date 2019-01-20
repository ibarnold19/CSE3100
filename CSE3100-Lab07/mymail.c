#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<errno.h>

#define DEFAULT_HOST "cse3100net.engr.uconn.edu"
#define DEFAULT_PORT 25
//#define DEFAULT_HOST "smtp.uconn.edu"
//#define DEFAULT_PORT 25

//Email struct is a handy way to organize your email fields, the fields are not dynamically sized so things will get truncated
typedef struct Email{
    char from[100];
    char to[100];
    char message[1000];

}Email;
//Payload struct just holds a buffer (you'll have to malloc it before use and free it afterwards) and the buffers size. For use with socket_read
typedef struct Payload{
    char* buf;
    int sz;

} Payload;
//Socket_send will just take a socket id and a message and keep trying to send until the whole message has sent.
int   socket_send(int sid, char* message);
//Socket read takes an initialized payload struct and will resize the buffer as necessary to hold however much information is read from the socket specified
int   socket_read(int sid, Payload *pp);
void  checkError();

int main(int argc, char*argv[]){
    /*First, connect to the server */
    int port;
    char* servername;
    servername = (argc > 1)? argv[1] : DEFAULT_HOST;
    port = (argc > 1)? atoi(argv[2]) : DEFAULT_PORT;
    struct hostent *server = gethostbyname(servername);

    if (server==NULL){
       perror("Error, no such host\n");
       exit(1);
    }
    //Connects to the server (either default or set by commmand line)
    struct sockaddr_in serv_addr;

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr,
          (char *) &serv_addr.sin_addr.s_addr,
          server->h_length);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd<0) exit(-1);

    serv_addr.sin_port = htons(port);
    int status = connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    checkError(status, __LINE__);
    printf("Connected to %s:%d successfully\n", servername, port);
    
    /* Get your machines fully qualified domain name, stored in fqdn variable*/
    struct addrinfo hints;
    struct addrinfo* info;
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    getaddrinfo(hostname ,NULL, &hints, &info);
    char* fqdn = info[0].ai_canonname;
    printf("%s\n", fqdn);
    
    //Now that the connection is made in sockfd (variable holding the sockets file descriptor), gather the information to send to the server, talk to the server to read it, and 
    //Solution goes here
    Email email;
    bzero((char*) &email.message,1000);
    printf("From:");
    scanf("%s", email.from);
    printf("To:");
    scanf("%s", email.to);
    printf("Message:\n\n");
    char line[100]="";
    while (strncmp(".\n", line,2)!=0){
        fgets(line, 100, stdin);
        strcat(email.message, line);
    }

    Payload pr;
    pr.buf = (char*) malloc(100);
    pr.sz = 100;
    char* buf = malloc(1000);
    socket_read(sockfd, &pr);
    
    sprintf(buf, "EHLO %s\n", "omar");//fqdn);
    socket_send(sockfd, buf);

    for(int i=0; i<9; i++){
        socket_read(sockfd, &pr);
    }
    
    sprintf(buf, "mail from: %s\n", email.from);
    socket_send(sockfd, buf);
    socket_read(sockfd, &pr);
    
    sprintf(buf, "rcpt to: %s\n", email.to);
    socket_send(sockfd, buf);
    socket_read(sockfd, &pr);
    
    socket_send(sockfd, "data\n");
    socket_read(sockfd, &pr);
    
    socket_send(sockfd, email.message);
    socket_read(sockfd, &pr);
    
    printf("%s\n", pr.buf);
    free(pr.buf);
    free(buf);
    //End Solution
    freeaddrinfo(info);
    return 0;
}
void checkError(int status, int line){
    if (status < 0){
        printf("socket error(%d)-%d: [%s]\n", getpid(), line, strerror(errno));
        exit(-1);
    }
}

int socket_send(int sid, char* message){
    int length = strlen(message);
    int sent=0, rem = length;
        while (sent!=length){
            int nbSent = write(sid, message+sent, rem);
            sent+=nbSent;
            rem-=nbSent;
        }
    return sent;
}
int  socket_read(int sid, Payload* pp) {
    Payload p = *pp;
    int ttl = 0, at= 0;
    int recvd;
    do { 
        recvd = read(sid, p.buf+at, 1);
        ttl += recvd;
        at += recvd;
        if (recvd > 0 && ttl == p.sz){
            p.buf = realloc(p.buf, p.sz*2);
            p.sz*=2;
        }
    } while (p.buf[ttl-1]!='\n');
    p.buf[ttl] = '\0';
    return ttl;
}  