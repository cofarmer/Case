//////////////////////////////////////////////////
// comm.h�ļ�

// ����һЩ��������



#ifndef __COMM_H__
#define __COMM_H__


// У��͵ļ���
// ��16λ����Ϊ��λ����������������ӣ��������������Ϊ������
// ���ټ���һ���ֽڡ����ǵĺʹ���һ��32λ��˫����
//
unsigned short checksum(USHORT* buff, int size);

bool set_ttl(SOCKET s, int nValue);
bool set_timeout(SOCKET s, int nTime, BOOL bRecv = TRUE);


#endif // __COMM_H__