
#include<stdio.h>
extern int line;
extern string token;
extern symset sym;
extern string word[45];
extern string line_cache1,line_cache2;
extern bool isright;
extern int  errorcount;
class error{
public:
	error(int n){
		isright = false;
		printf("\n\nnumber:%d\n", ++errorcount);
		printf("error   %d       line:%d\n",n, line);
		cout << "token:" + token + "               sym:" + word[sym]<< endl;
		cout << line_cache2<< endl;
		cout << line_cache1 << endl;
	}
	error(int n,string wrong){
		isright = false;
		printf("\n\nnumber:%d\n", ++errorcount);
		printf("error%d:%s       line:%d\n",n,wrong.c_str(), line);
		cout << "token:" + token + "               sym:" + word[sym] << endl;
		cout << line_cache2 << endl;
		cout << line_cache1 << endl;
	}
	error(string wrong){
		isright = false;
		printf("\n\nnumber:%d\n", ++errorcount);
		printf("error:%s       line:%d\n",wrong.c_str(), line);
		cout << "token:" + token + "               sym:" + word[sym] << endl;
		cout << line_cache2 << endl;
		cout << line_cache1 << endl;
	}
	error(string wrong,int n){
		isright = false;
		printf("error:%s     %d  line:%d\n", wrong.c_str(),n, line);
		cout << "token:" + token + "               sym:" + word[sym] << endl;
		cout << line_cache2 << endl;
		cout << line_cache1 << endl;
	}
}; 