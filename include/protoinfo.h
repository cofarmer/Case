//////////////////////////////////////////////////
// protoinfo.h�ļ�

/*

����Э���ʽ
����Э����ʹ�õĺ�

 */


#ifndef __PROTOINFO_H__
#define __PROTOINFO_H__


#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_ARP   0x0806

struct et_header				// 14�ֽڵ���̫ͷ
{
	unsigned char	dhost[6];			// Ŀ��MAC��ַdestination mac address
	unsigned char	shost[6];			// ԴMAC��ַsource mac address
	unsigned short	type;				// �²�Э�����ͣ���IP��ETHERTYPE_IP����ARP��ETHERTYPE_ARP����
};


#define ARPHRD_ETHER 	1

// ARPЭ��opcodes
//
#define	ARPOP_REQUEST	1		// ARP ����	
#define	ARPOP_REPLY		2		// ARP ��Ӧ


struct arp_header		// 28�ֽڵ�ARPͷ
{
	unsigned short	hrd;				//	Ӳ����ַ�ռ䣬��̫����ΪARPHRD_ETHER
	unsigned short	eth_type;			//  ��̫�����ͣ�ETHERTYPE_IP ����
	unsigned char	maclen;				//	MAC��ַ�ĳ��ȣ�Ϊ6
	unsigned char	iplen;				//	IP��ַ�ĳ��ȣ�Ϊ4
	unsigned short	opcode;				//	�������룬ARPOP_REQUESTΪ����ARPOP_REPLYΪ��Ӧ
	unsigned char	smac[6];			//	ԴMAC��ַ
	unsigned char	saddr[4];			//	ԴIP��ַ
	unsigned char	dmac[6];			//	Ŀ��MAC��ַ
	unsigned char	daddr[4];			//	Ŀ��IP��ַ
};


// Э��
//
#define PROTO_ICMP    1
#define PROTO_IGMP    2
#define PROTO_TCP     6
#define PROTO_UDP     17

// IP/TCP �汾
//
#define IPV_4		 0x40
#define IPV_6		 0x60

struct ip_header	// 20�ֽڵ�IPͷ
{
    unsigned char     iphVerLen;      // �汾�ź�ͷ���ȣ���ռ4λ��
    unsigned char     ipTOS;          // �������� 
    unsigned short    ipLength;       // ����ܳ��ȣ�������IP���ĳ���
    unsigned short    ipID;			  // �����ʶ��Ωһ��ʶ���͵�ÿһ�����ݱ�
    unsigned short    ipFlags;	      // ��־
    unsigned char     ipTTL;	      // ����ʱ�䣬����TTL
    unsigned char     ipProtocol;     // Э�飬������TCP��UDP��ICMP��
    unsigned short    ipChecksum;     // У���
    unsigned long     ipSource;       // ԴIP��ַ
    unsigned long     ipDestination;  // Ŀ��IP��ַ
}; 


// ����TCP��־
#define   TCP_FIN   0x01
#define   TCP_SYN   0x02
#define   TCP_RST   0x04
#define   TCP_PSH   0x08
#define   TCP_ACK   0x10
#define   TCP_URG   0x20
#define   TCP_ACE   0x40
#define   TCP_CWR   0x80

struct tcp_header		// 20�ֽڵ�TCPͷ
{
	unsigned short	sourcePort;			// 16λԴ�˿ں�
	unsigned short	destinationPort;	// 16λĿ�Ķ˿ں�
	unsigned long	sequenceNumber;		// 32λ���к�
	unsigned long	acknowledgeNumber;	// 32λȷ�Ϻ�
	unsigned char	dataoffset;			// ��4λ��ʾ����ƫ��
	unsigned char	flags;				// 6λ��־λ
										//FIN - 0x01
										//SYN - 0x02
										//RST - 0x04 
										//PUSH- 0x08
										//ACK- 0x10
										//URG- 0x20
										//ACE- 0x40
										//CWR- 0x80

	unsigned short	windows;			// 16λ���ڴ�С
	unsigned short	checksum;			// 16λУ���
	unsigned short	urgentPointer;		// 16λ��������ƫ���� 
};

struct udp_header
{
	unsigned short			sourcePort;		// Դ�˿ں�		
	unsigned short			destinationPort;// Ŀ�Ķ˿ں�		
	unsigned short			len;			// �������
	unsigned short			checksum;		// У���
};

struct icmp_header					// 12�ֽڵ�ICMPͷ
{
	unsigned char   icmp_type;		// ��Ϣ����
	unsigned char   icmp_code;		// ����
	unsigned short  icmp_checksum;	// У���

	// �����ǻ���ͷ
	//
	unsigned short  icmp_id;		// ����Ωһ��ʶ�������ID�ţ�ͨ������Ϊ����ID
	unsigned short  icmp_sequence;	// ���к�
	unsigned long   icmp_timestamp; // ʱ���
};

struct igmp_header							// 8�ֽڵ�IGMP����ͷ
{
	unsigned char			ver_type;		// �汾������
	unsigned char			reserved;		// ����
	unsigned short			checksum;		// У���
	unsigned long			address;		// ���ַ(D��IP��ַ)
};

#endif // __PROTOINFO_H__

