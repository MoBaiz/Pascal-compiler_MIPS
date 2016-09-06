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
	t.push(level);//���������Ͳ����ж����� �������� ��������ջ
	if (sym == beginsym){
		m.emit(LABLE, name, "", "");
		compound_statement();
	}
	else error(33, "���̻���˵��û����;��β");
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
		else  error(2,"����������Ԥ�ϵ�=û�г���");
	}
	else error(1,"��������ȱ�ٱ�ʶ��");
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
		else error(3,"�Ƿ��ĳ���ֵ");
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
	    l.queue();           //Ԥ����procedure
		if (sym == beginsym){
			kind("var declaration");
			return;
		}
		else if (sym == proceduresym || sym == functionsym)           //todo  test
			return;                   //���Ǳ����������
	}
	else error(5,"����˵����Ӧ���У�");
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
				error(6,"ȱ�ٱ���");
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
		else error(7,"����������ȱ�٣�");
	}
}
type block::base_type(){
	if (sym == integersym || sym == charsym)
		kind("base type");
	if (sym == integersym)
		return Int;
	else if (sym == charsym)
		return Char;
	else error(8,"���ǻ�������");
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
							//t.insertT_Back2(count,Arrayint,length);              //��������飬��ַҪ��Ӧ�ļ�������ĳ���
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
	else error(12, "����˵������Ӧ����;��β");
}
string block::pro_head(int level){
	string name;
	l.getsym();
	if (sym == ident){
		name = t.alloc_function_name(token);
		t.insert(token,name,Procedure,unkown,0, level);
		l.getsym();
		if (sym == lparen){       //��������
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
	else error(9,"�������ʽ������");
	return count;
}
int block::formal_parameter_segment(int level){
	int count = 0;
	bool isvar = false;                    //  true:��var���β�                 false:�����β�
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
			else error(10,"��ʽ��������ȱ�ٱ���");
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
		else error(11,"��ʽ��������ȱ�٣�");
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
	else error(12, "����˵������Ӧ����;��β");
}
string block::funt_head(int level){
	int count = 0;
	string name;
	l.getsym();
	if (sym == ident){
		name = t.alloc_function_name(token);
		t.insert(token,name,Funcion, unkown, 0, level);
		l.getsym();
		if (sym == lparen){       //��������
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
			else error(13,"�����ײ�ȱ��:");
		}
		else if (sym == colon){  //û�в�����                
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
			error("����û������");
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
			return token;    //���ʽ
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
	   error("���Ӳ������ַ�");
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
		else error(14,"�����ʽ��ȱ��������");
	}
	else if (sym == ident){
		if (!t.lookup_all(token))
			error("δ����ı�ʶ��");
		temp = sym;
		string s = token;
        kinds k=t.lookup_kind(token);
		type T = t.lookup_type(token);
		if (k == nop)
			error("û�ж���ı���");
		else if (k == Procedure)
			error("���̲����������б�����");
		l.queue();
		 if (sym == lbrack){
			 if (T != Arraychar&&T != Arrayint)
				 error("û�ж�����������");
			l.decache();
			l.getsym();
			op2=expression();
			if (isnum(op2)){
				int num = stringtoint(op2);
				if (num >= t.ret_array_length(s)){
					error("����Խ��");
					cout << "���鳤��Ӧ��С�ڣ�"; cout << t.ret_array_length(s) << endl;
				}
				else if (num < 0)
					error("�����±겻��С��0");
			}
			
			l.getsym();                                            //����ȡֵ
			if (sym == rbrack){
				res = m.alloc();
				
				if (op2.find('.') != -1)    //�Ѿ���ֵ���ԣ���res��������
					op2 = op2.substr(0, op2.find_first_of('.'));
					res = res + "." + "&" + s + "|" + op2;
					m.emit(ASS, "&" + s + "|" + op2, "", res);
					
			//	m.emit(ASS, "&" + s + "|" + op2, "", res);
				kind("factor");
			//	return res + "." + "&" + s + "|" + op2;
				return res;
			}
			else error(15, "����ȱ��]");
		}
		else if (sym == lparen){
			l.decache();
			l.getsym();
			int length=actual_parameter_table(s);
			int fuck = t.ret_funct_pro_val_length(s);
			if (t.ret_funct_pro_val_length(s) != length)
				error("�������ò�������Ӧ����", t.ret_funct_pro_val_length(s));
			m.emit(CALL_F,s,"","");
			kind("factor");
			return "?"+s;                  //���غ�����       
		}
		else {                            //��ʶ��
			sym = temp;
			if (t.lookup_kind(s) == Funcion)
				m.emit(CALL_F,s,"","");
			kind("factor");
			int i;
			//if ((i = t.lookup(s, Const)) != -1)
		    //  return t.ret_const(s);           //����ֱ�ӷ�����ֵ����Ϊ�ַ� ��Ϊaskii��ֵ
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
		error(29,"�������ȱ��end");
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
		error("�Ƿ��ĸ�ֵ�����");
	if (t.lookup_kind(Id) == Const)
		error("�������ܱ���ֵ");
	string op2, temp_alloc;
	l.queue();
	if (sym == becomes){
		if (t.lookup_kind(Id) == Procedure)
			error("���������ܱ���ֵ");
		l.decache();
		l.getsym();
		type T = t.lookup_type(Id);
		if (T != Int&&T != Char)
			error("�Ƿ��ĸ�ֵ��䣬��ֵ�����ֵ����");
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
				error("����Խ��");
				cout << "���鳤��Ӧ��С�ڣ�"; cout << t.ret_array_length(Id) << endl;
			}
			else if (num<0)
			{
				error("�����±겻��С��0");
			}
		}
		l.getsym();
		if (sym == rbrack){
			l.getsym();  
			if (t.lookup(Id, Arraychar) == -1 && t.lookup(Id, Arrayint) == -1)           //����û�ж���
				error(101,"ʹ�õ�����û�ж���");
			if (sym == becomes){
				l.getsym();
				temp_alloc=expression();
				kind("assign sataement");
				//m.emit(ASS_ARR, Id, op2, temp_alloc);
				m.emit(ASS,temp_alloc,"","&"+Id+"|"+op2);
				return;
			}
			else error(20,"��ֵ���ȱ��:=");
		}
		error(14,"��ֵ���������ȱ��]");
	}

	else if (sym == lparen){
		l.decache();
		l.getsym();
		int length=actual_parameter_table(Id);
		if (t.ret_funct_pro_val_length(Id) != length)
			error("���̵��ò�������Ӧ����", t.ret_funct_pro_val_length(Id));
		//if (t.lookup_kind(Id) == Procedure)                            //ֻ���ǹ��̵������
			m.emit(CALL_P, Id, "", "");
	//	else error(39);
		kind("pro call");                      //ֻ���ǹ��̵������
	}
	else {
		if (t.lookup_kind(Id) == Procedure)                            //ֻ���ǹ��̵������
			m.emit(CALL_P,Id,"","");
		else error("�����ǹ��̵������");
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
		error(19,"ʵ�ڲ�������ȱ��,");
	do{
		l.getsym();
		op1 = expression();
		m.emit(PARAMETER, op1, id, inttostring(count++));
		var_count++;
		l.getsym();
	} while (sym == comma);
	if (sym == rparen)
		kind("actual_parameter_table");
	else error(19, "ʵ�ڲ�������ȱ��)");
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
				if (temp.find('.') != -1){   //����
					string t_temp = temp.substr(0,temp.find("."));
					int pos1 = temp.find('&');
					int pos2 = temp.find('|');
					int  length = pos2 - pos1-1;
					string  array_name = temp.substr(pos1+1,length);
					if (t.lookup(array_name, Arraychar) != -1)
						m.emit(PRINTF_C,t_temp, "", "");
					else 	m.emit(PRINTF, t_temp, "", "");
				}
				else  if (t.lookup(temp,Char)!=-1)   //�ַ�
					m.emit(PRINTF_C, temp, "", "");
				else m.emit(PRINTF,temp,"","");	    //����
				l.getsym();
				if (sym == rparen){
					kind("write statement");
					return;
				}
				else error(17,"д�����ȱ��)");
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
			
		else error(17,"д�����ȱ��)");
	}
	else error(18,"д�����ȱ��(");
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
		else error(21,"������ȱ��end");
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
			m.emit(JGEQ, op1, op2, lable1);              //��ʵ������෴������then���
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
			}//�����
			if (sym == elsesym){
				lable2 = m.getlable();
				m.emit(J,"","",lable2);
				m.emit(LABLE, lable1, "", "");
				l.decache();
				l.queue();
				if (sym != semicolon&&sym != endsym){
					l.decache();
					statement();
				}//�����
				kind("if-then-else condition statement");
				m.emit(LABLE, lable2, "", "");
				return;
			}
			m.emit(LABLE, lable1, "", "");
			kind("if-then condition statement");
		}
		else error(30,"�������ȱ��then");
	}
	else error(23,"�������ȱ�ٹ�ϵ�����");
}
void block::case_table(int it,string op1, string lable_end){
	string op2,lable;
	op2=case_constant();//��getsym
	lable = m.getlable();
	//m.emit(JEQL,op1,op2,lable);
	m.insert_emit(it, JEQL, op1, op2, lable);
	if (sym == colon){
		m.emit(LABLE, lable, "", "");
		l.queue();
		if (sym != endsym&&sym!=semicolon){
			l.decache();
			statement();
		}//�����
		
		m.emit(J, "", "",lable_end);      //����дbackֵ
		kind("case table");
	}
	else error(22,"���Ԫ�ر�ȱ��:");
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
		else error(3, "�Ƿ��ĳ���ֵ");
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
					}//�����
					kind("for");
					m.emit(J, "", "", lable1);
					m.emit(LABLE, lable2, "", "");
				}
				else error(25,"for���ȱ��do");
			}
			else error("Ӧ���� downto ���� to ");
		}
		else error(20,"for����еĸ�ֵ���ȱ�٣�=");
	}
	else error(24,"for���ȱ��ѭ������");
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
				else error(27,"ȱ�ٶ����ĸ�ֵ����");
				l.getsym();
			}
			if (sym == rparen)
				kind("read statement");
			else error(28,"�����û����ȷ�� )");
		}
		else error("ȱ�ٶ����ĸ�ֵ����");
	}
	else error(26,"�����read ����β");
}
int block::stringtoint(string s){
	int num=0;
	int length = s.size();
	for (int i = 0; i <length; i++){
		num = num * 10 + s[i] - '0';
	}
	return num;
}
