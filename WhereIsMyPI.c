#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>


char *getIP(char * dev)
{
	struct ifreq ifr;
	int fdSock;
	
	fdSock = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want an IPv4 netmask */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want netmask attached to "eth0" */
	strncpy(ifr.ifr_name, dev, IFNAMSIZ-1);

	ioctl(fdSock, SIOCGIFADDR, &ifr);
 
	close(fdSock);
 
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

}
char *getMask(char *dev)
{
	struct ifreq ifr;
	int fdSock,i,m1,m2,m3,m4,bits=0;
	static char output[20];
	
	fdSock = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want an IPv4 netmask */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want netmask attached to "eth0" */
	strncpy(ifr.ifr_name, dev, IFNAMSIZ-1);

	ioctl(fdSock, SIOCGIFNETMASK, &ifr);
 
	close(fdSock);

	//convert netmask to CIDR notation
	sscanf(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),"%d.%d.%d.%d",&m1,&m2,&m3,&m4);
	for(i=0;i<8;i++)
		{
			if(m1 & 1)
				bits++;
			m1=m1>>1;
			if(m2 & 1)
				bits++;
			m2=m2>>1;
			if(m3 & 1)
				bits++;
			m3=m3>>1;
			if(m4 & 1)
				bits++;
			m4=m4>>1;
		}

	sprintf(output,"%s/%d",getIP(dev),bits);
	return output;
}
int main(int argc, char **argv)
{
	char devString[10];
	char comand[100];
	char line[20],pi[20];
	FILE *fd=NULL;
	int exist=0;
		
	if(argc!=2)
		{
			printf("\nUsage: %s [iface]\n",argv[0]);
			exit(EXIT_FAILURE);
		}
		
	if((fd=popen("cat /proc/net/dev","r"))==NULL)
		{
			printf("\nCan't open /proc/net/dev");
			exit(EXIT_FAILURE);
		}
		
	
	sprintf(devString," %s:",argv[1]);
	while(fgets(line,sizeof(line),fd)!=NULL)
		{
			//printf("\n%s",line);
			if(strstr(line, devString)!=NULL) //device exist
				exist=1;
				
		}
	fclose(fd);
		
	if(!exist)
		{
			printf("No such device\n");
			exit(EXIT_FAILURE);
		}

	system("ip  neigh flush all"); //flush arp cache
	
	

	sprintf(comand,"fping -a -r1 -g %s  > /dev/null 2>&1",getMask(argv[1]));
	//printf("\n%s\n",comand);
	system(comand);
	
	
	exist=0;
	fd=popen("arp -n | fgrep \" b8:27:eb\"","r");
	while(fgets(line, 200,fd)!=NULL)
		{
			exist=1;
			strncpy(pi,line,sizeof(line));
			pi[sizeof(line)]=0;
			printf("pi@%s\n",pi);
		}
	if(!exist)
		printf("Could'nt find PI in this network\n");
		
	fclose(fd);
	return 0;
}
