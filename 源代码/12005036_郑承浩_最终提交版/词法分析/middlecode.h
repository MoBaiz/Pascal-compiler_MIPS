#pragma once
#include<string>
#include<vector>
#include<map>
#include<iostream>
#include <iomanip>
#include <strstream>
#include<fstream>
using namespace std;
enum _operator
{
	ADD, SUB, MUL, DIV, MIN,
	J, JEQL, JNEQ, JLSS, JLEQ, JGRE, JGEQ,
	ASS, ASS_ARR, ASS_LARR,
	PARAMETER, CALL_P, CALL_F, RETURN, SCANF, PRINTF_S, PRINTF_C, PRINTF, LABLE
};

struct MidCode
{
	_operator op;
	string op1, op2, res;
	int num;//如果是跳转语句，表示跳转目标（某个中间代码的序号），如果是常量赋值语句则表示右边的整数
};
class Middle
{
	int t=-1;//临时变量的下标从0开始
	int l = -1; //lable 下标从0开始              //TODOlable地址记录
	int cur;
	struct Function
	{
		string name;//函数名
		int begin, end;//分别指向该函数的第一个四元式和最后一个四元式之后的位置
		int temp;//临时变量数
	};
public:
	vector<MidCode> MidCodes;
	Middle() :cur(-1){}
	int size() { return MidCodes.size(); };
	void emit(_operator op, string op1, string op2, string res);
	string alloc();
	//释放临时变量
	void newfunction();
	void release(int n);
	void end();
	void showmidcode(string file);
	string getlable();
	vector<MidCode>::iterator getit();
	void insert_emit(int it, _operator op, string op1, string op2, string res);
//private:
	void print(MidCode mc, ostream &os);
};