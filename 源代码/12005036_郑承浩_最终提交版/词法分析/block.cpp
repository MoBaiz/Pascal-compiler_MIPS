#include"block.h"
#include"table.h"
#include<string>
extern symset sym;
extern int line;
extern string token;
extern string word[45];
extern type t;
extern kinds K;
void showsym(string s){
	cout << word[sym]+"   token:"+token+"           "+s << endl;
};
class kind{
public:
	kind(string s){
		//cout << "this is " + s + " statement.     line:";
	//	cout << line << endl;
	}
};
void block::parse(int level,string name){
	//showsym("begin----------------------------------------------------------");
	//cout <<level<<endl;
	if (sym == constsym){
		const_declaration(level);
		l.getsym();
	}
	
	if (sym == varsym){
		var_declaration(level);
		l.getsym();
	}
	while (sym == proceduresym || sym == functionsym){
		if (sym == proceduresym)
			pro_declaration(level);
		else funt_declaration(level);
		l.getsym();
	}
	t.push(level);//到复合语句就不会有定义了 将过程名 函数名入栈
	if (sym == beginsym){
		m.emit(LABLE, name, "", "");
		compound_statement();
	}
	else error(33, "过程或函数说明没有以;结尾");
	//showsym("end this is a block------------------------------------------");
	//cout << level<< endl;
	t.storetable(name);
	t.pop(name);
	m.emit(RETURN,"", "", "");
}
void block::const_declaration(int level){
	l.getsym();
	const_define(level);
	while (sym == comma){
		l.getsym();
		const_define(level);
	}
	if (sym == semicolon){
		kind("const declaration");
	}
}
void  block::const_define(int level){
	if (sym == ident){
		t.insert(token,Const,unkown,0,level);
		l.getsym();
		if (sym == eql){
			l.getsym();
			constant();
			kind("const define");
		}
		else  error(2,"常量定义中预料的=没有出现");
	}
	else error(1,"常量定义缺少标识符");
}
string block::constant(){
	bool nagative = false;
	string return_temp;
	if (sym == number){
		kind("constant");
		return_temp = token;
		t.insertTV(Int,stringtoint(token));
		l.getsym();
	}
	else if (sym == add || sym == sub){
		if (sym == sub)
			nagative = true;
		l.getsym();
		if (sym == number&&nagative){
			nagative = false;
			kind("constant");
			return_temp = "-"+token;
			t.insertTV(Int, 0-stringtoint(token));
			l.getsym();
		}
		else if (sym == number){
			kind("constant");
			return_temp = token;
			t.insertTV(Int,stringtoint(token));
			l.getsym();
		}
		else error(3,"非法的常量值");
	}
	else if (sym == character){
		kind("constant");
		return_temp = token;
		t.insertTV(Char,token[0]);
		l.getsym();
	}
	return return_temp;
}
void block::var_declaration(int level){
	l.getsym();
	var_define(level);
	l.getsym();
	if (sym == semicolon){
	    l.queue();           //预读到procedure
		if (sym == beginsym){
			kind("var declaration");
			return;
		}
		else if (sym == proceduresym || sym == functionsym)           //todo  test
			return;                   //不是变量声明语句
	}
	else error(5,"变量说明后应该有；");
	do{
		l.queue();
		if (sym == proceduresym || sym == functionsym||sym==beginsym){           //todo  test
			kind("var declaration");
			return;
		}    
		l.getsym();
		var_define(level);
		l.getsym();
	} while (sym==semicolon);
}
void block::var_define(int level){
	int count = 0;
	if (sym == ident){
		//insert(token,Var,unkown,0, level);
		ident_cache.push_back(token);
		l.getsym();
		count++;
		while (sym == comma){
			l.getsym();
		//	t.insert(token, Var, unkown, 0, level);
			ident_cache.push_back(token);
			count++;
			if (sym != ident)
				error(6,"缺少变量");
			l.getsym();
		}
		if (sym == colon){
			l.getsym();
            type tmep_t=Type(count);
			for (vector<string>::iterator it = ident_cache.begin(); it != ident_cache.end(); it++){
				t.insert(*it, Var, tmep_t,array_length, level);
			}
			ident_cache.clear();
			kind("var define");
		}
		else error(7,"变量定义中缺少：");
	}
}
type block::base_type(){
	if (sym == integersym || sym == charsym)
		kind("base type");
	if (sym == integersym)
		return Int;
	else if (sym == charsym)
		return Char;
	else error(8,"不是基本类型");
	return unkown;
}
type block::Type(int count){
	//if (sym == integersym || sym == charsym){
		//t.insertT_Back(count, base_type());
	if (sym == integersym){
		array_length = 0;
		return Int;
	}
	else if (sym == charsym){
		array_length = 0;
		return Char;
	}
//	}
	else if (sym == arraysym){
		l.getsym();
		if (sym == lbrack){
			l.getsym();
			array_length = stringtoint(token);
			if (sym == number){
				l.getsym();
				if (sym == rbrack){
					l.getsym();
					if (sym == ofsym){
						l.getsym();
						type b_temp=base_type();
						if (b_temp == Int)
							//t.insertT_Back2(count,Arrayint,length);              //如果是数组，地址要相应的加上数组的长度
							return Arrayint;
						else if (b_temp == Char)
							//t.insertT_Back2(count, Arraychar, length);
							return Arraychar;
					}
				}
			}
		}
	}
}

void  block::pro_declaration(int level){
	string name=pro_head(level);
	l.getsym();
	parse(level+1,name);
	l.getsym();
	if (sym == semicolon){
		l.queue();
		while (sym == proceduresym){
			l.decache();
			name=pro_head(level);
			l.getsym();
			parse(level+1,name);
			l.getsym();
			l.queue();
		}
		if (sym == beginsym){
			kind("pro declaration");
		}
	}
	else error(12, "函数说明部分应该以;结尾");
}
string block::pro_head(int level){
	string name;
	l.getsym();
	if (sym == ident){
		name = t.alloc_function_name(token);
		t.insert(token,name,Procedure,unkown,0, level);
		l.getsym();
		if (sym == lparen){       //进参数表
			l.getsym();
			t.push(level);
			t.setfunctionname(name);
			t.resetval();
			m.newfunction();
			int length=formal_parameter_table(level+1);
			l.getsym();
			t.insertV_back(length,length);
			kind("procedure head");
		}
		else if (sym == semicolon){
			t.push(level);
			t.setfunctionname(name);
			t.resetval();
			t.insertnop(name, level + 1);
			m.newfunction();
			kind("procedure head");
		}
	}
	return name;
}
int block::formal_parameter_table(int level){
	int count=0;
	count+=formal_parameter_segment(level);
	l.getsym();
	while (sym == semicolon){
		l.getsym();
	count+=formal_parameter_segment(level);
		l.getsym();
	}
	if (sym == rparen)
		kind("formal parameter table");
	else error(9,"错误的形式参数表");
	return count;
}
int block::formal_parameter_segment(int level){
	int count = 0;
	bool isvar = false;                    //  true:带var的形参                 false:正常形参
	if (sym == varsym){
		l.getsym();
		isvar = true;
	}
	if (sym == ident){
	//	t.insert(token,Var,unkown,0,level);
		ident_cache.push_back(token);
		count++;
		l.getsym();
		while (sym == comma){
			l.getsym();
			if (sym == ident){
				ident_cache.push_back(token);
				l.getsym();
		//		t.insert(token, Var, unkown, 0, level);
				count++;
			}
			else error(10,"形式参数段中缺少变量");
		}
		if (sym == colon){
			l.getsym();
			type t_temp=base_type();
		//	 if (sym == integersym)
		//		t.insertT_Back(count,Int);
		//	else if (sym == charsym&&isvar)
		//		t.insertT_Back(count,Char);

			for (vector<string>::iterator it = ident_cache.begin(); it != ident_cache.end(); it++){
				t.insert(*it, Var, t_temp, 0, level);
			}
			ident_cache.clear();
			 if (isvar)
				 t.insert_formal_back(count);
			kind("formal parameter segment");
		}
		else error(11,"形式参数段中缺少：");
	}
	return count;
}
void  block::funt_declaration(int level){
	string name=funt_head(level);
	l.getsym();
	parse(level+1,name);
	l.getsym();
	if (sym == semicolon){
		l.queue();
		while (sym == functionsym){
			l.decache();
			name=funt_head(level);
			l.getsym();
			parse(level+1,name);
			l.getsym();
			l.queue();
		}
		if (sym == beginsym)
			kind("funt declaration");
	}
	else error(12, "函数说明部分应该以;结尾");
}
string block::funt_head(int level){
	int count = 0;
	string name;
	l.getsym();
	if (sym == ident){
		name = t.alloc_function_name(token);
		t.insert(token,name,Funcion, unkown, 0, level);
		l.getsym();
		if (sym == lparen){       //进参数表
			l.getsym();
			t.push(level);
			t.setfunctionname(name);
			t.resetval();
			m.newfunction();
			count+=formal_parameter_table(level+1);
			l.getsym();
			if (sym == colon){
				l.getsym();
				if (sym == integersym)
					t.insertTV_back(Int,count,count);
				else if (sym == charsym)
					t.insertTV_back(Char, count, count);
				base_type();
				l.getsym();   //;
				kind("function head");
			}
			else error(13,"函数首部缺少:");
		}
		else if (sym == colon){  //没有参数表                
			l.getsym();
			if (sym == integersym)
				t.insertTV_back(Int, count, count);
			else if (sym == charsym)
				t.insertTV_back(Char, count, count);
			base_type();
			t.push(level);
			t.setfunctionname(name);
			t.resetval();
			t.insertnop(name, level + 1);
			m.newfunction();
			l.getsym();   //;
			kind("function head");
		}
		else if (sym == semicolon){
			/*t.push(level);
			t.setfunctionname(name);
			t.resetval();
			t.insertnop(name, level + 1);
			m.newfunction();
			kind("function head");*/
			error("函数没有类型");
		}
	}
	return name;
}
string block::expression(){
	string op1, op2, res,temp_alloc;;
	symset t_temp;
	bool use = true;
	if (sym == add || sym == sub){
		t_temp = sym;
		l.getsym();
		op1=term();
		if (t_temp == sub){
			res = m.alloc();
			m.emit(MIN, op1, "",res);
			op1 = res;
		}
	}
	else op1=term();
	res = op1;
	l.queue();
	while (sym == add || sym == sub){
		symset t = sym;
		l.decache();
		l.getsym();
		op2=term();
		if (t == add&&use){
			res = m.alloc();
			m.emit(ADD, op1, op2, res);
			use = false;
		}
		else if (t == add){
			temp_alloc = m.alloc();
			m.emit(ADD, res, op2, temp_alloc);
			res = temp_alloc;
		}
		else if (t == sub&&use){
			res = m.alloc();
			m.emit(SUB, op1, op2, res);
			use = false;
		}
		else if (t == sub){
			temp_alloc = m.alloc();
			m.emit(SUB, res, op2, temp_alloc);
			res = temp_alloc;
		}
		l.queue();
	}
	kind("expression");
	//cout << "??????????" + res << endl;
	return res;
}

string block::term(){
	string op1, op2, res, temp_alloc;
	op1=factor();
	 res = op1;
	l.queue();
	while (sym == times || sym == slash){
		symset t = sym;
		l.decache();
		l.getsym();
		op2 = factor();
		if (t == times&&use){
			res = m.alloc();
			m.emit(MUL, op1, op2,res);
			use = false;
		}
		else if (t == times){
			temp_alloc = m.alloc();
			m.emit(MUL, res,op2,temp_alloc);
			res = temp_alloc;
		}
		else if (t == slash&&use){
			res = m.alloc();
			m.emit(DIV, op1, op2, res);
			use = false;
		}
		else if (t == slash){
			temp_alloc = m.alloc();
			m.emit(DIV, res, op2, temp_alloc);
			res = temp_alloc;
		}
		l.queue();
	}
	kind("term");
	return res;
}
/*string block::factor(){
	if (sym == number){
		kind("factor");
		return token; 
	}	
	else if(sym == lparen){
		l.getsym();
		expression();
		l.getsym();
		if (sym == rparen){
			kind("factor");
			return token;    //表达式
		}
		else error(14);
	}
	else if (sym == ident){
		if (t.lookup(token, Var)){
			kind("var factor");
			return token;
		}
		l.getsym();
		if (sym == lbrack){
			l.getsym();
			expression();
			l.getsym();
			if (sym == rbrack)
				kind("factor");
			else error(15);
		}
		else if (sym == lparen){
			l.getsym();
			actual_parameter_table();
			kind("factor");
		}
	}
}
*/
string block::factor(){
	string op1, op2, res;
	if (sym == number){
		kind("factor");
		return token;
	}
	else if (sym == character){
	   error("因子不能是字符");
		int val = token[0];
		return inttostring(val);
	}
	else if (sym == lparen){
		l.getsym();
		op1=expression();
		l.getsym();
		if (sym == rparen){
			kind("factor");
			return op1;
		}
		else error(14,"（表达式）缺少右括号");
	}
	else if (sym == ident){
		if (!t.lookup_all(token))
			error("未定义的标识符");
		temp = sym;
		string s = token;
        kinds k=t.lookup_kind(token);
		type T = t.lookup_type(token);
		if (k == nop)
			error("没有定义的变量");
		else if (k == Procedure)
			error("过程不能再因子中被调用");
		l.queue();
		 if (sym == lbrack){
			 if (T != Arraychar&&T != Arrayint)
				 error("没有定义的数组变量");
			l.decache();
			l.getsym();
			op2=expression();
			if (isnum(op2)){
				int num = stringtoint(op2);
				if (num >= t.ret_array_length(s)){
					error("数组越界");
					cout << "数组长度应该小于："; cout << t.ret_array_length(s) << endl;
				}
				else if (num < 0)
					error("数组下标不能小于0");
			}
			
			l.getsym();                                            //数组取值
			if (sym == rbrack){
				res = m.alloc();
				
				if (op2.find('.') != -1)    //已经赋值属性，对res重新属性
					op2 = op2.substr(0, op2.find_first_of('.'));
					res = res + "." + "&" + s + "|" + op2;
					m.emit(ASS, "&" + s + "|" + op2, "", res);
					
			//	m.emit(ASS, "&" + s + "|" + op2, "", res);
				kind("factor");
			//	return res + "." + "&" + s + "|" + op2;
				return res;
			}
			else error(15, "数组缺少]");
		}
		else if (sym == lparen){
			l.decache();
			l.getsym();
			int length=actual_parameter_table(s);
			int fuck = t.ret_funct_pro_val_length(s);
			if (t.ret_funct_pro_val_length(s) != length)
				error("函数调用参数数量应该是", t.ret_funct_pro_val_length(s));
			m.emit(CALL_F,s,"","");
			kind("factor");
			return "?"+s;                  //返回函数名       
		}
		else {                            //标识符
			sym = temp;
			if (t.lookup_kind(s) == Funcion)
				m.emit(CALL_F,s,"","");
			kind("factor");
			int i;
			//if ((i = t.lookup(s, Const)) != -1)
		    //  return t.ret_const(s);           //常量直接返回其值，若为字符 则为askii码值
			//else 
			return s;
		}
	}
	return "";
}
void  block::compound_statement(){
	do{
		l.getsym();
		if (sym == endsym)
			break;
	/*	while (sym==semicolon){
			l.queue(); 
			l.decache();
		}*/
		statement();
	    l.getsym();
	} while (sym == semicolon);
	if (sym != endsym)
		error(29,"复合语句缺少end");
	 kind("compund");
}
void block::statement(){
	string temp_token;
	string temp_alloc;
	if (sym == ident){
		assign_satement(token);
	}
	else if (sym == ifsym){
		condition_statement();
		kind("");
	}
	else if (sym==casesym){
		case_statement();
		kind("");
	}
	else if (sym == beginsym){
		compound_statement();
		kind("");
	}
	else if (sym == readsym){
		read_statement();
		kind("");
	}
	else if (sym == writesym){
		write_statement();
		kind("");
	}
	else if (sym == forsym){
		for_statement();
		kind("");
	}
}

void block::assign_satement(string Id){
	if (!t.lookup_all(Id))
		error("非法的赋值语句左部");
	if (t.lookup_kind(Id) == Const)
		error("常量不能被赋值");
	string op2, temp_alloc;
	l.queue();
	if (sym == becomes){
		if (t.lookup_kind(Id) == Procedure)
			error("过程名不能被赋值");
		l.decache();
		l.getsym();
		type T = t.lookup_type(Id);
		if (T != Int&&T != Char)
			error("非法的赋值语句，赋值语句左值错误");
		temp_alloc=expression();
		kind("assign satement");
		m.emit(ASS,temp_alloc,"",Id);
	}
	else if (sym == lbrack){
		l.decache();
		l.getsym();
		op2 = expression();
		if (isnum(op2)){
			int num = stringtoint(op2);
			if (num >= t.ret_array_length(Id)){
				error("数组越界");
				cout << "数组长度应该小于："; cout << t.ret_array_length(Id) << endl;
			}
			else if (num<0)
			{
				error("数组下标不能小于0");
			}
		}
		l.getsym();
		if (sym == rbrack){
			l.getsym();  
			if (t.lookup(Id, Arraychar) == -1 && t.lookup(Id, Arrayint) == -1)           //数组没有定义
				error(101,"使用的数组没有定义");
			if (sym == becomes){
				l.getsym();
				temp_alloc=expression();
				kind("assign sataement");
				//m.emit(ASS_ARR, Id, op2, temp_alloc);
				m.emit(ASS,temp_alloc,"","&"+Id+"|"+op2);
				return;
			}
			else error(20,"赋值语句缺少:=");
		}
		error(14,"赋值语句中数组缺少]");
	}

	else if (sym == lparen){
		l.decache();
		l.getsym();
		int length=actual_parameter_table(Id);
		if (t.ret_funct_pro_val_length(Id) != length)
			error("过程调用参数数量应该是", t.ret_funct_pro_val_length(Id));
		//if (t.lookup_kind(Id) == Procedure)                            //只能是过程调用语句
			m.emit(CALL_P, Id, "", "");
	//	else error(39);
		kind("pro call");                      //只能是过程调用语句
	}
	else {
		if (t.lookup_kind(Id) == Procedure)                            //只能是过程调用语句
			m.emit(CALL_P,Id,"","");
		else error("必须是过程调用语句");
		kind("pro call");
	}
}

/*
void block::assign_satement(string Id){
l.queue();
if (sym == becomes){
l.decache();
l.getsym();
expression();
kind("assign satement");
}
else if (sym == lbrack){
l.decache();
l.getsym();
expression();
l.getsym();
if (sym == rbrack){
l.getsym();
if (sym == becomes){
l.getsym();
expression();
kind("assign sataement");
return;
}
else error(20);
}
error(14);
}}
*/

int block::actual_parameter_table(string id){
	string op1;
	int var_count=0;
	int count = 1;
	op1=expression();                            
	m.emit(PARAMETER,op1,id,inttostring(count++));
	var_count++;
	l.getsym();
	if (sym == rparen){
		kind("actual_parameter_table");
		return var_count;
	}
	else if (sym != comma)
		error(19,"实在参数表中缺少,");
	do{
		l.getsym();
		op1 = expression();
		m.emit(PARAMETER, op1, id, inttostring(count++));
		var_count++;
		l.getsym();
	} while (sym == comma);
	if (sym == rparen)
		kind("actual_parameter_table");
	else error(19, "实在参数表中缺少)");
	return var_count;
}
void block::write_statement(){
	string temp;
	l.getsym();
	if (sym == lparen){
		l.getsym();
		if (sym == str){
			temp = token;
			l.getsym();
			m.emit(PRINTF_S,temp,"","");
			if (sym == rparen){
				kind("write statement");
				return;
			}
			else if (sym == comma){
				l.getsym();
				temp=expression();
				if (temp.find('.') != -1){   //数组
					string t_temp = temp.substr(0,temp.find("."));
					int pos1 = temp.find('&');
					int pos2 = temp.find('|');
					int  length = pos2 - pos1-1;
					string  array_name = temp.substr(pos1+1,length);
					if (t.lookup(array_name, Arraychar) != -1)
						m.emit(PRINTF_C,t_temp, "", "");
					else 	m.emit(PRINTF, t_temp, "", "");
				}
				else  if (t.lookup(temp,Char)!=-1)   //字符
					m.emit(PRINTF_C, temp, "", "");
				else m.emit(PRINTF,temp,"","");	    //数字
				l.getsym();
				if (sym == rparen){
					kind("write statement");
					return;
				}
				else error(17,"写语句中缺少)");
			}
		}
		temp=expression();
		if (t.lookup(temp, Char) != -1)
			m.emit(PRINTF_C, temp, "", "");
		else m.emit(PRINTF, temp, "", "");
		l.getsym();
		if (sym == rparen){
			kind("write statement");
			return;
		}
			
		else error(17,"写语句中缺少)");
	}
	else error(18,"写语句中缺少(");
}
void block::case_statement(){
	string lable_end,op1,op2,res;
	l.getsym();
	op1=expression();
	l.getsym();
	lable_end = m.getlable();
	int it = m.size();
	if (sym == ofsym){
		l.getsym();
		case_table(it++,op1,lable_end);
		l.getsym();
		while (sym == semicolon){
			l.getsym();
			case_table(it++,op1,lable_end);
			l.getsym();
		}
		m.insert_emit(it, J,"","",lable_end);
		if (sym == endsym)
			kind("case statement");
		else error(21,"情况语句缺少end");
	}
	m.emit(LABLE, lable_end, "", "");
}

void block::condition_statement(){
	string op1;
	string op2;
	string res;
	symset temp_sym;
	string lable1;
	string lable2;
	l.getsym();
	op1=expression();
	l.getsym();
	if (sym == lss || sym == leq || sym == gre || sym == geq || sym == eql || sym == neq){
		temp_sym = sym;
		l.getsym();
		op2=expression();
		lable1 = m.getlable();
		switch (temp_sym)
		{
		case lss:
			m.emit(JGEQ, op1, op2, lable1);              //与实际情况相反，跳过then语句
			break;
		case leq:
			m.emit(JGRE, op1, op2, lable1);
			break;
		case gre:
			m.emit(JLEQ, op1, op2, lable1);
			break;
		case geq:
			m.emit(JLSS, op1, op2, lable1);
			break;
		case eql:
			m.emit(JNEQ, op1, op2, lable1);
			break;
		case neq:
			m.emit(JEQL, op1, op2, lable1);
			break;
		default:
			break;
		}
		l.getsym();
		if (sym == thensym){
			l.queue();
			if (sym!=elsesym&&sym!=endsym){
				l.decache();
				statement();
				l.queue();
			}//空语句
			if (sym == elsesym){
				lable2 = m.getlable();
				m.emit(J,"","",lable2);
				m.emit(LABLE, lable1, "", "");
				l.decache();
				l.queue();
				if (sym != semicolon&&sym != endsym){
					l.decache();
					statement();
				}//空语句
				kind("if-then-else condition statement");
				m.emit(LABLE, lable2, "", "");
				return;
			}
			m.emit(LABLE, lable1, "", "");
			kind("if-then condition statement");
		}
		else error(30,"条件语句缺少then");
	}
	else error(23,"条件语句缺少关系运算符");
}
void block::case_table(int it,string op1, string lable_end){
	string op2,lable;
	op2=case_constant();//带getsym
	lable = m.getlable();
	//m.emit(JEQL,op1,op2,lable);
	m.insert_emit(it, JEQL, op1, op2, lable);
	if (sym == colon){
		m.emit(LABLE, lable, "", "");
		l.queue();
		if (sym != endsym&&sym!=semicolon){
			l.decache();
			statement();
		}//空语句
		
		m.emit(J, "", "",lable_end);      //待回写back值
		kind("case table");
	}
	else error(22,"情况元素表缺少:");
}
string block::case_constant(){
	bool nagative = false;
	string return_temp;
	if (sym == number){
		return_temp = token;
		l.getsym();
	}
	else if (sym == add || sym == sub){
		if (sym == sub)
			nagative = true;
		l.getsym();
		if (sym == number&&nagative){
			nagative = false;
			return_temp = "-" + token;
			l.getsym();
		}
		else if (sym == number){
			return_temp = token;
			l.getsym();
		}
		else error(3, "非法的常量值");
	}
	else if (sym == character){
		kind("constant");
		int val = token[0];
		return_temp = inttostring(val);
		l.getsym();
	}
	return return_temp;
}
void block::for_statement(){
	string op1, op2, res;
	string lable1, lable2;
	symset temp_sym;
	lable1 = m.getlable();
	lable2 = m.getlable();
	l.getsym();
	if (sym == ident){
		t.lookup_var(token);
		res = token;
		l.getsym();
		if (sym == becomes){
			l.getsym();
			op1=expression();
			m.emit(ASS,op1,"",res);
			l.getsym();
			if (sym== tosym)
				m.emit(SUB, res, "1", res);
			else m.emit(ADD, res, "1", res);
			m.emit(LABLE,lable1,"","" );
			if (sym == downtosym || sym == tosym){
				temp_sym = sym;
				if (temp_sym == tosym)
					m.emit(ADD, res, "1", res);
				else m.emit(SUB, res, "1", res);
				l.getsym();
				op2=expression();
				if (temp_sym == tosym)
					m.emit(JGRE, res, op2, lable2);
				else m.emit(JLSS, res, op2, lable2);
				l.getsym();
				if (sym == dosym){
					l.queue();
					if (sym!= semicolon){
						l.decache();
						statement();
					}//空语句
					kind("for");
					m.emit(J, "", "", lable1);
					m.emit(LABLE, lable2, "", "");
				}
				else error(25,"for语句缺少do");
			}
			else error("应该是 downto 或者 to ");
		}
		else error(20,"for语句中的赋值语句缺少：=");
	}
	else error(24,"for语句缺少循环变量");
}
void block::read_statement(){
	l.getsym();
	if (sym == lparen){
		l.getsym();
		if (sym == ident){
			m.emit(SCANF, token, "", "");
			l.getsym();
			while (sym == comma){
				l.getsym();
				if (sym == ident)
				m.emit(SCANF, token, "", "");
				else error(27,"缺少读语句的赋值对象");
				l.getsym();
			}
			if (sym == rparen)
				kind("read statement");
			else error(28,"读语句没有正确的 )");
		}
		else error("缺少读语句的赋值对象");
	}
	else error(26,"错误的read 语句结尾");
}
int block::stringtoint(string s){
	int num=0;
	int length = s.size();
	for (int i = 0; i <length; i++){
		num = num * 10 + s[i] - '0';
	}
	return num;
}
