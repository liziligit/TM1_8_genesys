//author xiangming sun sphy2007@126.com

// 这个程序用来执行c++脚本，也就是说，用 C++的格式编写脚本文件可以用该程序执行
//  比如，该程序编译成可执行文件 mytcp 后,  用./mytcp -c scr.c 可以运行scr.c里面的内容，src.c以c++的格式来编写
// 编译该程序用g++ ccode.cpp -lhdf5 -o mytcp 编译


#include "lib.h"
#include "clib.h"

extern int help;  //帮助开关
extern int pid;   //参数命令匹配变量，参见具体使用
extern int gargc;  //全局输入参数个数
extern char** gargv;  //全局输入参数列表


int main(int argc, char* argv[])
{
//将系统的参数个数和参数列表传给全局参数个数和全局参数列表

	gargc=argc;
	gargv=argv;

//这些字符串用来和输入的c++脚本共同组成完整的c++文件。
//这是一个小技巧，可以看到main函数的声明在这些字符串里
//最后还有return和大括号，在输入的c脚本里就不需要这些内容	
string head="#include \"lib.h\"\n #include \"clib.h\"\n\n extern int help;\n\n";
string head1="\n\n int main(int argc, char* argv[]){  string par; if(argc>=2)par=argv[1];gargc=argc;gargv=argv;\n\n pid=para((char*)\"-help\");if(pid>0){help=1;} \n\n ";
string tail="\n\n if(help==1)cout<<endl; \n\n return 0;} \n\n";

//参数命令
//帮助和调试
pid=para((char*)"-help");  
if(pid>0){
	help=1;
}

file_loder myfile;
int compile=0;


//程序会默认在main函数里插入tcp.c，以便产生tcp连接
//使用 -tcp 命令改变所需的tcp文件
string lib="tcp.c";
int libTrue=1;
char *plib=(char*)lib.c_str();


pid=para((char*)"-debug");  
if(pid>0){
	debug=atoi(argv[pid+1]);
}



pid=para((char*)"-tcp");  
if(pid>0){
	lib=argv[pid+1];
	plib=(char*)lib.c_str();	
}
//使用 -noTcp 命令  不插入 tcp.c文件
pid=para((char*)"-noTcp");  
if(pid>0){
	libTrue=0;	
}

//-c_file 或者 -c命令编译并执行后面的c脚本文件

pid=para((char*)"-c_file");
if(pid<=0){
pid=para((char*)"-c");
}  
if(pid>0){
	string fn=argv[pid+1];  //获取脚本名字
	vector <string> para;  //支持传入多个参数
        for(int i=pid+2; i<argc;i++){
		if(argv[i][0]=='-') break;
			para.push_back(argv[i]);
		
	};
	string fne=fn+"e";  //生成的可执行文件的名字 src.c 变成 src.ce
	string fnc=fn+"c";  //生成的完整的c文件的名字 src.c 变成 src.cc
	string cmd;
	char *pfn=(char*)fn.c_str();
	char *pfnc=(char*)fnc.c_str();	
	char *pfne=(char*)fne.c_str(); //提取各种文件名字的字符串指针	
	long tc=file_loder::file_time(pfn);
	long te=file_loder::file_time(pfne);  //获取文件生成时间
	long tl=0;
	if(libTrue){
	tl=file_loder::file_time(plib);	//查看库文件生成时间，看有没有改动
	}	
	
	int tlib=file_loder::file_time((char*)"lib.h");
	if(tlib>tl) tl=tlib;
    tlib=file_loder::file_time((char*)"clib.h"); //查看库文件生成时间，看有没有改动
	if(tlib>tl) tl=tlib;
	
	if(debug>0){                   //如果发现库文件或者c脚本文件有更新，compile=1,就要重新编译
	cout<<"tc= "<<tc<<endl;
	cout<<"te= "<<te<<endl;
	cout<<"tl= "<<tl<<endl;
	}
	if(tc==0){compile=1;}
	else if(te==0){compile=1;}
	else if(tc>te){compile=1;}
	else if(tl>te){compile=1;}
	else {compile=0;}
	if(debug>0){
	cout<<"compile= "<<compile<<endl;
	}	

	if(compile){     //重新编译



	myfile.write(pfnc,head,(char*)"wb");  //写入头
	myfile.write(pfnc,head1,(char*)"ab");  //写入头
	
	if(libTrue){                      //写入tcp.c
	myfile.load(plib);	
	myfile.write(pfnc,(char*)"ab");		
	}
	
	
	myfile.load(pfn);	
	myfile.write(pfnc,(char*)"ab");      //写入脚本文件
	myfile.write(pfnc,tail,(char*)"ab");  //写入尾巴  src.cc生成完毕
//	cmd="g++ "+fnc+" -lhdf5 -o "+fne;  //生成编译命令
	cmd="g++ "+fnc+" -lpthread -o "+fne;  //生成编译命令
	system(cmd.c_str());
	/*	if(debug<10000){      //该条件成立 删除src.cc
	cmd="rm -f "+fnc;
	system(cmd.c_str());
		}*/	
	}
	
	if(compile){
		te=file_loder::file_time(pfne);  //如果src.ce比其他文件生成的早，报错
		if(tc>te||tl>te){
			cout<<"script format error"<<endl;
			return 0;}
	}


	cmd="./"+fne+" ";  //执行src.ce

        for(int i=0; i<para.size();i++){
        if(i%2==0) cmd+=" -"+para[i];
        else       cmd+=" "+para[i];	
        }
        //cout<<cmd.c_str()<<endl;
	system(cmd.c_str());
	
}



// mytcp.IP="192.168.2.3";
// mytcp.port="1024";

// mytcp.setup();
// mytcp.configure_adc();

return 0;
}
