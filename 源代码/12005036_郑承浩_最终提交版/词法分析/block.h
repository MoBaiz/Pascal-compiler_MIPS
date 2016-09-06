#pragma once
#include "lex.h"
#include"middlecode.h"
#include"error.h"
#include"table.h"
using namespace std;
class block{
	lex &l;
	table &t;
	Middle &m;
	symset temp;
	bool use = true;
	string global_function_name;
	vector<string> ident_cache;            //缓存标识符
	int array_length;
public:
	block(lex &L, table &T, Middle &M) :l(L), t(T), m(M){};
	void  parse(int level, string name);
private:
	void  const_declaration(int level);
	void  const_define(int level);
	string constant(); 
	string 	case_constant();
	void  chara();
	void  var_declaration(int level);
	void  var_define(int level);
	type  Type(int count);
	type  base_type();
	void  pro_declaration(int level);
	string pro_head(int level);
	int formal_parameter_table(int level);
	int formal_parameter_segment(int level);
	void  funt_declaration(int level);
	string funt_head(int level);
	void  compound_statement();
	void statement();
	void assign_satement(string);
	int actual_parameter_table(string);
	void write_statement();
	void case_statement();
	void condition_statement();
	void case_table(int, string, string);         //返回常量
	void for_statement();
	void read_statement();
	string  expression();
	string term();
	string factor();
	int stringtoint(string s);
	string inttostring(int i){
		strstream ss;
		string s;
		ss << i;
		ss >> s;
		return s;
	}
	bool block::isnum(string op){
		if (isdigit(op[0]) || op[0] == '-')
			return true;
		else return false;
	}
};