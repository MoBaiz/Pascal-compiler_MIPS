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
	int num;//�������ת��䣬��ʾ��תĿ�꣨ĳ���м�������ţ�������ǳ�����ֵ������ʾ�ұߵ�����
};
class Middle
{
	int t=-1;//��ʱ�������±��0��ʼ
	int l = -1; //lable �±��0��ʼ              //TODOlable��ַ��¼
	int cur;
	struct Function
	{
		string name;//������
		int begin, end;//�ֱ�ָ��ú����ĵ�һ����Ԫʽ�����һ����Ԫʽ֮���λ��
		int temp;//��ʱ������
	};
public:
	vector<MidCode> MidCodes;
	Middle() :cur(-1){}
	int size() { return MidCodes.size(); };
	void emit(_operator op, string op1, string op2, string res);
	string alloc();
	//�ͷ���ʱ����
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