#include <stdio.h>

#include <sys/poll.h>
#include <arpa/inet.h>

#define NETMAP_WITH_LIBS //开启netmap
#include <net/netmap_user.h>

#pragma pack(1) //单字节对齐

#define ETH_ADDR_LENGTH		6
#define PROTO_IP			0x0800
#define PROTO_ARP			0x0806
#define PROTO_UDP			17
#define PROTO_ICMP			1


struct ethhdr {
	unsigned char h_dst[ETH_ADDR_LENGTH];
	unsigned char h_src[ETH_ADDR_LENGTH];
	unsigned short h_proto;
	
}; // 14


struct iphdr {

	unsigned char hdrlen:4,
				  version:4; 
	unsigned char tos;
	unsigned short totlen;
	unsigned short id;
	unsigned short flag_offset; 
	unsigned char ttl; 
	unsigned char type;
	unsigned short check;
	unsigned int sip;
	unsigned int dip;

}; // 20



struct udphdr {

	unsigned short sport;
	unsigned short dport;

	unsigned short length; //udp总包的长度
	unsigned short check;

}; // 8


struct udppkt {

	struct ethhdr eh; // 14
	struct iphdr ip;  // 20 
	struct udphdr udp; // 8

	unsigned char data[0];

}; 


int main(){
	
	struct nm_desc *nmr = nm_open("netmap:enp0s31f6", NULL, 0, NULL);
	if(nmr == NULL){
		return -1;
	}

    struct pollfd pfd = {0};
	pfd.fd = nmr->fd;
	pfd.events = POLLIN;

	while(1){
		
		int ret = poll(&pfd, 1, -1);
		if(ret<0) continue;
		if(pfd.revents & POLLIN){

			struct nm_pkthdr h;
			unsigned char *stream = nm_nextpkt(nmr, &h);

			struct ethhdr *eh = (struct ethhdr *) stream;

			printf("eh->h_proto---> %x",ntohs(eh->h_proto));
			
			if(ntohs(eh->h_proto = PROTO_IP)){
				
				struct udppkt *udp = (struct udppkt *)stream;
				
				if(udp->ip.type == PROTO_UDP){
					int udplength = ntohs(udp->udp.length);

					udp->data[udplength-8] = '\0';

					printf("udp --> %s\n", udp->data);
					
				}
			}
		}

	}

	return 0;
}