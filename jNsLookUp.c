
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>




struct Header{
    unsigned short ID;
    unsigned char rd:1;
    unsigned char tc:1;
    unsigned char aa:1;
    unsigned char opCode:4;
    unsigned char qr:1;
    
    unsigned char rcode:4;
    unsigned char z:3;
    unsigned char ra:1;

    unsigned short qdCount;
    unsigned short anCount;
    unsigned short nsCount;
    unsigned short arCount;
};


struct QuestionName{
    unsigned char *qName;
//    struct Questions que;
    
};

struct Questions{
    
    unsigned short qType;
    unsigned short qClass;
   
};

struct AwnserName{
    unsigned char *aName;
};

struct Awnser{
    unsigned short type;
    unsigned short class;
    unsigned int TTL;
    unsigned short rDataPref;
    unsigned int rDataExch;
    
};


struct Authority{
    unsigned short author;
};
struct Additional{
    unsigned short addit;
};




void questionName(unsigned char *name);
void dnsServIp(char *dnsIp);
void buildPacket(unsigned char *packet,char *name);



int main(int argc, char **argv){
    int argvLen = strlen(argv[1]);
    unsigned char *name = argv[1];
    
    char dns[15];
    unsigned char pack[1024];
    memset(pack,0,1024);
    questionName(name);
    
    
    dnsServIp(dns);
    
    buildPacket(pack,name);

    
    
    int i;
    for(i = 0;i<39;i++)
        printf("%x,",pack[i]);
    

    	  
    int sockfd;
  unsigned char recvd[1024];
	struct sockaddr_in servaddr;
    int portNum=53;

    
    sockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
		    
    
    if(sockfd == -1)
        printf("\nerror in making a sock\n");
    else
        printf("\nmade a sock: %d\n",sockfd);
    
	bzero((char *)&servaddr,sizeof(servaddr));
    
    
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(portNum);
    
	servaddr.sin_addr.s_addr = inet_addr(dns);
    inet_pton(AF_INET,dns,&(servaddr.sin_addr));
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));


    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))== -1)
    printf("Problems connecting to DNS server.\n");
    else
        printf("Connected to DNS server.\n");

		
        int   n = write(sockfd,pack,39);


        if (n < 0)
            printf("ERROR writing to socket: %d\n",n);
        else
            printf("Writing to socket: %d\n",n);
    
     int size=read(sockfd,recvd,1024);
    if (size < 0)
        printf("ERROR receiveing fromsocket: %d\n",size);
    else
         printf("Receiveing fromsocket: %d\n",size);
    
    int o;
    for(o = 0;o<size;o++)
        if(recvd[o]==4){
            printf("Name: www.ncc.edu\n");
            printf("Address: %d.%d.%d.%d\n",recvd[o+1],recvd[o+2],recvd[o+3],recvd[o+4]);
            break;
        }
        
    
   
    close(sockfd);
    return 0;

}


void questionName(unsigned char *qname){
    char destination[255];
    memset(destination,0,255);
    
    char tempChar[63];
    memset(tempChar,0,63);
    
    
    int qSize=0;
    char temp;
    
    char charCtr[4];
    char qCtr = 0;
    
    int i;
    int len;
    
    qSize= strlen(qname);

    
    for(i=0;i<qSize;i++){
        temp = qname[i];
        if(temp == '.'){
           
            sprintf(charCtr,"%c",qCtr);
            strcat(destination,charCtr);
            strcat(destination,tempChar);
            bzero(tempChar,qCtr);
            bzero(charCtr,4);
            qCtr= 0;
        }else{
            len = strlen(tempChar);
            tempChar[len]=temp;
            qCtr++;
        }
        
        
    }
    sprintf(charCtr,"%c",qCtr);
    strcat(destination,charCtr);
    strcat(destination,tempChar);
    strcpy(qname,destination);
}

void dnsServIp(char *dnsIp){
    
    FILE *fp;
    
    char filename[] = "//etc//resolv.conf";
    
    fp = fopen(filename,"r");
    char ip[50];
    memset(ip,0,50);
    
    
    if(fp != NULL){
        char line[50];
        memset(ip,0,50);
        while(fgets(line,sizeof line, fp) != NULL)
            if(line[0] == 'n')
            {
                int i;
                int size = strlen(line);
                for(i = 0; i < size;i++){
                    if(line[i+11] !='\0')
                        ip[i]= line[i+11];
                }
                
                
            }
        fclose(fp);
        
        
    }
    
    strcpy(dnsIp,ip);
    
}

void buildPacket(unsigned char *packet,char *name){
    
    struct Header head;
    struct QuestionName quName;
    struct Questions ques;
 
    head.ID = htons(1);
    
    head.rd = 1;
    head.tc=0;
    head.aa=0;
    head.opCode=0;
    head.qr=0;
    
    head.rcode=0;
    head.z=0;
    head.ra=0;
    
    head.qdCount=htons(1);
    head.anCount=0;
    head.nsCount=0;
    head.arCount=0;
    
    quName.qName= name;
    ques.qType = htons(1);
    ques.qClass =htons(1);

    int sizeQName = strlen(quName.qName);
    printf("sizeQName: %d\n",sizeQName);

    memcpy(packet,(unsigned char*)&head,sizeof(head));
    memcpy(packet+sizeof(head),quName.qName,sizeQName+1);
    memcpy(packet+sizeof(head)+sizeQName+1,(unsigned char*)&ques,sizeof(ques));
    

     int len =sizeof(head)+sizeQName+1+sizeof(ques);
    packet[len] = '\0';

    printf("len: %d\n",len);
    
}
