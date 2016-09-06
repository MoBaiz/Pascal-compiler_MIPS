#pragma once
#include <vector>
#include<map>
#include<iostream>
#include<stdio.h>
#include <list>
#include<map>
#include<iostream>
#include<string>
#include<strstream>
using namespace std;
enum kinds{
	Const, Var, Procedure, Funcion, nop
};
enum type{   //��������
	Int, Char, Arrayint,Arraychar,unkown,varInt,varChar
};
struct flame{
	string name;
	kinds k;
	type t;
	int val;
	int level;
	int addr;
	bool val_formal;
	string function_filed;      //����ִ��·��
	string rename_function;     //�������ĳ�����
};
struct Block{
	int begin;
	int end;
	int level;
	int addr;
	string function_filed;
};
class table{
	int begin=-1;
	int Addr = 1;
	map<int, flame> stack;
	vector<Block> link;
	map<int,bool> usecondition;
	map<string,vector<Block>> Lqueue;
	flame temp;
	int top = -1;
	string function_name;
	string cur_function_name;
	int function_count=0;
public:
	void insert(string name, kinds k, type t, int val, int level);
	void table::insert(string name, string rename, kinds k, type t, int val, int level);
	void setfunctionname(string name){
		function_name = function_name+"/"+name;
		cur_function_name = name;
	};
	void endblock();
	void insertTV(type t, int val);
	void show();
	void insertT_Back(int length,type t);          //����������� 
	void insertT_Back2(int length, type t,int val);         //�����������
	void insertT(type t);
	void insertV_back(int val,int back);
	void insertTV_back(type t, int val, int back);
	void pop(string);
	void push(int level);
	void storetable(string name);
	void showlink();
	void showqueue();
	void resetval();
	void changetable(string name);
	int lookup(string name, kinds k);
	int lookup(string name, type t);
	type lookup_type(string name);       //����������
	int ret_addr();        //���ظú����ı�����
	bool findfunt(string name);
	kinds lookup_kind(string name);
	int lookup_var(string name);
	bool table::lookup_formal(string name);//todo ����ͬ��
	bool table::lookup_formal(string funtion_id, int offset);
	int table::ret_function_offset_l(string name);
	string ret_const(string name);
	int table::ret_offset_l(string name, bool base);
	int ret_addr_var(string name,bool base);//���ر���name�ĵ�ַ
	void insert_formal_back(int count);  //���ñ����β�
	string table::findvar(string function_name, int offset);//������var�ı���
	void table::insertnop(string functionname, int level);    //�޲�����������Nop
	string table::alloc_function_name(string);
	string function_rename(string name);
	int ret_array_length(string s);
	int table::ret_funct_addr(){
			int length = stack.size() - 1;
			int level = stack[length].level;
			int i;
			for (i = length; i >= 0; i--){
				if (stack[i].level != level&& usecondition[i]){
					if (stack[i].k == Funcion)
						return stack[i].addr;
					else return -1;
				}
			}
			return -1;
		}
	int table::ret_funct_pro_val_length(string name){
		int length = stack.size() - 1;
		use();
		for (int i = length; i >= 0; i--){
			if (stack[i].name== name&&(stack[i].k==Funcion||stack[i].k==Procedure)&& usecondition[i]){
				return stack[i].val;
			}
		}
		return -1;
	}
	bool table::lookup_all(string name);
private:
	flame find(int index);
	void use();                 //��Ӧÿ��ջ��ʹ�����,��¼��usecondition����
	bool isin(int i);               //�ж�ջ�Ƿ���ʹ��״̬
	void error(int i);
	string cur_function(string);             //���ص�ǰ������
	void sub_curname(string);
	void  redefine_cheak(string name,kinds k,type t,string path,int length);
	string inttostring(int num)
	{
		strstream ss;
		string s;
		ss << num;
		ss >> s;
		return s;
	}
};