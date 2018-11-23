#include "lib.h"
 #include "clib.h"

 extern int help;



 int main(int argc, char* argv[]){  string par; if(argc>=2)par=argv[1];gargc=argc;gargv=argv;

 pid=para((char*)"-help");if(pid>0){help=1;} 

 //////////////////////////////////////////////////////// tcp setup
tcp mytcp;
mytcp.IP="192.168.2.3";
mytcp.port="1024";

mytcp.setup();
int sockfd=mytcp.sockfd;
sockfd=mytcp.sockfd;
