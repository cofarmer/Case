


#include "stdafx.h"
#include "../include/protoinfo.h"
#include "../include/initsock.h"

#include <MSTcpIP.h>

#pragma comment(lib, "Advapi32.lib")

CInitSock theSock;


inline void parse_alpha_string(const char *src, int len)
{	
	if (!src || !len) return ;

	char *buf = (char *)malloc(len + 1);
	if (!buf)
	{
		return ;
	}
	memset(buf, '\0', len + 1);

	for (int i=0; i<len; i++)
	{
		if (*(src + i))
		{
			*(buf + i) = *(src + i);
		}
		else
		{
			*(buf + i) = '_';
		}
	}

	printf("%s \r\n", buf);

	free(buf);
}

void decode_tcp_packet(char *buf, unsigned short length)
{
	int data_length;
	unsigned short dst_port;
	unsigned short src_port;

	int data_offset = 0;
	struct tcp_header *tcp_hdr = (struct tcp_header *)buf;

	data_offset = (tcp_hdr->dataoffset >> 4) * sizeof(unsigned long);	
	data_length = length - data_offset;

	//FIN - 0x01
	//SYN - 0x02
	//RST - 0x04 
	//PUSH- 0x08
	//ACK- 0x10
	//URG- 0x20
	//ACE- 0x40
	//CWR- 0x80
	if (tcp_hdr->flags & TCP_FIN)
	{
		printf("FIN \r\n");
	}
	if (tcp_hdr->flags & TCP_SYN)
	{
		printf("SYN \r\n");
	}
	if (tcp_hdr->flags & TCP_RST)
	{
		printf("RST \r\n");
	}
	if (tcp_hdr->flags & TCP_PSH)
	{
		printf("PSH \r\n");
	}
	if (tcp_hdr->flags & TCP_ACK)
	{
		printf("ACK \r\n");
	}
	if (tcp_hdr->flags & TCP_URG)
	{
		printf("URG \r\n");
	}
	if (tcp_hdr->flags & TCP_ACE)
	{
		printf("ACE \r\n");
	}
	if (tcp_hdr->flags & TCP_CWR)
	{
		printf("CWR \r\n");
	}

	src_port = ntohs(tcp_hdr->sourcePort);
	dst_port = ntohs(tcp_hdr->destinationPort);
	switch(dst_port)
	{
	case 21:
		printf("ftp (21) \r\n");
		break;
	case 80:
	case 8080:
		printf("http (80/8080) \r\n");		
		break;
	case 443:
		printf("https (443) \r\n");
		break;
	case 110:
		printf("POP3 \r\n");
		break;
	}

	parse_alpha_string((buf + data_offset), data_length);
}

void decode_udp_packet(char *buf)
{
	struct udp_header *udp_hdr = (struct udp_header *)buf;
	unsigned short src_port = ntohs(udp_hdr->sourcePort);
	unsigned short dst_port = ntohs(udp_hdr->destinationPort);
	unsigned short chk_sum = ntohs(udp_hdr->checksum);

	char *itr = NULL;
	char *data = NULL;
	int data_len = ntohs(udp_hdr->len);

	printf("PORT: %u ===> %u \r\n", src_port, dst_port);

	switch(udp_hdr->destinationPort)
	{
	case 53:
		printf("name-domain server \r\n");
		return ;
	case 137:
		printf("NETBIOS Name service \r\n");
		return ;
	case 138:
		printf("NETBIOS Datagram service \r\n");
		return ;
	case 139:
		printf("NETBIOS session service \r\n");
		return ;
	}

	parse_alpha_string(buf, data_len);
}

void decode_icmp_packet(char *buf)
{
	struct icmp_header *icmp_hdr = (struct icmp_header *)buf;

}

void decode_igmp_packet(char *buf)
{
	struct igmp_header *igmp_hdr = (struct igmp_header *)buf;
}

void decode_ip_packet(char *buf)
{
	struct ip_header *ip_hdr = (struct ip_header *)buf;	
	in_addr source, dest;
	char src_ip[32], dst_ip[32];

	// 从IP头中取出源IP地址和目的IP地址
	//
	source.S_un.S_addr = ip_hdr->ipSource;
	dest.S_un.S_addr = ip_hdr->ipDestination;
	strcpy_s(src_ip, _countof(src_ip), inet_ntoa(source));
	strcpy_s(dst_ip, _countof(dst_ip), inet_ntoa(dest));

	printf("IP: %s ==>> %s \r\n", src_ip, dst_ip);

	int ip_header_len = (ip_hdr->iphVerLen & 0xf) * sizeof(unsigned long);
	int ip_header_version = (ip_hdr->iphVerLen & 0xf0);
	int ip_data_len = ntohs(ip_hdr->ipLength);

	switch(ip_hdr->ipProtocol)
	{
	case IPPROTO_TCP:
		printf("Protocol: TCP\r\n");
		decode_tcp_packet(buf + ip_header_len, ip_data_len);
		break;
	case IPPROTO_UDP:
		printf("Protocol: UDP\r\n");
		decode_udp_packet(buf + ip_header_len);
		break;
	case IPPROTO_ICMP:
		printf("Protocol: ICMP\r\n");
		decode_icmp_packet(buf + ip_header_len);
		break; 
	case IPPROTO_IGMP:
		printf("Protocol: IGMP\r\n");
		decode_igmp_packet(buf + ip_header_len);
		break;
	default:
		printf("Unimplelemted, Protocol id %d \r\n", ip_hdr->ipProtocol);
		break;
	}

	printf("\r\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	SOCKET raw_sock = INVALID_SOCKET;
	char host_name[56] = {0};
	struct hostent *host = NULL;
	sockaddr_in addr_in;
	unsigned short port = 8888;

	char buff[1024] = {0};
	int nRet;

	do 
	{
		// 创建原始套节字
		//
		raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
		if (raw_sock == INVALID_SOCKET)
		{
			printf("create socket failed, error code %d \r\n", WSAGetLastError());
			break;
		}

		gethostname(host_name, _countof(host_name));
		if((host = gethostbyname((char*)host_name)) == NULL)	
		{
			printf("gethostbyname failed, error code %d \r\n", WSAGetLastError());
			break;
		}

		// 在调用ioctl之前，套节字必须绑定
		//
		addr_in.sin_family  = AF_INET;
		addr_in.sin_port    = htons(port);
		addr_in.sin_addr.S_un.S_addr = inet_addr("10.0.0.13");

		printf("Binding to interface : %s \n", inet_ntoa(addr_in.sin_addr));
		if(bind(raw_sock, (struct sockaddr *)&addr_in, sizeof(addr_in)) == SOCKET_ERROR)
		{
			printf("bind failed, error code %d \r\n", WSAGetLastError());
			break;
		}

		// 设置SIO_RCVALL控制代码，以便接收所有的IP包	
		//
		DWORD dwValue = 1;
		if(ioctlsocket(raw_sock, SIO_RCVALL, &dwValue) != 0)	
		{
			printf("ioctlsocket failed, error code %d \r\n", WSAGetLastError());
			break;
		}

		// 开始接收封包
		//
		while(TRUE)
		{
			nRet = recv(raw_sock, buff, 1024, 0);
			if(nRet > 0)
			{
				decode_ip_packet(buff);
			}
		}

	} while (false);
	
	if (raw_sock != INVALID_SOCKET)
	{
		closesocket(raw_sock);
	}

	return 0;
}

