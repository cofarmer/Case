// ssl_synctime.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h> 
#include <string.h> 
#include <time.h> 
#include <winsock2.h> 
#include <openssl/ssl.h> 
#include <openssl/bio.h> 
#include <openssl/err.h> 

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "libeay32")
#pragma comment(lib, "ssleay32")

typedef struct _TimeInfo 
{ 
	time_t client;  /*客户端的时间*/ 
	time_t server;  /*服务器的时间*/ 
} TimeInfo; 

/** 
* 同步系统时间. 
*/ 
BOOL syncSystemTime(time_t t) 
{ 
	SYSTEMTIME st; 
	FILETIME   ft;   
	LONGLONG   ll;   

	ll = Int32x32To64(t, 10000000) + 116444736000000000; //1970.01.01   

	ft.dwLowDateTime  = (DWORD)ll;   
	ft.dwHighDateTime = (DWORD)(ll >> 32);   

	return FileTimeToSystemTime(&ft, &st) && SetSystemTime(&st); 
} 

/** 
* 获取SSL握手过程中服务器与客户端双方的系统时间. 
*/ 
void getSSLHandleShakeTimeInfo(int write_p, 
							   int version, 
							   int content_type, 
							   const void* vbuf, 
							   size_t len, 
							   SSL *ssl, 
							   void *vti) 
{ 
	unsigned char *buf = (unsigned char *)vbuf;
	TimeInfo *ti = (TimeInfo *)vti;

	if(content_type != 22)   //require handshake message 
		return; 
	if(len < 42) 
		return; 
	if(buf[0] == 1)          //ClientHello Message send from client to server 
		ti->client = htonl(*((u_long*)(buf + 6))); 
	else if(buf[0] == 2)     //ServerHello Message send from server to client 
		ti->server = htonl(*((u_long*)(buf + 6))); 
	else 
		return; 
} 

int main() 
{ 
	BIO * bio; 
	SSL * ssl; 
	SSL_CTX * ctx; 
	TimeInfo timeInfo = {-1, -1}; 
	BOOL timeSynced = FALSE; 
	long result; 

	/* Set up the library */ 
	SSL_library_init(); 
	ERR_load_BIO_strings(); 
	SSL_load_error_strings(); 

	/* Set up the SSL context */ 
	ctx = SSL_CTX_new(TLSv1_2_client_method()); 
	if(ctx == NULL) 
	{ 
		fprintf(stderr, "Error new SSL_CTX\n"); 
		ERR_print_errors_fp(stderr); 
		SSL_CTX_free(ctx); 
		return 0; 
	} 

	/* Get Server and Client system time via SSL Handshake */ 
	SSL_CTX_set_msg_callback(ctx, getSSLHandleShakeTimeInfo); 
	SSL_CTX_set_msg_callback_arg(ctx, &timeInfo); 

	/* Setup the connection */ 
	bio = BIO_new_ssl_connect(ctx); 

	/* Set the SSL_MODE_AUTO_RETRY flag */ 
	BIO_get_ssl(bio, &ssl); 
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY); 

	/* Create and setup the connection */ 
	BIO_set_conn_hostname(bio, "10.0.0.171:443"); 
	if(BIO_do_handshake(bio) <= 0) 
	{ 
		fprintf(stderr, "Error attempting to connect\n"); 
		ERR_print_errors_fp(stderr); 
		BIO_free_all(bio); 
		SSL_CTX_free(ctx); 
		return 0; 
	} 

	char send_buffer[4096] = {0};
	char recv_buffer[4096] = {0};
	int write_size = 0;
	int recv_size = 0;

	strcpy(send_buffer, "GET / HTTP/1.1\r\n");
	strcat(send_buffer, "Host: 10.0.0.171\r\n");
	strcat(send_buffer, "User-Agent: openssl_demo\r\n");
	strcat(send_buffer, "\r\n");

	write_size = SSL_write(ssl, send_buffer, strlen(send_buffer));
	printf("%d \r\n", write_size);
	recv_size = SSL_read(ssl, recv_buffer, 4095);
	printf("%s \r\n", recv_buffer);
	printf("%d \r\n", recv_size);
	
	/* Close the connection and free the context */ 
	BIO_free_all(bio); 
	SSL_CTX_free(ctx); 
	return 0; 
} 
