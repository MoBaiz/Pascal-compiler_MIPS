#include<stdio.h>
#include<iostream>
#include<fstream>
#include<map>
#include<sstream>
#include <list>
using namespace std;
enum symset{//标识符的类别识别码
	arraysym, constsym, endsym, integersym, thensym, beginsym,
	dosym, forsym, ofsym, tosym, casesym, downtosym, functionsym,
	proceduresym, varsym, charsym, elsesym, ifsym, readsym, writesym,
	add, times, sub, slash, lparen, rparen, eql, comma, period, smark, dmark,
	lss, leq, geq, gre, neq, colon, semicolon, lbrack, rbrack, becomes, ident, number, str, character
};
struct symloc 
{
	symset sym;
	int line;
};
class lex{
	ifstream fin;
	map<string, symset> SymSet;
	list<symloc> Queue;
public:
	lex(string file_name);
	void getch();
	void get_normal_ch();
	void getsym();
	symset queue(); //返回进队元素
	void decache();//消除缓存
private:
	bool isletter(char a);
	bool isnum(char a);
	string chartostring(char a);
	symset dequeue();   //返回出队后队列长度
	int size();
};
