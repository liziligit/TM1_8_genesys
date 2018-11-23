#ifndef __TCP__
#define __TCP__



/******************************************************************************/
#define Sleep(x) (usleep((x)*5000))
/******************************************************************************/

#define MAXSLEEP 2


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

 using namespace std;
///////////////////////////
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>

#ifdef __linux /* on linux */
#include <pty.h>
#include <utmp.h>
#else /* (__APPLE__ & __MACH__) */
//#include <util.h> /* this is for mac or bsd */
#endif

#include <paths.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>



//#include <hdf5.h>

/* utilities */
#define bitsof(x) (8*sizeof(x))

#ifdef DEBUG
  #define debug_printf(fmt, ...) do { fprintf(stderr, fmt, ##__VA_ARGS__); fflush(stderr); \
                                    } while (0)
#else
  #define debug_printf(...) ((void)0)
#endif
#define error_printf(fmt, ...) do { fprintf(stderr, fmt, ##__VA_ARGS__); fflush(stderr); \
                                  } while(0)

#define MIN(x,y) (((x)>(y))?(y):(x))
#define MAX(x,y) (((x)<(y))?(y):(x))

#ifndef strlcpy
#define strlcpy(a, b, c) do {                   \
        strncpy(a, b, (c)-1);                   \
        (a)[(c)-1] = '\0';                      \
    } while (0)
#endif



#define NAME_BUF_SIZE 256




int debug;





char *conv16network_endian(uint16_t *buf, size_t n)
{
    size_t i;
    for(i=0; i<n; i++) {
        buf[i] = htons(buf[i]);
    }
    return (char*)buf;
}

char *conv32network_endian(uint32_t *buf, size_t n)
{
    size_t i;
    for(i=0; i<n; i++) {
        buf[i] = htonl(buf[i]);
    }
    return (char*)buf;
}

size_t cmd_read_status(uint32_t **bufio, uint32_t addr)
{
    uint32_t *buf;
    buf = *bufio;
    if(buf == NULL) {
        buf = (uint32_t*)calloc(1, sizeof(uint32_t));
        if(buf == NULL)
            return -1;
        *bufio = buf;
    }
    buf[0] = (0xffff0000 & ((0x8000 + addr) << 16));
    conv32network_endian(buf, 1);
    return 1*sizeof(uint32_t);
}

size_t cmd_send_pulse(uint32_t **bufio, uint32_t mask)
{
    uint32_t *buf;
    buf = *bufio;
    if(buf == NULL) {
        buf = (uint32_t*)calloc(1, sizeof(uint32_t));
        if(buf == NULL)
            return -1;
        *bufio = buf;
    }
    buf[0] = 0x000b0000 | (0x0000ffff & mask);
    conv32network_endian(buf, 1);
    return 1*sizeof(uint32_t);
}

size_t cmd_write_memory(uint32_t **bufio, uint32_t addr, uint32_t *aval, size_t nval)
{
    size_t idx, i;
    uint32_t *buf;

    buf = *bufio;
    if(buf == NULL) {
        buf = (uint32_t*)calloc(nval*2+2, sizeof(uint32_t));
        if(buf == NULL)
            return -1;
        *bufio = buf;
    }

    idx = 0;
    buf[idx++] = 0x00110000 | (0x0000ffff & addr);          // address LSB
    buf[idx++] = 0x00120000 | (0x0000ffff & (addr>>16));    // address MSB
    buf[idx++] = 0x00130000 | (0x0000ffff & (*aval));       // data LSB
    buf[idx++] = 0x00140000 | (0x0000ffff & ((*aval)>>16)); // data MSB
    for(i=1; i<nval; i++) {                                 // more data
        buf[idx++] = 0x00130000 | (0x0000ffff & aval[i]);
        buf[idx++] = 0x00140000 | (0x0000ffff & (aval[i]>>16));
    }
    conv32network_endian(buf, nval*2+2);
    return (nval*2+2)*sizeof(uint32_t);
}

size_t cmd_read_memory(uint32_t **bufio, uint32_t addr, uint32_t n)
{
    size_t idx;
    uint32_t *buf;

    buf = *bufio;
    if(buf == NULL) {
        buf = (uint32_t*)calloc(4, sizeof(uint32_t));
        if(buf == NULL)
            return -1;
        *bufio = buf;
    }

    idx = 0;
    buf[idx++] = 0x00110000 | (0x0000ffff & addr);       // address LSB
    buf[idx++] = 0x00120000 | (0x0000ffff & (addr>>16)); // address MSB
    buf[idx++] = 0x00100000 | (0x0000ffff & n);          // n words to read
    buf[idx++] = 0x80140000;                             // initialize read

    conv32network_endian(buf, 4);
    return 4*sizeof(uint32_t);
}

size_t cmd_write_register(uint32_t **bufio, uint32_t addr, uint32_t val)
{
    uint32_t *buf;
    buf = *bufio;
    if(buf == NULL) {
        buf = (uint32_t*)calloc(1, sizeof(uint32_t));
        if(buf == NULL)
            return -1;
        *bufio = buf;
    }
    buf[0] = (0xffff0000 & ((0x0020 + addr) << 16)) | (0x0000ffff & val);
    conv32network_endian(buf, 1);
    return 1*sizeof(uint32_t);
}

size_t cmd_read_register(uint32_t **bufio, uint32_t addr)
{
    uint32_t *buf;
    buf = *bufio;
    if(buf == NULL) {
        buf = (uint32_t*)calloc(1, sizeof(uint32_t));
        if(buf == NULL)
            return -1;
        *bufio = buf;
    }
    buf[0] = (0xffff0000 & ((0x8020 + addr) << 16));
    conv32network_endian(buf, 1);
    return 1*sizeof(uint32_t);
}

size_t cmd_read_datafifo(uint32_t **bufio, uint32_t n)
{
    uint32_t *buf;
    buf = *bufio;
    if(buf == NULL) {
        buf = (uint32_t*)calloc(1, sizeof(uint32_t));
        if(buf == NULL)
            return -1;
        *bufio = buf;
    }
    buf[0] = (0xffff0000 & (0x0019 << 16)) | (0x0000ffff & n);
    conv32network_endian(buf, 1);
    return 1*sizeof(uint32_t);
}
	

static int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen)
{
    int nsec;
    /* Try to connect with exponential backoff. */
    for (nsec = 1; nsec <= MAXSLEEP; nsec <<= 1) {
        if (connect(sockfd, addr, alen) == 0) {
            /* Connection accepted. */
            return(0);
        }
        /*Delay before trying again. */
        if (nsec <= MAXSLEEP/2)
            sleep(nsec);
    }
    return(-1);
}



static int get_socket(char *host, char *port)
{
    int status;
    struct addrinfo addrHint, *addrList, *ap;
    int sockfd, sockopt;

    memset(&addrHint, 0, sizeof(struct addrinfo));
    addrHint.ai_flags = AI_CANONNAME|AI_NUMERICSERV;
    addrHint.ai_family = AF_INET; /* we deal with IPv4 only, for now */
    addrHint.ai_socktype = SOCK_STREAM;
    addrHint.ai_protocol = 0;
    addrHint.ai_addrlen = 0;
    addrHint.ai_canonname = NULL;
    addrHint.ai_addr = NULL;
    addrHint.ai_next = NULL;

    status = getaddrinfo(host, port, &addrHint, &addrList);
    if(status < 0) {
        error_printf("getaddrinfo: %s\n", gai_strerror(status));
        return status;
    }

    for(ap=addrList; ap!=NULL; ap=ap->ai_next) {
        sockfd = socket(ap->ai_family, ap->ai_socktype, ap->ai_protocol);
        if(sockfd < 0) continue;
        sockopt = 1;
        if(setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&sockopt, sizeof(sockopt)) == -1) {
            close(sockfd);
            warn("setsockopt TCP_NODELAY");
            continue;
        }
        sockopt = 1;
        if(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char*)&sockopt, sizeof(sockopt)) == -1) {
            close(sockfd);
            warn("setsockopt SO_KEEPALIVE");
            continue;
        }
        if(connect_retry(sockfd, ap->ai_addr, ap->ai_addrlen) < 0) {
            close(sockfd);
            warn("connect");
            continue;
        } else {
            break; /* success */
        }
    }
    if(ap == NULL) { /* No address succeeded */
        error_printf("Could not connect, tried %s:%s\n", host, port);
        return -1;
    }
    freeaddrinfo(addrList);
    return sockfd;
}
	


static int query_response_with_timeout(int sockfd, char *queryStr, size_t nbytes, char *respStr,
                                       ssize_t nbytes_ret_exp, struct timeval *tv)
{
    int maxfd;
    fd_set rfd;
    int nsel;
    ssize_t nr, nw;
    size_t ret;
	int i;
	
	
    nw = send(sockfd, queryStr, nbytes, 0);
	
    if(debug>4){
     printf("send print \n");		
	for(i=0;i<nbytes;i++){
		printf("%x ",(queryStr[i]));
	}
		printf("\n");
	}
	
    if(nw<0) {
        warn("send");
        return (int)nw;
    }
    if(nbytes_ret_exp == 0) return nw;

    ret = 0;
    for(;;) {
        FD_ZERO(&rfd);
        FD_SET(sockfd, &rfd);
        maxfd = sockfd;
        nsel = select(maxfd+1, &rfd, NULL, NULL, tv);
        if(nsel < 0 && errno != EINTR) { /* other errors */
            return nsel;
        }
        if(nsel == 0) { /* timed out */
            warn("select");
            break;
        }
        if(nsel>0 && FD_ISSET(sockfd, &rfd)) {
            nr = read(sockfd, respStr+ret, BUFSIZ-ret);
            // debug_printf("nr = %zd\n", nr);
            if(nr>0) {
                ret += nr;
                if(ret >= nbytes_ret_exp && nbytes_ret_exp > 0) break;
            } else {
                break;
            }
        }
    }
	
    if(debug>5){
		printf("receive print \n");
	for(i=0;i<nbytes_ret_exp;i++){
		printf("%x ",(respStr[i]));
	}
		printf("\n");
	}	
	
    return (int)ret;
}


static int query_response(int sockfd, char *queryStr, size_t nbytes,
                          char *respStr, ssize_t nbytes_ret_exp)
{
    timeval tv;
	tv.tv_sec=0;
	tv.tv_usec=500000;
	
	
    return query_response_with_timeout(sockfd, queryStr, nbytes, respStr, nbytes_ret_exp, &tv);
}




int configSocket;
class tcp{
public:




string IP;
string port;

char *scopeAddress;
char *scopePort;
int sockfd;

int setup(){
	scopeAddress=(char*)IP.c_str();
	scopePort=(char*)port.c_str();
    sockfd = get_socket(scopeAddress, scopePort);
    if(sockfd < 0) {
        error_printf("Failed to establish a socket.\n");
        return EXIT_FAILURE;
    }	
	configSocket=sockfd;
}

int sendBuf(char * queryStr, int nbytes,int print=0){
	
	
    if(print>0){
	for(int i=0;i<nbytes;i++){
		// cout<<i<<endl;
		printf("%x ",(queryStr[i]));

		
	}
		printf("\n");
	}	
	
    // int nw = send(sockfd, queryStr, nbytes, 0);
	


	return 1;
}

int sendWord(unsigned int v,int print=0){
	// cout<<"hhhhhhhhhh  "<<print<<endl;
	unsigned int a=v;
	return sendBuf((char*)(&a),4,print);
}

	
};



//hdf5支持，后向兼容考虑。加载上面的库可直接将梅元的程序写到脚本里。


int help=0;
int pid;
int gargc;char** gargv;int para(char *a);int para(char *a){if(help==1)cout<<a<<" ";int i;for(i=0;i<gargc;i++){if(strcmp(a,gargv[i])==0) return i;}return -1;}

 uint32_t *buf32;    //全局变量，用来存储发送给fpga tcp端口的数据的指针
 char buf[BUFSIZ];   //全局变量，用来存储发送给fpga tcp端口的数据

 //写fpga内部配置寄存器 a为寄存器地址 val为寄存器的值
 void config_reg(int a,unsigned int val)	{
	 int n;
 buf32 = (uint32_t*)buf;	
     n = cmd_write_register(&buf32, a, val);
    n = query_response(configSocket, buf, n, buf, 0);
}

 //读fpga内部状态寄存器 a为寄存器地址 读回的值在buf32里面
 void status_reg(int a)	{
	 int n;
 buf32 = (uint32_t*)buf;	
     n = cmd_read_status(&buf32, a);
    n = query_response(configSocket, buf, n, buf, 4);
}

//可操作所有寄存器 a为寄存器地址 val为寄存器的值
 void reg(int a,unsigned int val)	{
	 int n;
 buf32 = (uint32_t*)buf;	

    buf32[0] = (0xffff0000 & ((0x0000 + a) << 16)) | (0x0000ffff & val);
    conv32network_endian(buf32, 1);
    n=1*sizeof(uint32_t);	 
    n = query_response(configSocket, buf, n, buf, 0);
}
//脉冲寄存器，在脉冲寄存器的指定位上产生脉冲 a为指定的位数
 void pulse_reg(int a)	{
	 int n;
 buf32 = (uint32_t*)buf;	
        n = cmd_send_pulse(&buf32, (1<<a) & 0xffff);        
        n = query_response(configSocket, buf, n, buf, 0);                 
}

 //写fpga内部配置寄存器 a为寄存器地址 val为寄存器的值，可指定socket，用于与多个tcp端口通信
 void config_reg(int sockfd,int a,unsigned int val)	{
	 int n;
 buf32 = (uint32_t*)buf;	
     n = cmd_write_register(&buf32, a, val);
    n = query_response(sockfd, buf, n, buf, 0);
}

 //读fpga内部状态寄存器 a为寄存器地址 读回的值在buf32里面 可指定socket，用于与多个tcp端口通信
 void status_reg(int sockfd,int a)	{
	 int n;
 buf32 = (uint32_t*)buf;	
     n = cmd_read_status(&buf32, a);
    n = query_response(sockfd, buf, n, buf, 4);
}
//可操作所有寄存器 a为寄存器地址 val为寄存器的值  可指定socket，用于与多个tcp端口通信
 void reg(int sockfd, int a,unsigned int val)	{
	 int n;
 buf32 = (uint32_t*)buf;	

    buf32[0] = (0xffff0000 & ((0x0000 + a) << 16)) | (0x0000ffff & val);
    conv32network_endian(buf32, 1);
    n=1*sizeof(uint32_t);	 
    n = query_response(sockfd, buf, n, buf, 0);
}
//脉冲寄存器，在脉冲寄存器的指定位上产生脉冲 a为指定的位数  可指定socket，用于与多个tcp端口通信
 void pulse(int sockfd,int a)	{
	 int n;
 buf32 = (uint32_t*)buf;	
        n = cmd_send_pulse(&buf32, (1<<a) & 0xffff);        
        n = query_response(sockfd, buf, n, buf, 0);                 
}


//告诉fpga要读取的数据量 sockfd为连接号， nbytes为字节数
void askTcpData(int sockfd, int nbytes){

	 int n;
 buf32 = (uint32_t*)buf;	
    int a=26;
	nbytes/=4;
	int val=(nbytes>>16);
    buf32[0] = (0xffff0000 & ((0x0000 + a) << 16)) | (0x0000ffff & val);
    conv32network_endian(buf32, 1);
    n=1*sizeof(uint32_t);	 
    n = query_response(sockfd, buf, n, buf, 0);	
	
    a=25;
	val=(0x0000ffff & nbytes);
    buf32[0] = (0xffff0000 & ((0x0000 + a) << 16)) | (0x0000ffff & val);
    conv32network_endian(buf32, 1);
    n=1*sizeof(uint32_t);	 
    n = query_response(sockfd, buf, n, buf, 0);	
	
}
//告诉fpga要读取的数据量 用全局链接号 configSocket， nbytes为字节数
void askTcpData(int nbytes){
	askTcpData(configSocket, nbytes);
}


//从fpag读取数据， nbytes 为数据量，读到ibuf缓冲区，只需 askTcpData 一次
int read_mem_ns(int sockfd, char *ibuf,int nbytes){
	

	
    struct timeval tv;		
    fd_set rfd;
    ssize_t readTotal;	
    int maxfd, nsel;
    int n;
	readTotal = 0;
	int run=0;
	
	int    nb = 0;
    // while(nb < SCOPE_NCH * SCOPE_MEM_LENGTH_MAX * sizeof(SCOPE_DATA_TYPE)) {
	
    while(1) {
		
			//cout<<"---- "<<run<<endl;
			run++;
			//askTcpData(sockfd,nbytes);		
			//cout<<"asked"<<endl;
        for(;;) {
            
			
            tv.tv_sec  = 0;
            tv.tv_usec = 5000;
            FD_ZERO(&rfd);
            FD_SET(sockfd, &rfd);
            maxfd = sockfd;
            nsel = select(maxfd+1, &rfd, NULL, NULL, &tv);
            if(nsel < 0 && errno != EINTR) { /* other errors */
                warn("select");
                break;
            }
            if(nsel == 0) {
                //warn("select");
                //error_printf("readTotal at break = %zd\n\n", readTotal);
                // close(sockfd);
                // signal_kill_handler(0);
                //readTotal = nbytes;
                break;
            }
            if(nsel>0) {
                if(FD_ISSET(sockfd, &rfd)) {
                    n = read(sockfd, ibuf+readTotal, nbytes-readTotal);
                    // debug_printf("n = %zd\n", n);
                    if(n < 0) {
                        warn("read");
                        break;
                    } else if(n == 0) {
                        warn("read: socket closed");
                        break;
                    }
                    readTotal += n;
                }
            }
            if(readTotal >= nbytes) {
                if(readTotal > nbytes) {
                    error_printf("(readTotal = %zd) > (nbytes = %d)\n", readTotal, nbytes);
                }
                break;
            }
        }
		
            if(readTotal >= nbytes) {
                break;
            }	
    nb++;			
	
	}
	return readTotal;
	
}

//从fpag读取数据， nbytes 为数据量，读到ibuf缓冲区，只需 askTcpData 一次
int read_mem_ns(char *ibuf,int nbytes){
	return read_mem_ns(configSocket,ibuf,nbytes);
}


//从fpag读取数据， nbytes 为数据量，读到ibuf缓冲区，需 askTcpData 多次，支持梅元原始的固件，在新固件中不能用
int read_mem(int sockfd, char *ibuf,int nbytes){
	

	
    struct timeval tv;		
    fd_set rfd;
    ssize_t readTotal;	
    int maxfd, nsel;
    int n;
	readTotal = 0;
	int run=0;
	
	int    nb = 0;
    // while(nb < SCOPE_NCH * SCOPE_MEM_LENGTH_MAX * sizeof(SCOPE_DATA_TYPE)) {
    while(nb < 100) {
		
			// cout<<"---- "<<run<<endl;
			run++;
			askTcpData(sockfd,nbytes);	
			
        for(;;) {

			
            tv.tv_sec  = 2;
            tv.tv_usec = 500 * 1000;
            FD_ZERO(&rfd);
            FD_SET(sockfd, &rfd);
            maxfd = sockfd;
            nsel = select(maxfd+1, &rfd, NULL, NULL, &tv);
            if(nsel < 0 && errno != EINTR) { /* other errors */
                warn("select");
                break;
            }
            if(nsel == 0) {
                warn("select");
                error_printf("readTotal at break = %zd\n\n", readTotal);
                // close(sockfd);
                // signal_kill_handler(0);
                //readTotal = nbytes;
                break;
            }
            if(nsel>0) {
                if(FD_ISSET(sockfd, &rfd)) {
                    n = read(sockfd, ibuf+readTotal, nbytes-readTotal);
                    // debug_printf("n = %zd\n", n);
                    if(n < 0) {
                        warn("read");
                        break;
                    } else if(n == 0) {
                        warn("read: socket closed");
                        break;
                    }
                    readTotal += n;
                }
            }
            if(readTotal >= nbytes) {
                if(readTotal > nbytes) {
                    error_printf("(readTotal = %zd) > (nbytes = %d)\n", readTotal, nbytes);
                }
                break;
            }
        }
		
            if(readTotal >= nbytes) {
                break;
            }	
    nb++;			
	
	}
	return readTotal;
	
}
int read_mem(char *ibuf,int nbytes){
	return read_mem(configSocket,ibuf,nbytes);
}




//字符串操作，看a是否在sp中

bool spa(char a, char* sp=(char*)" \t\r;()=,"){
int l=strlen(sp);
for(int i=0;i<l;i++){
if(a==sp[i]) return true;
}
return false;
}

//分裂字符串，将a变成多个子字符串，空格符号有sp指定

void strsplit(char *a,vector<char *>&b){

int l=strlen(a);
	int space=1;
	for(int i=0;i<l;i++){
		if(space==1&&(!spa(a[i]))){
		b.push_back(a+i);
		space=0;
		continue;
		}
		if(space==0&&spa(a[i])){
			a[i]=0;
			space=1;
			continue;
		}
		if(spa(a[i]))space=1; else space=0;
	}
};

//将字符串参数转化为整形数  比如输入  0xa  变成 10；输入  0b1111  变成 15；
class paralist2var{
public:
	paralist2var(){
	debug=0;
	}
char * c3;
int value;
int debug;
void transform(){
    char *cp=0;
    char *bp=0;

	if(c3[0]=='0'&&(c3[1]=='b'||c3[1]=='B')){
	    bp=c3+2;}
	else if(c3[0]=='b'||c3[0]=='B'){
		bp=c3+1;}
	else if(c3[0]=='0'&&(c3[1]=='x'||c3[1]=='X')){
		cp=c3+2;}
	else if(c3[0]=='x'||c3[0]=='X'){
		cp=c3+1;}
	else{
	int j=0;
	for(j=0;j<strlen(c3);j++){
		if(c3[j]>='a'&&c3[j]<='f') {cp=cp=c3;break;}
		if(c3[j]>='A'&&c3[j]<='F') {cp=cp=c3;break;}
	
	}
	}

	if(bp!=0){
	int j=0;
	for(j=0;j<strlen(bp);j++){
		value=(value<<1);
		if(bp[j]=='1'){ value++; }	
		if(bp[j]!='1'&&bp[j]!='0'){ cout<<"format error "<<c3[j]<<endl; exit(0); }	
	}
	
	}
	else if(cp!=0){
istringstream is( cp );
is >> hex >> value;
is.clear();	
	}else{
	value=atoi(c3);
	}
	if(debug>0)
	cout<<"value= "<<value<<endl;

}
};



#endif /* __COMMON_H__ */
