#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

#define BUFSZ 200
int port=6180;
typedef struct data
{
	int sock;
	char buf[200];
	int lens;
}data;

int readSocket(data *get)
{
   int rc;
      do
      {
	      rc=read(get->sock,get->buf,get->lens);
	      if(rc==-1)
	      {
		      printf("read:%s\n",strerror(errno));
		      pthread_exit(NULL);
	      }
	      else if (rc==0) 
	      {
		      printf("connection closed\n");
		     pthread_exit(NULL);

	      }
	      else printf("received %d bytes:%.*s\n",rc,rc,get->buf);
	}while(rc>0);
    return rc;
}
int writeSocket(data *output)
{
	int rc;
	      while(1)
	      {
		      gets(output->buf);
		      output->buf[strlen(output->buf)]='\n';
		      //sprintf(output->buf,"%s\n",output->buf);
	      if((rc=write(output->sock,output->buf,strlen(output->buf)))!=strlen(output->buf))
	      {
		      printf("write:%s",rc<0?strerror(errno):"incomplete\n");
		      pthread_exit(NULL);
	      }
	      }
  return rc;
}
int main(int argc,char *argv[])
{
	char buf[BUFSZ];;
	int rc;
	int childPid;
	pthread_t id1,id2;

	int fd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==fd)
	{
		printf("socket:%s\n",strerror(errno));
		exit(-1);
	}

	struct sockaddr_in sin;
	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr=htonl(INADDR_ANY);
	sin.sin_port=htons(port);

	if(bind(fd,(struct sockaddr*)&sin,sizeof(sin))==-1)
	{
		printf("bind %s\n",strerror(errno));
		exit(-1);
		
	}
	if(listen(fd,1)==-1)
	{
		printf("listen:%s\n",strerror(errno));
		exit(-1);
	}
while(1)
{
	struct sockaddr_in cin;
	data dataRead,dataWrite;
	socklen_t cin_sz=sizeof(cin);

	int fa=accept(fd,(struct sockaddr*)&cin,&cin_sz);
	if(fa==-1)
	{
		printf("accept%s\n",strerror(errno));
		continue;
	}
	if(sizeof(cin)==cin_sz)
		printf("connect from %s:%d\n",inet_ntoa(cin.sin_addr),(int)ntohs(cin.sin_port));
	memset(&dataRead,0,sizeof(dataRead));
	memset(&dataWrite,0,sizeof(dataWrite));	
	dataRead.sock=fa;
	dataRead.lens=200;
	dataWrite.sock=fa;
	pthread_create(&id1,NULL,(void*)readSocket,&dataRead);
        pthread_create(&id2,NULL,(void*)writeSocket,&dataWrite);
  } 

return 0;
}
