#pragma once
#include<deque>
#include<map>
#include<string>
#include "table.h"
#include "middlecode.h"
using namespace std;
int TEMP_OFFSET = 10;
string Temp[8]{
	"t0", "t1", "t2", "t3", "t4", "t5","t6","t7"
};
string tr_int[8]{"0","1","2","3","4","5","6","7"};
int count[8] = {0};           //������̭��������̭ʹ�����ٵ� 
int used_tr[3] = { 0, 0, 0 };
enum Temp_R{         //��ʱ�Ĵ���  ȫ�ּĴ���������
	t0, t1, t2, t3, t4,t5,t6,t7, overflow
};
class Generator
{
	string Table_Name;//���ڼ�¼��ǰ���ű������
	table &t;
	Middle &m;
	int str_num = 0;//���ı��
	string str_name;//��������
	int cc=0;
	int fuck = 0;
	enum Op{
		addu, addiu, subu, mult, div, mflo, 
		sw, lw, move, li, la,
		j, jal, jr, beq, bne, bltz, blez, bgtz, bgez, syscall,
		label
	};
	
	//ȫ��������������ȫ�ֱ����ʹ�
	struct Data{
		string id;
		string type;
		string value;
	};
	//mipsָ��
	struct Instruction
	{
		Op op;
		string op1;
		string op2;
		string op3;
	};
	vector<Data> MipsData;
	vector<Instruction> MipsText;
	vector<pair<string,Instruction>>  formal;
	int overflowcount;
	int countmod=0;                   //mod 5
	void code(Op, string, string, string);
	void new_data(string, string, string value = "");
	void addformal(string,Op, string, string, string);//������--��Ԫʽ
	void popformal(string);
	//��ʱ�Ĵ�����
	//$t0������ʹ�ú������ͷ�
	//map<string, Temp_R> tr_st;
	string t_1,t_2;
	list<pair<string,Temp_R>> tr_st;
	map<Temp_R, string> tr_ts;
	list<pair<string, Temp_R>> tr_st_cache;
	map<Temp_R, string> tr_ts_cache;
	map<string, string> var_fact;
	int count[8];           //������̭��������̭ʹ�����ٵ� 
	map<string, int> t_overflow;//�������ʱ�����б�������ʱ������������ջ�ϵĵ�ַ��ƫ������
	void base(int l);     //������ַ���㣺����l�����base�������t8    
	string alloc_t(string name);         //Ϊ����Ϊname�ı�������Ĵ���    
	void push_temp_r();
	void stroe(string);
	Temp_R find_least_use();
	void addcode(Instruction instruction);
	void change_register_state(Temp_R, string);
	string Generator::inttostring(int num);
	bool isnum(string op);
	int stringtoint(string s);
	list<pair<string, Temp_R>>::iterator ret_loc_temp_r(string name);    //����name�ĸ���ʱ�Ĵ����ĵ�����
	void del_loc_temp_r(string name);
	int length_loc_temp_r(list<pair<string, Temp_R>>::iterator);
	void intital_temp_r();  //��ʼ����ʱ�Ĵ�����
	void load(string);//�ѱ�����ֵ��ֵ�����䵽�ļĴ���������������
	void Generator::addbase(string name, int l);
	void Generator::push_var_r();
	int find_longestuse();
public:
	Generator(table &T, Middle &M) :t(T), m(M){};
	void generate();
	void output(string);
	void print(Instruction, ostream&);
};
list<pair<string, Temp_R>>::iterator Generator::ret_loc_temp_r(string name){           //���ر����ĵ�������Ϊ���򷵻�end������
	int i = 0;
	for (list<pair<string, Temp_R>>::iterator it = tr_st.begin(); it != tr_st.end(); it++){
		i++;
		if (it->first == name)
			return it;
	}
	return tr_st.end();
}
int Generator::length_loc_temp_r(list<pair<string, Temp_R>>::iterator temp){       //���ر������ڵ�λ�ã�Ϊ���򷵻�-1
	int i = 0;
	for (list<pair<string, Temp_R>>::iterator it = tr_st.begin(); it != tr_st.end(); it++){
		i++;
		if (it==temp)
			return i;
	}
	return -1;      
}

string Generator::alloc_t(string orginname){
	Temp_R tr;
	string name;
	if (orginname.find('.')!=-1)
		name = orginname.substr(0, orginname.find('.'));
	else name = orginname;
	bool isfunction = false;
	if (name[0] == '?'){         //��������
		int a=name.size();
		string b = name.substr(1,a);
		name = b;
		isfunction = true;
	}
	auto it=ret_loc_temp_r(name);
	if (it==tr_st.end()) {          //�ñ���û��������ʱ�Ĵ���
		tr = find_least_use();
		change_register_state(tr,name);//�鿴�Ĵ���״̬����ռ�ý����������
		tr_st.push_back(make_pair(name,tr));
		count[tr]++;
		if (name[1] != '_'){//����Ǳ����Ļ���Ҫִ��load����
			load(name);
		}
		used_tr[fuck++ % 3] = stringtoint(tr_int[tr]);
		return Temp[tr];             //������ʱ�Ĵ���
	}
	else if (it->second != overflow){     //�Ѿ�������ʱ�Ĵ�������δ���
		count[it->second]++;
		if (isfunction)
			load(name); //���·���ֵ
		used_tr[fuck++ % 3] = stringtoint(tr_int[it->second]);
		return Temp[it->second];
	}
	else{                              //�ñ����Ѿ����������ջ���ػص��±���tr��
		tr = find_least_use();
		count[tr]++;
		t_overflow.erase(t_overflow.find(name));    //��������������¼���Ѿ�д��
		change_register_state(tr, name);
		it->second = tr;
		int offset = -4 * (3 + TEMP_OFFSET + t.ret_addr() + length_loc_temp_r(ret_loc_temp_r(name)));
		if (isfunction)
			load(name);
		else code(lw, Temp[tr], inttostring(offset), "fp");//lw $t_ offset(sp)
		used_tr[fuck++ % 3] = stringtoint(tr_int[tr]);
		return  Temp[tr];
	}
}


Temp_R Generator::find_least_use(){            //����Ϊ�������ʹ�ò�����
int loc;
	do{
		loc = countmod++ % 8;
	} while (used_tr[0] == loc||used_tr[1] == loc||used_tr[2] == loc);
	count[loc] = 0;//����
	switch (loc){
	case 0:return t0;
	case 1:return t1;
	case 2:return t2;
	case 3:return t3;
	case 4:return t4;
	case 5:return t5;
	case 6:return t6;
	case 7:return t7;
	default:
		break;
	}
}

/*
Temp_R Generator::find_least_use(){            //������δʹ��
	int loc;
	bool use = false;
	do{
		if (use)
			count[loc]++;
		loc = find_longestuse();
		use = true;
	} while (used_tr[0] == loc || used_tr[1] == loc || used_tr[2] == loc);
	count[loc] = countmod;//����
	switch (loc){
	case 0:return t0;
	case 1:return t1;
	case 2:return t2;
	case 3:return t3;
	case 4:return t4;
	case 5:return t5;
	case 6:return t6;
	case 7:return t7;
	default:
		break;
	}
}

int  Generator::find_longestuse(){
	int least = count[0];
	int i, loc = 0;
	for (i = 0; i < 8; i++){
		if (least>count[i]){
			least = count[i];
			loc = i;
		}
	}
	return loc;
}
*/
void  Generator::change_register_state(Temp_R tr, string name){
	auto it = tr_ts.find(tr);
	if (it == tr_ts.end()){ //tr��δ��ռ��
		tr_ts[tr] = name;
	}
	else {            //tr��ռ�ã��޸�tr_st,��д��overflow,�ٰ�tr����
		//cout << "name:   " + name + "t.ret_addr()"; cout << t.ret_addr() << endl;
		int offset = -4 * (3 + TEMP_OFFSET + t.ret_addr() + length_loc_temp_r(ret_loc_temp_r(tr_ts[tr])));          //��trд�ض�Ӧ�ļĴ���
		t_overflow[tr_ts[tr]] = offset;
		code(sw, Temp[tr], inttostring(offset), "fp");
 		ret_loc_temp_r(tr_ts[tr])->second = overflow;
		tr_ts[tr] = name;
	}
}
void Generator::intital_temp_r(){
	for (int i = 0; i<8; i++){
		count[i] = 0;
	}
	tr_st.erase(tr_st.begin(),tr_st.end());
	tr_ts.erase(tr_ts.begin(),tr_ts.end());
}
void Generator::base(int l){                           //����l�����base�������t8
	 code(move,"t8","fp","");        // t8 =fp;
	 if (l == 0)
		 return;
	while (l>0){
		code(lw, "t8","-4", "t8");          //t8=s[t8];
		l--;
	}
}
void  Generator::generate(){
	code(j, "main","" ,"");
	int length = m.size();
	for (int i = 0;i<length; i++){
		MidCode it = m.MidCodes[i];
	//	m.print(it, cout);
		if (it.op1 == it.op2&&it.op1[0] == '?'){
			Op temp_op;
			switch (it.op){
			case ADD:temp_op = addu; 
				code(temp_op, "a2", "a3", alloc_t(it.res));
				break;
			case SUB:temp_op = subu;
				code(temp_op, "a2", "a3", alloc_t(it.res)); 
				break;
			case MUL:temp_op = mult;
				code(temp_op, "a2", "a3","");
				code(mflo,alloc_t(it.res),"","");
				break; 
			case DIV:temp_op = div; 
				code(temp_op, "a2", "a3","");
				code(mflo, alloc_t(it.res), "", "");
				break;
			}
			continue;
		}
		switch (it.op){
		case LABLE:
			push_temp_r();
			if (isdigit(it.op1[0]))
				code(label, "label" + it.op1, "", "");
			else if(it.op1=="main"){
				code(label, it.op1, "", "");
				intital_temp_r();
				code(move,"fp","sp","");
				code(sw, "fp", "-4", "sp");           //��sl;
				Table_Name = it.op1;
				t.changetable(Table_Name);         //�������ű�
			//	cout << Table_Name << endl;
			//	t.showlink();
			}
			else{
				intital_temp_r();
				code(label, it.op1, "", "");
				Table_Name = it.op1;
				t.changetable(Table_Name);         //�������ű�
			//	cout << Table_Name << endl;
			//	t.showlink();
				                               //�����ֳ�
				code(sw, "t0", "-16", "fp");
				code(sw, "t1", "-20", "fp");
				code(sw, "t2", "-24", "fp");
				code(sw, "t3", "-28", "fp");
				code(sw, "t4", "-32", "fp");
				code(sw, "t5", "-36", "fp");
				code(sw, "t6", "-40", "fp");
				code(sw, "t7", "-44", "fp");
				code(sw,"a2","-48","fp");
				code(sw,"a3", "-52", "fp");
				code(move, "fp", "sp", "");
				code(sw, "ra", "-12", "fp");
			}
			break;
		case ADD:                                     //op1,op2��Ϊ����  �������Ż�
			if (isnum(it.op1) && isnum(it.op2))
				code(li, alloc_t(it.res), inttostring(stringtoint(it.op1) + stringtoint(it.op2)), "");
			else if (isnum(it.op1))
				code(addiu, alloc_t(it.res), alloc_t(it.op2), it.op1);
			else if (isnum(it.op2))
				code(addiu, alloc_t(it.res), alloc_t(it.op1), it.op2);
			else code(addu, alloc_t(it.op1), alloc_t(it.op2), alloc_t(it.res));
			break;
		case SUB:
			if (isnum(it.op1) && isnum(it.op2))
				code(li, alloc_t(it.res), inttostring(stringtoint(it.op1) - stringtoint(it.op2)), "");
			else if (isnum(it.op1)){//res = op1 - op2;
				code(subu, "zero", alloc_t(it.op2), alloc_t(it.op2));
				code(addiu, alloc_t(it.res), alloc_t(it.op2), it.op1);
			}
			else if (isnum(it.op2) && it.op2[0] != '-') //res=op1-op2
				code(addiu, alloc_t(it.res), alloc_t(it.op1), '-' + it.op2);
			else if (isnum(it.op2) && it.op2[0] == '-')           //res=op1-(-op2)
				code(addiu, alloc_t(it.res), alloc_t(it.op1), inttostring(0 - stringtoint(it.op2)));
			else code(subu, alloc_t(it.op1), alloc_t(it.op2), alloc_t(it.res));
			break;
		case MUL:
			if (isnum(it.op1) && isnum(it.op2))
				code(li, alloc_t(it.res), inttostring(stringtoint(it.op1)*stringtoint(it.op2)), "");
			else if (isnum(it.op1)){
				code(li, "t9", it.op1, "");
				code(mult, "t9", alloc_t(it.op2), "");
				code(mflo, alloc_t(it.res), "", "");
			}
			else if (isnum(it.op2)){
				code(li, "t9", it.op2, "");
				code(mult, alloc_t(it.op1), "t9", "");
				code(mflo, alloc_t(it.res), "", "");
			}
			else{
				code(mult, alloc_t(it.op1), alloc_t(it.op2), "");
				code(mflo, alloc_t(it.res), "", "");
			}
			break;
		case DIV:
			if (isnum(it.op1) && isnum(it.op2))                //todo ����Ϊ0
				code(li, alloc_t(it.res), inttostring(stringtoint(it.op1) / stringtoint(it.op2)), "");
			else if (isnum(it.op1)){
				code(li, "t9", it.op1, "");
				code(div, "t9", alloc_t(it.op2), "");
				code(mflo, alloc_t(it.res), "", "");
			}
			else if (isnum(it.op2)){
				code(li, "t9", it.op2, "");
				code(div, alloc_t(it.op1), "t9", "");
				code(mflo, alloc_t(it.res), "", "");
			}
			else {
				code(div, alloc_t(it.op1), alloc_t(it.op2), "");
				code(mflo, alloc_t(it.res), "", "");
			}
			break;
		case MIN:                    
			if (isnum(it.op1))
			code(li, alloc_t(it.res), inttostring(0 - stringtoint(it.op1)), "");
			else{
				code(subu, "zero", alloc_t(it.op1), alloc_t(it.op1));
				code(move,alloc_t(it.res),alloc_t(it.op1),"");
			}
			break;
		case J:
			push_temp_r();
			if (isdigit(it.res[0]))
				code(j, "label" + it.res, "", "");
			else code(j, it.res, "", "");
			break;
		case JEQL:
			if (!isnum(it.op1)){
				t_1 = alloc_t(it.op1);
				code(move, "v0", t_1, "");
			}
			if (!isnum(it.op2)){
				t_2 = alloc_t(it.op2);
				code(move, "v1", t_2, "");
			}
			push_temp_r();
			if (isnum(it.op1) && isnum(it.op2)){
				code(li, "v0", it.op1, "");
				code(li, "v1", it.op2, "");
			}
			else if (isnum(it.op1)){
				code(li, "v0", it.op1, "");
			}
			else if (isnum(it.op2)){
				code(li, "v1", it.op2, "");
			}
			code(beq, "v0","v1",it.res);
			break;

		case JNEQ:
			if (!isnum(it.op1)){
				t_1 = alloc_t(it.op1);
				code(move, "v0", t_1, "");
			}
			if (!isnum(it.op2)){
				t_2 = alloc_t(it.op2);
				code(move, "v1", t_2, "");
			}
			push_temp_r();
			if (isnum(it.op1) && isnum(it.op2)){
				code(li, "v0", it.op1, "");
				code(li, "v1", it.op2, "");
			}
			else if (isnum(it.op1)){
				code(li, "v0", it.op1, "");
			}
			else if (isnum(it.op2)){
				code(li, "v1", it.op2, "");
			}
			code(bne, "v0", "v1", it.res);
			break;
		case JLSS:
			if (!isnum(it.op1)){
				t_1 = alloc_t(it.op1);
				code(move, "v0", t_1, "");
			}
			if (!isnum(it.op2)){
				t_2 = alloc_t(it.op2);
				code(move, "v1", t_2, "");
			}
			push_temp_r();
			if (isnum(it.op1) && isnum(it.op2)){
				code(li, "v0", it.op1, "");
				code(li, "v1", it.op2, "");
			}
			else if (isnum(it.op1)){
				code(li, "v0", it.op1, "");
			}
			else if (isnum(it.op2)){
				code(li, "v1", it.op2, "");
			}
			code(subu, "v0", "v1","v0");
			code(bltz, "v0",it.res,"");
			break;
		case JLEQ:
			if (!isnum(it.op1)){
				t_1 = alloc_t(it.op1);
				code(move, "v0", t_1, "");
			}
			if (!isnum(it.op2)){
				t_2 = alloc_t(it.op2);
				code(move, "v1", t_2, "");
			}
			push_temp_r();
			if (isnum(it.op1) && isnum(it.op2)){
				code(li, "v0", it.op1, "");
				code(li, "v1", it.op2, "");
			}
			else if (isnum(it.op1)){
				code(li, "v0", it.op1, "");
			}
			else if (isnum(it.op2)){
				code(li, "v1", it.op2, "");
			}
			code(subu, "v0", "v1", "v0");
			code(blez, "v0",it.res,"" );
			break;
		case JGRE:
			if (!isnum(it.op1)){
				t_1 = alloc_t(it.op1);
				code(move, "v0", t_1, "");
			}
			if (!isnum(it.op2)){
				t_2 = alloc_t(it.op2);
				code(move, "v1", t_2, "");
			}
			push_temp_r();
			if (isnum(it.op1) && isnum(it.op2)){
				code(li, "v0", it.op1, "");
				code(li, "v1", it.op2, "");
			}
			else if (isnum(it.op1)){
				code(li, "v0", it.op1, "");
			}
			else if (isnum(it.op2)){
				code(li, "v1", it.op2, "");
			}
			code(subu, "v0", "v1", "v0");
			code(bgtz, "v0",it.res,"");
			break;
		case JGEQ:
			if (!isnum(it.op1)){
				t_1 = alloc_t(it.op1);
				code(move, "v0", t_1, "");
			}
			if (!isnum(it.op2)){
				t_2 = alloc_t(it.op2);
				code(move, "v1", t_2, "");
			}
			push_temp_r();
			if (isnum(it.op1) && isnum(it.op2)){
				code(li, "v0", it.op1, "");
				code(li, "v1", it.op2, "");
			}
			else if (isnum(it.op1)){
				code(li, "v0", it.op1, "");
			}
			else if (isnum(it.op2)){
				code(li, "v1", it.op2, "");
			}
			code(subu, "v0", "v1", "v0");
			code(bgez, "v0",it.res,"");
			break;
		case ASS:
			if (isnum(it.op1)){
				code(li, alloc_t(it.res), it.op1, "");
			}
			else code(move, alloc_t(it.res), alloc_t(it.op1), "");
			if (it.res.find('.') != -1)  {     //�����������飬д������ջ
				it.res = it.res.substr(0, it.res.find('.'));
			}
		    if (it.res.find('&')!=-1)       //�����������飬д������ջ
				stroe(it.res);
			break;
		case SCANF:{
					   type ty = t.lookup_type(it.op1);
					   if (ty == Int){     // ��int
						   code(li, "v0", "5", "");
						   code(syscall, "", "", "");
						   code(move, alloc_t(it.op1), "v0", "");
					   }
					   else if (ty == Char){ // read char
						   code(li, "v0", "12", "");
						   code(syscall, "", "", "");
						   code(move, alloc_t(it.op1), "v0", "");
					   }
					   break; }
		case PRINTF:
			if (isnum(it.op1))
				code(li, "a0", it.op1, "");
			else code(move, "a0", alloc_t(it.op1), "");
			code(li, "v0", "1", "");
			code(syscall, "", "", "");
			break;
		case PRINTF_S:
			str_name = "str" + inttostring(++str_num);
			new_data(str_name, "ASCIIZ", it.op1);
			code(la, "a0", str_name, "");
			code(li, "v0", "4", "");
			code(syscall, "", "", "");
			break;
		case PRINTF_C:
			code(move, "a0", alloc_t(it.op1), "");
			code(li, "v0", "11", "");
			code(syscall, "", "", "");
			break;
		case RETURN:
			if (Table_Name != "main"){           //��main������Ҫ�ָ��ϸ������ֳ�
				push_temp_r();//��ռĴ�����
				code(lw, "ra", "-12", "fp");     //ȡ��ra
				code(addiu, "sp", "fp", "4");        //sp=fp+4;��ջ
				code(lw, "fp", "-8", "fp");          //ȡǰһ����fp  dl;
				code(lw, "t0", "-16", "fp");
				code(lw, "t1", "-20", "fp");
				code(lw, "t2", "-24", "fp");
				code(lw, "t3", "-28", "fp");
				code(lw, "t4", "-32", "fp");
				code(lw, "t5", "-36", "fp");
				code(lw, "t6", "-40", "fp");
				code(lw, "t7", "-44", "fp");
				code(lw, "a2", "-48", "fp");
				code(lw, "a3", "-52", "fp");
				code(jr, "ra", "", "");
			}
			break;
		/*
		case PARAMETER:
			if (stringtoint(it.op2)==1){      //���ռ� 
				int 	offset = -4 * (3 + TEMP_OFFSET + t.ret_addr() + tr_st.size());
				code(addiu, "sp", "fp", inttostring(offset));
			}
			if (it.op1.find('.') != -1){   //���ε������飬����������ֵ��������ʱ�Ĵ�����
				it.op1 = it.op1.substr(it.op1.find('.') + 1, it.op1.size());
			}
			if (isnum(it.op1)){         //���ε�����
				int offset = -4 * (3 + TEMP_OFFSET + stringtoint(it.res));
				addformal(it.op2,li, "t8", inttostring(offset),"");
				addformal(it.op2,addu, "t8", "sp", "t8");
				addformal(it.op2,li,"t9",it.op1,"");
				addformal('+'+it.op2,sw,"t9","","t8");
			}
			else if (t.lookup_formal(it.op2, stringtoint(it.res)) && (it.op1.find('&') != -1)){//var ���������
				it.op1 = it.op1.substr(it.op1.find('.') + 1);
				int pos1 = it.op1.find('&');
				int pos2 = it.op1.find('|');
				string array_name = it.op1.substr(pos1+1,pos2-pos1 - 1);
				string array_offset = it.op1.substr(pos2 + 1);
				stroe(it.op1);
				//addbase(t.ret_offset_l(array_name,false));
				int offset = -4 * (3 + TEMP_OFFSET + stringtoint(it.res));
				if (isnum(array_offset)){
					int varoffset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false) + stringtoint(array_offset));       //��var������ͷ��ַ
					addbase(it.op2, t.ret_offset_l(array_name, false));
					addformal(it.op2,addiu, "t8", "t8", inttostring(varoffset));
					addformal('+' + it.op2, sw, "t8", inttostring(offset), "sp");
				}
				else{                                                                   //offostҪ��4              
					int varoffset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false));
					addformal(it.op2, li, "t9", "4", "");
					addformal(it.op2, mult, alloc_t(array_offset), "t9", "");
					addformal(it.op2, mflo, "t9", "", "");
					addformal(it.op2, subu, "zero", "t9", "t9");
					addbase(it.op2, t.ret_offset_l(array_name, false));
					addformal(it.op2, addiu, "t8", "t8", inttostring(varoffset));
					addformal(it.op2, addu, "t8", "t9", "t8");
					addformal('+' + it.op2, sw, "t8", inttostring(offset), "sp");
				}
			}
			else if (it.op1[0]=='&'){//����������
				int pos = it.op1.find('|');
				string array_name = it.op1.substr(1, pos - 1);
				string array_offset = it.op1.substr(pos + 1, it.op1.size());
				if (isnum(array_offset)){
					int offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false) + stringtoint(array_offset));
					addformal('+' + it.op2, sw, alloc_t(it.op1), inttostring(offset), "sp");
				}
				else{
					int offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false));
					addformal(it.op2, addiu, alloc_t(array_offset), alloc_t(array_offset), inttostring(offset));            //����������ַ
					addformal('+' + it.op2, sw, alloc_t(it.op1), inttostring(offset), "sp");
				}
			}
			else if (t.lookup_formal(it.op2, stringtoint(it.res)) && t.lookup_formal(it.op1)){       //����var������var����
				int offset = -4 * (3 + TEMP_OFFSET + stringtoint(it.res));
				//addformal(lw,alloc_t(it.op1),inttostring(offset), "fp");
				//addformal(sw, alloc_t(it.op1), inttostring(offset), "sp");
				addformal(it.op2, lw, "s1", inttostring(offset), "fp");
				addformal('+' + it.op2, sw, "s1", inttostring(offset), "sp");
			}
			else if (t.lookup_formal(it.op2, stringtoint(it.res))){     //�������var����
				if (it.op1[1] == '_')                     //var����ʱ����
				error(106,"var�βα����Ǳ���");
				if (it.op1.find('.') != -1)
					it.op1 = it.op1.substr(it.op1.find('.') + 1);
				stroe(it.op1);           
				addbase(it.op2, t.ret_offset_l(it.op1, true));
				int varoffset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(it.op1, true));       //��var�ĵ�ַ
				addformal(it.op2, addiu, "t8", "t8", inttostring(varoffset));
				int offset = -4 * (3 + TEMP_OFFSET + stringtoint(it.res));
				addformal('+' + it.op2, sw, "t8", inttostring(offset), "sp");
			}
			else {                                                     //��������
				int offset = -4 * (3 + TEMP_OFFSET + stringtoint(it.res));
				addformal('+' + it.op2, sw, alloc_t(it.op1), inttostring(offset), "sp");
			}
			break;
			*/
			case PARAMETER:
				if (stringtoint(it.res) == 1){      //���ռ� 
					int 	offset = -4 * (3 + TEMP_OFFSET + t.ret_addr() + tr_st.size());
					code(addiu, "sp", "fp", inttostring(offset));
				}
				if (it.op1.find('.') != -1){   //���ε������飬����������ֵ��������ʱ�Ĵ�����
					it.op1 = it.op1.substr(it.op1.find('.') + 1, it.op1.size());
				}
				if (isnum(it.op1)){         //���ε�����
					int offset = -4 * (3 + TEMP_OFFSET + stringtoint(it.res));
					code(li, "t8", inttostring(offset), "");
					code(addu, "t8", "sp", "t8");
					code(li, "t9", it.op1, "");
					code(sw, "t9", "", "t8");
				}
				else if (t.lookup_formal(it.op2, stringtoint(it.res)) && (it.op1.find('&') != -1)){//var ���������
					it.op1 = it.op1.substr(it.op1.find('.') + 1);
					int pos1 = it.op1.find('&');
					int pos2 = it.op1.find('|');
					string array_name = it.op1.substr(pos1 + 1, pos2 - pos1 - 1);
					string array_offset = it.op1.substr(pos2 + 1);
					stroe(it.op1);
					//addbase(t.ret_offset_l(array_name,false));
					int offset = -4 * (3 + TEMP_OFFSET + stringtoint(it.res));
					if (isnum(array_offset)){
						int varoffset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false) + stringtoint(array_offset));       //��var������ͷ��ַ
						base(t.ret_offset_l(array_name, false));
						code(addiu, "t8", "t8", inttostring(varoffset));
						code(sw, "t8", inttostring(offset), "sp");
					}
					else{                                                                   //offostҪ��4              
						int varoffset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false));
						code(li, "t9", "4", "");
						code(mult, alloc_t(array_offset), "t9", "");
						code(mflo, "t9", "", "");
						code(subu, "zero", "t9", "t9");
						base(t.ret_offset_l(array_name, false));
						code(addiu, "t8", "t8", inttostring(varoffset));
						code(addu, "t8", "t9", "t8");
						code(sw, "t8", inttostring(offset), "sp");
					}
				}
				else if (it.op1[0] == '&'){//����������
					int pos = it.op1.find('|');
					string array_name = it.op1.substr(1, pos - 1);
					string array_offset = it.op1.substr(pos + 1, it.op1.size());
					if (isnum(array_offset)){
						int offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false) + stringtoint(array_offset));
						code(sw, alloc_t(it.op1), inttostring(offset), "sp");
					}
					else{
						int offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false));
						code(addiu, alloc_t(array_offset), alloc_t(array_offset), inttostring(offset));            //����������ַ
						code(sw, alloc_t(it.op1), inttostring(offset), "sp");
					}
				}
				else if (t.lookup_formal(it.op2, stringtoint(it.res)) && t.lookup_formal(it.op1)){       //����var������var����
					int offset = -4 * (3 + TEMP_OFFSET + stringtoint(it.res));
					//addformal(lw,alloc_t(it.op1),inttostring(offset), "fp");
					//addformal(sw, alloc_t(it.op1), inttostring(offset), "sp");
					code(lw, "s1", inttostring(offset), "fp");
					code(sw, "s1", inttostring(offset), "sp");
				}
				else if (t.lookup_formal(it.op2, stringtoint(it.res))){     //�������var����
					if (it.op1[1] == '_')                     //var����ʱ����
						error(106, "var�βα����Ǳ���");
					if (it.op1.find('.') != -1)
						it.op1 = it.op1.substr(it.op1.find('.') + 1);
					stroe(it.op1);
					base(t.ret_offset_l(it.op1, true));
					int varoffset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(it.op1, true));       //��var�ĵ�ַ
					code(addiu, "t8", "t8", inttostring(varoffset));
					int offset = -4 * (3 + TEMP_OFFSET + stringtoint(it.res));
					code(sw, "t8", inttostring(offset), "sp");
				}
				else {                                                     //��������
					int offset = -4 * (3 + TEMP_OFFSET + stringtoint(it.res));
					code(sw, alloc_t(it.op1), inttostring(offset), "sp");
				}
				break;
		case CALL_F:{
						if (t.ret_funct_pro_val_length(it.op1) == 0){
							int 	offset = -4 * (3 + TEMP_OFFSET + t.ret_addr() + tr_st.size());
							code(addiu, "sp", "fp", inttostring(offset));
						}       
					 //  popformal(it.op1);           //����
					   int l = t.ret_function_offset_l(it.op1);
					  // push_temp_r();         //�����ʱ�Ĵ�����
					   push_var_r();//����д��ջ�ռ�
					   base(l);
					   code(sw, "t8", "-4", "sp");             //sl
					   code(sw, "fp", "-8", "sp");                      //dl
					   code(jal, t.function_rename(it.op1), "", "");
					   code(move, "a2", "a3", "");
					   int  offset = -4 * (3 + TEMP_OFFSET + t.ret_funct_addr());
					   code(move, "t8", "fp","");
					   code(lw, "t8", "-4", "t8");
					   code(lw, "a3", inttostring(offset), "t8");     //���뷵��ֵ
					   //	code(jal,it.op1, "", "");
					   break; }
		case CALL_P:{
						if (t.ret_funct_pro_val_length(it.op1) == 0){
							int 	offset = -4 * (3 + TEMP_OFFSET + t.ret_addr() + tr_st.size());
							code(addiu, "sp", "fp", inttostring(offset));
						}
						//popformal(it.op1);           //����
						int l = t.ret_function_offset_l(it.op1);
						//push_temp_r();         //�����ʱ�Ĵ�����
						push_var_r();//����д��ջ�ռ�
						base(l);
						code(sw, "t8", "-4", "sp");             //sl
						code(sw, "fp", "-8", "sp");                      //dl
						code(jal,t.function_rename(it.op1), "", "");
					//	code(jal,it.op1, "", "");
						break; }
		default:
			break;
		}
	}
}
void Generator::addbase(string name, int l){                           //����l�����base�������t8
	addformal(name, move, "t8", "fp", "");        // t8 =fp;
	if (l == 0)
		return;
	while (l>0){
		addformal(name, lw, "t8", "-4", "t8");          //t8=s[t8];
		l--;
	}
}

void Generator::load(string name){
	int pos,offset;
	string array_name, array_offset;
	if (t.lookup_kind(name)==Const)  //���볣��
	  code(li,alloc_t(name),t.ret_const(name),"");           //����ֱ�ӷ�����ֵ����Ϊ�ַ� ��Ϊaskii��ֵ 
	else if (name[0] == '&'){                       //��������
		pos = name.find('|');
		array_name = name.substr(1, pos - 1);
		array_offset = name.substr(pos + 1, name.size());
		offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false));
		if (isnum(array_offset)){
			offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false) + stringtoint(array_offset));
			base(t.ret_offset_l(array_name, false));
			code(lw, alloc_t(name),inttostring(offset), "t8");
		}
		else {
			code(li, "t9", "4", "");
			code(mult, alloc_t(array_offset), "t9", "");
			code(mflo, "t9", "", "");
			code(subu, "zero", "t9", "t9");
			code(addiu, "t9", "t9", inttostring(offset));            //����������ַ
			//	code(move, "t8", alloc_t(array_offset), "");
			base(t.ret_offset_l(array_name, false));
			code(addu, "t8", "t9", "t8");
			code(lw, alloc_t(name),"", "t8");
		}
	}
	else if (t.lookup_formal(name)){ //��ͨvar����
		offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(name, true));
	code(lw, "s1", inttostring(offset), "fp");
	code(lw, alloc_t(name),"","s1");
	}
	else {                                 //������ͨ����
		base(t.ret_offset_l(name, true));
		offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(name, true));
		code(lw, alloc_t(name), inttostring(offset), "t8");
	}
}
void Generator::stroe(string orginname){         //�ѱ���д��ջ�ռ�
	int pos,offset;
	string array_name, array_offset;
	string name;
	if (t.lookup_kind(orginname)==Const)  //��������
		return;
	if (orginname.find('.') != -1)
		name = orginname.substr(orginname.find('.')+1);
	else name = orginname;
	if (name[0] == '&'){                       //��������
		pos = name.find('|');
		array_name = name.substr(1, pos - 1);
		array_offset = name.substr(pos + 1);
		offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false));
		if (isnum(array_offset)){
			offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(array_name, false) + stringtoint(array_offset));
			base(t.ret_offset_l(array_name, false));
			code(sw, alloc_t(name), inttostring(offset), "t8");
		}
	    else{
			code(li, "t9", "4", "");
			code(mult, alloc_t(array_offset), "t9", "");
			code(mflo, "t9", "", "");
			code(subu, "zero", "t9", "t9");
			code(addiu,"t9","t9", inttostring(offset));            //����������ַ
		//	code(move, "t8", alloc_t(array_offset), "");
			base(t.ret_offset_l(array_name, false));
			code(addu, "t8","t9", "t8");
			code(sw, alloc_t(name),"", "t8");
	}
	}
	else if (t.lookup_formal(name)){ //var����
		offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(name, true));
	code(lw,"s1",inttostring(offset),"fp");
	code(sw, alloc_t(name), "", "s1");
	}
	else{
		offset = -4 * (3 + TEMP_OFFSET + t.ret_addr_var(name, true));
		base(t.ret_offset_l(name, true));
		code(sw, alloc_t(name), inttostring(offset), "t8");
	}
}

void Generator::push_temp_r(){              //����ʱ�Ĵ�����д��ջ�ռ�
	int i = 1;
	for (auto it = tr_st.begin(); it != tr_st.end(); it++){
		if (it->second != overflow && (t.lookup_kind(it->first)== Const)){
			int offset = -4 * (3 + TEMP_OFFSET + t.ret_addr() + i);
			code(sw, Temp[it->second], inttostring(offset), "fp");
		}
		if (it->first[1] != '_')    //�Ǳ���
			stroe(it->first);
		i++;
	}
	intital_temp_r();
}
void Generator::push_var_r(){              //�ѱ���д��ջ�ռ�
	for (auto it = tr_st.begin(); it != tr_st.end(); it++){  //������ʱ�Ĵ���
		if (it->first[1] == '_')
			tr_st_cache.push_back(make_pair(it->first,it->second));
	}
	for (auto it = tr_ts.begin(); it != tr_ts.end(); it++){
		if (it->second[1] == '_')
			tr_ts_cache[it->first] = it->second;
	}
	int i = 1;
	for (auto it = tr_st.begin(); it != tr_st.end(); it++){       //��ջ
		if (it->second != overflow && (t.lookup_kind(it->first) == Const)){
			int offset = -4 * (3 + TEMP_OFFSET + t.ret_addr() + i);
			code(sw, Temp[it->second], inttostring(offset), "fp");
		}
		if (it->first[1] != '_')    //�Ǳ���
			stroe(it->first);
		i++;
	}
	tr_st.clear();
	tr_ts.clear();
	for (auto it = tr_st_cache.begin(); it != tr_st_cache.end(); it++){  //д��
		if (it->first[1] == '_')
			tr_st.push_back(make_pair(it->first, it->second));
	}
	for (auto it = tr_ts_cache.begin(); it != tr_ts_cache.end(); it++){
		if (it->second[1] == '_')
			tr_ts[it->first] = it->second;
	}
	tr_st_cache.clear();
	tr_ts_cache.clear();
}
void Generator::del_loc_temp_r(string name){
	for (auto it = tr_st.begin(); it != tr_st.end(); it++){
		if (it->first == name){
			if(tr_st.size()!=1)
				tr_st.erase(it);
			else tr_st.clear();
			break;
		}
	}
}
void Generator::new_data(string id, string type, string value)
{
	Data data;
	data.id = id;
	data.type = type;
	data.value = value;
	MipsData.push_back(data);
}
void Generator::code(Op op, string op1, string op2, string op3)
{
	Instruction instruction;
	instruction.op = op;
	instruction.op1 = op1;
	instruction.op2 = op2;
	instruction.op3 = op3;
//	printf("%d  ",++cc);
//	print(instruction, cout);
	MipsText.push_back(instruction);
}
void Generator::addcode(Instruction instruction)
{
	//print(instruction, cout);
	MipsText.push_back(instruction);
}
void Generator::addformal(string name,Op op, string op1, string op2, string op3)
{
	Instruction instruction;
	instruction.op = op;
	instruction.op1 = op1;
	instruction.op2 = op2;
	instruction.op3 = op3;
	formal.push_back(make_pair(name, instruction));
}
void Generator::popformal(string name){
	string function_name;
	int length = t.ret_funct_pro_val_length(name);
	while (true){
		auto it = formal.end() - 1;
		if (it->first[0] == '+'){
			if (length == 0)
				break;
			function_name = it->first.substr(1);
			length--;
		}
		if (it->first != name)
			error("������������");
		addcode(it->second);
		formal.pop_back();
		if (formal.size() == 0)
			break;
	}
}
void Generator::output(string file_name){
	//1.���������
	ofstream fout;
	fout.open(file_name+".s");
	if (!fout)
		cout << "�޷����ļ�mips.S" << endl;
	//2.������ݶ���ָ��
	fout << ".data" << endl;
//	cout << ".data" << endl;
	for (unsigned i = 0; i < MipsData.size(); i++){
		Data& data = MipsData[i];
		fout << data.id << ":" << "\t." << data.type;
		if (data.value != ""){
			if (data.type == "ASCIIZ"){
				fout << "\t\"" << data.value << "\"";
		//		cout << "\t\"" << data.value << "\"";
			}
			else{ 
				fout << "\t" << data.value; 
		//		cout << "\t" << data.value;
			}
		}
		fout << endl;
	}
	fout << endl;
	//3.��������ָ��
	fout << ".text" << endl;
//	cout << ".text" << endl;
	for (unsigned i = 0; i < MipsText.size(); i++)
	{
		Instruction is = MipsText[i];
		print(is, fout);
//		print(is, cout);
	}
	fout.close();
}

void Generator::print(Instruction is, ostream &os)
{
	string Op_str[] =
	{ "addu", "addiu", "subu", "mult", "div", "mflo", 
	"sw", "lw", "move", "li", "la", 
	"j", "jal", "jr", "beq", "bne", "bltz", "blez", "bgtz", "bgez", "syscall",
	"label"
	};
	switch (is.op)
	{
	case mflo:
	case jr:
		os << Op_str[is.op] << "\t$" << is.op1 << endl;
		break;
	case sw:
	case lw:
		os << Op_str[is.op] << "\t$" << is.op1 << "," << is.op2 << "($" << is.op3 << ")" << endl;
		break;
	case mult:
	case div:
	case move:
		os << Op_str[is.op] << "\t$" << is.op1 << ",$" << is.op2 << endl;
		break;
	case li:
	case la:
		os << Op_str[is.op] << "\t$" << is.op1 << "," << is.op2 << endl;
		break;
	case bltz:
	case blez:
	case bgtz:
	case bgez:
		os << Op_str[is.op] << "\t$" << is.op1 << "," << "label"+is.op2 << endl;
		break;
	case beq:
	case bne:
		os << Op_str[is.op] << "\t$" << is.op1 << ",$" << is.op2 << ",label" << is.op3 << endl;
		break;
	case j:
	case jal:
		os << Op_str[is.op] << "\t" << is.op1 << endl;
		break;
	case addiu:
		os << Op_str[is.op] << "\t$" << is.op1 << ",$" << is.op2 << "," << is.op3 << endl;
		break;
	case syscall:
		os << "syscall" << endl;
		break;
	case label:
		os << is.op1 + ":" << endl;
		break;
	case addu:
	case subu:
		os << Op_str[is.op] << "\t$" << is.op3<< ",$" << is.op1<< ",$" << is.op2<< endl;              //֮ǰ������  ���� 
		break;
	default:cout << "�޷�ʶ���MIPSָ��" << endl;
	}
}
bool Generator::isnum(string op){
	if (isdigit(op[0]) || op[0] == '-')
		return true;
	else return false;
}
int Generator::stringtoint(string s){
	int i,num = 0;
	if (s[0] == '-'){
		for (i = 1; i < s.size(); i++){
			num = s[i] - '0' + num * 10;
		}
		return 0 - num;
	}
	else
	for (i = 0; i < s.size(); i++){
		num = s[i]-'0' + num * 10;
	}
	return num;
}
string Generator::inttostring(int num)
{
	strstream ss;
	string s;
	ss << num;
	ss >> s;
	return s;
}