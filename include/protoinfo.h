//////////////////////////////////////////////////
// protoinfo.h文件

/*

定义协议格式
定义协议中使用的宏

 */


#ifndef __PROTOINFO_H__
#define __PROTOINFO_H__


#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_ARP   0x0806

struct et_header				// 14字节的以太头
{
	unsigned char	dhost[6];			// 目的MAC地址destination mac address
	unsigned char	shost[6];			// 源MAC地址source mac address
	unsigned short	type;				// 下层协议类型，如IP（ETHERTYPE_IP）、ARP（ETHERTYPE_ARP）等
};


#define ARPHRD_ETHER 	1

// ARP协议opcodes
//
#define	ARPOP_REQUEST	1		// ARP 请求	
#define	ARPOP_REPLY		2		// ARP 响应


struct arp_header		// 28字节的ARP头
{
	unsigned short	hrd;				//	硬件地址空间，以太网中为ARPHRD_ETHER
	unsigned short	eth_type;			//  以太网类型，ETHERTYPE_IP ？？
	unsigned char	maclen;				//	MAC地址的长度，为6
	unsigned char	iplen;				//	IP地址的长度，为4
	unsigned short	opcode;				//	操作代码，ARPOP_REQUEST为请求，ARPOP_REPLY为响应
	unsigned char	smac[6];			//	源MAC地址
	unsigned char	saddr[4];			//	源IP地址
	unsigned char	dmac[6];			//	目的MAC地址
	unsigned char	daddr[4];			//	目的IP地址
};


// 协议
//
#define PROTO_ICMP    1
#define PROTO_IGMP    2
#define PROTO_TCP     6
#define PROTO_UDP     17

// IP/TCP 版本
//
#define IPV_4		 0x40
#define IPV_6		 0x60

struct ip_header	// 20字节的IP头
{
    unsigned char     iphVerLen;      // 版本号和头长度（各占4位）
    unsigned char     ipTOS;          // 服务类型 
    unsigned short    ipLength;       // 封包总长度，即整个IP报的长度
    unsigned short    ipID;			  // 封包标识，惟一标识发送的每一个数据报
    unsigned short    ipFlags;	      // 标志
    unsigned char     ipTTL;	      // 生存时间，就是TTL
    unsigned char     ipProtocol;     // 协议，可能是TCP、UDP、ICMP等
    unsigned short    ipChecksum;     // 校验和
    unsigned long     ipSource;       // 源IP地址
    unsigned long     ipDestination;  // 目标IP地址
}; 


// 定义TCP标志
#define   TCP_FIN   0x01
#define   TCP_SYN   0x02
#define   TCP_RST   0x04
#define   TCP_PSH   0x08
#define   TCP_ACK   0x10
#define   TCP_URG   0x20
#define   TCP_ACE   0x40
#define   TCP_CWR   0x80

struct tcp_header		// 20字节的TCP头
{
	unsigned short	sourcePort;			// 16位源端口号
	unsigned short	destinationPort;	// 16位目的端口号
	unsigned long	sequenceNumber;		// 32位序列号
	unsigned long	acknowledgeNumber;	// 32位确认号
	unsigned char	dataoffset;			// 高4位表示数据偏移
	unsigned char	flags;				// 6位标志位
										//FIN - 0x01
										//SYN - 0x02
										//RST - 0x04 
										//PUSH- 0x08
										//ACK- 0x10
										//URG- 0x20
										//ACE- 0x40
										//CWR- 0x80

	unsigned short	windows;			// 16位窗口大小
	unsigned short	checksum;			// 16位校验和
	unsigned short	urgentPointer;		// 16位紧急数据偏移量 
};

struct udp_header
{
	unsigned short			sourcePort;		// 源端口号		
	unsigned short			destinationPort;// 目的端口号		
	unsigned short			len;			// 封包长度
	unsigned short			checksum;		// 校验和
};

struct icmp_header					// 12字节的ICMP头
{
	unsigned char   icmp_type;		// 消息类型
	unsigned char   icmp_code;		// 代码
	unsigned short  icmp_checksum;	// 校验和

	// 下面是回显头
	//
	unsigned short  icmp_id;		// 用来惟一标识此请求的ID号，通常设置为进程ID
	unsigned short  icmp_sequence;	// 序列号
	unsigned long   icmp_timestamp; // 时间戳
};

struct igmp_header							// 8字节的IGMP报文头
{
	unsigned char			ver_type;		// 版本和类型
	unsigned char			reserved;		// 保留
	unsigned short			checksum;		// 校验和
	unsigned long			address;		// 组地址(D类IP地址)
};

#endif // __PROTOINFO_H__

