
#include"table.h"
string Kind[4] = { "Const", " Var", "Procedure", "Funcion"};
string Type[6] = { "Int", "Char", "Arrayint","Arraychar","unknow" };
extern int line;
extern string token;
extern int errorcount;
extern string line_cache1, line_cache2;
void table::insert(string name, kinds k, type t, int val, int level){              //top是栈顶
	redefine_cheak(name, k, t, function_name,stack.size()-1);
	temp.name = name;
	temp.k = k;
	temp.level = level;
	temp.t = t;
	temp.val = val;
	temp.val_formal = false;
	temp.function_filed = function_name;
	if (k==Var||k==Funcion)
	temp.addr = Addr++;
	else temp.addr =Addr-1;
	stack[++top] = temp;
	if (t == Arraychar || t == Arrayint)
		Addr += val - 1;        //-1是应为数组本身也是var,已经加过1了 
}
void table::insert(string name,string rename, kinds k, type t, int val, int level){              //top是栈顶
	redefine_cheak(name, k, t, function_name,stack.size()-1);
	temp.name = name;
	temp.k = k;
	temp.level = level;
	temp.t = t;
	temp.val = val;
	temp.val_formal = false;
	temp.rename_function = rename;
	temp.function_filed = function_name;
	if (k == Var || k == Funcion)
		temp.addr = Addr++;
	else temp.addr = Addr - 1;
	stack[++top] = temp;
}
void table::insertnop(string functionname, int level){              //top是栈顶
	temp.name ="_nop";
	temp.k =nop;
	temp.level = level;
	temp.t = unkown;
	temp.val =0;
	temp.val_formal = false;
	temp.function_filed = function_name+"/"+functionname;
	stack[++top] = temp;
}
void table::insertTV(type t, int val) {  //插入类型/值
	redefine_cheak(stack[top].name, stack[top].k, stack[top].t, function_name,stack.size()-2);
	stack[top].t = t;
	stack[top].val = val;
}
void table::insertT(type t) {
	redefine_cheak(stack[top].name, stack[top].k,t, function_name,stack.size()-2);
	stack[top].t = t;
}
void table::insertV_back(int val,int back) {
	stack[top-back].val= val;
}
void table::insertTV_back(type t,int val, int back) {
	redefine_cheak(stack[top-back].name, stack[top-back].k,t, function_name,stack.size()-2-back);
	stack[top - back ].t = t;
	stack[top - back ].val = val;
}
void table::show(){
	int length = stack.size();
	for (int i = 0; i < length; i++){
		auto it = stack.find(i);
		temp = it->second;
		printf("%d:  n:%s\tk:%s\tt:%s\tv:%d\tl:%d\ta:%d\tf_filed: %s\n",i, temp.name.c_str(), Kind[temp.k].c_str(), Type[temp.t].c_str(), temp.val, temp.level,temp.addr,temp.function_filed.c_str());
	}
	     
}
void table::insertT_Back(int length,type t){
	for (int i = 0; i < length; i++){
		redefine_cheak(stack[top - length + i + 1].name, stack[top - length + i + 1].k, t, stack[top - length + i + 1].function_filed, stack.size() - 2 - length + i + 1);
		stack[top - length + i + 1].t = t;
	}
}
void table::insert_formal_back(int count){
	for (int i = 0; i <count; i++){
		stack[top- count + i + 1].val_formal= true;
	}
}
void table::insertT_Back2(int length, type t,int val){
	for (int i = 0; i < length; i++){
		redefine_cheak(stack[top - length + i + 1].name, stack[top - length + i + 1].k, t, stack[top - length + i + 1].function_filed, stack.size() - 2 - length + i + 1);
		stack[top - length + i + 1].t = t;
		stack[top - length + i + 1].val = val;
	}
	if (t == Arraychar || t == Arrayint)
		Addr += val-1;
}
flame table::find(int i){
	auto it = stack.find(i);
	return temp = it->second;
}
void table::pop(string popname){
	   /*int level = link.back().level;
		while (link.back().level == level){            //
			link.pop_back();
			if (link.size() == 0)
				break;
		}
		*/                                   //退栈策略错误
	if (link.size() == 0)
		return;
	while (cur_function(link.back().function_filed)==popname){            //
	link.pop_back();
	if (link.size() == 0)
	break;
	}
	sub_curname(popname);
		if (link.size()!=0)
		Addr = link.back().addr;
//		cout << "pop" << endl;
//		showlink();
}
void table::push(int level){
	Block temp;
	//int lev=1;
	if (begin == top){
	//	cout << "nothing push" << endl;
		return;
	}
	temp.begin = begin + 1;
	temp.end = top;
	temp.level = level;
	temp.addr = Addr;
	temp.function_filed = function_name;
	begin = top;
	link.push_back(temp);
	use();
//	cout << "push" << endl;
//	showlink();
/*	if (link.size() != 0)
		lev = link.back().level;
	if (level != lev){
		Addr = 1;
	}
	*/
}
void table::showlink(){
	for (int i = link.size()-1; i>=0; i--){
	printf("%d     %d      %d           %d               %s\n", link[i].begin, link[i].end,link[i].level,link[i].addr,link[i].function_filed.c_str());
	}
	cout << endl;
}
void table::storetable(string name){
	Lqueue[name] = link;
//	cout << "functionname:     " + name+"          absolutename"+function_name << endl;
//	showlink();
}
void table::showqueue(){
	map<string, vector<Block>>::iterator it;
	for (it = Lqueue.begin(); it != Lqueue.end(); ++it){
		link = it->second;
		printf("%s\n", it->first.c_str());
		showlink();
	}	//printf("%s\n", it->first.c_str())
}
void table::resetval(){
	Addr = 1;
}

int table::lookup(string name,kinds k){
	use();
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name == name&&stack[i].k == k&&usecondition[i])
			return i;
	}
	return -1;
}
int table::lookup(string name, type t){
	use();
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name == name&&stack[i].t == t&&usecondition[i])
			return i;
	}
	return -1;
}

/*kinds table::lookup_kind(string name){
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name == name&&usecondition[i])
			return stack[i].k;
	}
	cout << "find:" + name << endl;
	error(103);
	//return unkown;
}*/
type table::lookup_type(string name){
	use();
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name == name && (stack[i].t == Int || stack[i].t ==Char|| stack[i].t == Arraychar ||stack[i].t == Arrayint) && usecondition[i])
			return stack[i].t;
	}
	// error(104);
	return unkown;
}
bool table::lookup_all(string name){
	use();
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name == name)
			return true;
	}
	return false;
}
kinds table::lookup_kind(string name){
	use();
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name == name && usecondition[i])
			return stack[i].k;
	}
	return nop;
}
bool table::lookup_formal(string name){        
	use();
	int length = stack.size() - 1;
	for (int i = length; i >=0; i--){
		if (stack[i].name == name &&stack[i].val_formal && (stack[i].k == Var) && usecondition[i]){
		
			return true;
		}
	}
	return false;
}
bool table::lookup_formal(string funtion_id, int offset){
	int level = link[link.size() - 1].level;
	use();
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name == funtion_id&&(stack[i].k == Procedure||stack[i].k==Funcion) && usecondition[i]&&stack[i+offset].val_formal)
			return true;
	}
	return false;
}
int table::lookup_var(string name){
	use();
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name == name&&stack[i].k==Var&&usecondition[i])
			return i;
	}
	error(104);
	//return unkown;
}
void table::use(){
	for (int i = 0; i < stack.size(); i++){
		if (isin(i))
			usecondition[i] = true;
		else usecondition[i] = false;
	}
	if (begin<top)
	for (int i = begin+1; i <=top;i++)
		usecondition[i] = true;
}
bool table::isin(int item){
	for (int i = 0; i < link.size(); i++){
		if (item >= link[i].begin&&item <= link[i].end)
			return true;
	}
	return false;
}
void table::error(int n){
	printf("number:%d\n", ++errorcount);
	printf("error   %d       line:%d\n", n, line);
	cout << "token:" + token << endl;
	cout << line_cache2 << endl;
	cout << line_cache1 << endl;
	exit(1);
}
string table::ret_const(string name){
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name == name&&stack[i].k == Const&&usecondition[i])
			return inttostring(stack[i].val);
	}
	error(104);
	//return unkown;
}
int table::ret_array_length(string s){
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name ==s&&(stack[i].t == Arraychar||stack[i].t==Arrayint)&&usecondition[i])
			return stack[i].val;
	}
}
void table::changetable(string name){             //更新usecondition
	if (Lqueue.find(name) == Lqueue.end()){
		cout << "tabelname:" + name << endl;
		error(105);
}
	link = Lqueue[name];
	use();
	;
}
int table::ret_addr(){
	if (link.size() == 0)
		return 0;
	else 
	return link[link.size()-1].addr;
}
int table::ret_offset_l(string name,bool base){           //返回相对层数        base true --int/char  false array
	int l = link[link.size() - 1].level;         //当前层数
	int length = stack.size() - 1;
	if (base){
		for (int i = length; i >= 0; i--){
			if (stack[i].name == name && (stack[i].t == Int || stack[i].t == Char) && usecondition[i])
				return l - stack[i].level;
		}
	}
	else //数组
	for (int i = length; i >=0; i--){
		if (stack[i].name == name && (stack[i].t == Arraychar || stack[i].t == Arrayint) && usecondition[i])
			return l - stack[i].level;
	}
	cout << "find:" + name << endl;
	error(1041);
 }
int table::ret_function_offset_l(string name){           //返回相对层数        base true --int/char  false array
	int l = link[link.size() - 1].level;         //当前层数
	int length = stack.size() - 1;
	for (int i = length; i >= 0; i--){
		if (stack[i].name == name && (stack[i].k == Funcion || stack[i].k == Procedure) && usecondition[i])
			return l - stack[i].level;
	}
	cout << "find:" + name << endl;
	error(1042);
}
int table::ret_addr_var(string name,bool base){
	int length = stack.size() - 1;
	if (base)
	for (int i = length; i >=0; i--){
		if (stack[i].name == name && (stack[i].t == Int || stack[i].t == Char) && usecondition[i])
			return stack[i].addr;
	}
	else //数组
	for (int i = length; i >=0; i--){
		if (stack[i].name == name && (stack[i].t == Arraychar || stack[i].t == Arrayint) && usecondition[i])
			return stack[i].addr;
	}
	cout << "find:"+name << endl;
	error(1043);
}
string table::findvar(string function_name, int offset){            //返回var变量的名字
	int length = stack.size() - 1;
	int i;
	for (i = length; i >=0; i--){
		if (stack[i].name == function_name && (stack[i].k == Funcion || stack[i].k == Procedure) && usecondition[i])
			break;
	}
	if (i==0)
		error(107);
	else if (!stack[i + offset].val_formal)
		error(108);
	else return stack[i + offset].name;
}
string table::cur_function(string function_filed){             //返回当前函数名
	if (function_filed.find_last_of('/') != -1)
		return  function_filed.substr(function_filed.find_last_of('/') + 1, function_filed.size());
	else return  function_filed;
}
void table::sub_curname(string name){
	string lastname;
	if (function_name.find_last_of('/') != -1)
		  lastname=function_name.substr(function_name.find_last_of('/') + 1, function_name.size());
	else lastname=function_name;
	if (name == lastname){
		function_name = function_name.substr(0, function_name.find_last_of('/'));
	}
}
string table::alloc_function_name(string name){
	return name + inttostring(function_count++);
}
string table::function_rename(string name){
	int length = stack.size() - 1;
	int i;
	for (i = length; i >= 0; i--){
		if (stack[i].name ==name && (stack[i].k == Funcion || stack[i].k == Procedure) && usecondition[i])
			return stack[i].rename_function;
	}
	error(109);      //没有函数
}
void table::redefine_cheak(string name, kinds k, type t,string path,int length){
	int i;
	if (k == Const){                     //常量检查
		for (i = length; i >= 0; i--){
			if (stack[i].name == name && (stack[i].k == Const || stack[i].k == Var) && stack[i].function_filed == path){
				cout << "变量重定义" << endl;
				error(110);
			}
		}
	}
	else if (k == Funcion || k == Procedure){              // 过程 函数 检查
		for (i = length; i >= 0; i--){
			if (stack[i].name == name && (stack[i].k == Procedure || stack[i].k == Funcion) && stack[i].function_filed == path){
				cout << "变量重定义" << endl;
				error(110);
			}
		}
	}
	else if (k==Var&&(t==Arraychar||t==Arrayint)){              // 数组变量检查
		for (i = length; i >= 0; i--){
			if (stack[i].name == name && (stack[i].k == Const || stack[i].k == Var) && (stack[i].t == Arraychar || stack[i].t == Arrayint) && stack[i].function_filed == path){
				cout << "变量重定义" << endl;
				error(110);
			}
		}
	}
	else if (k == Var && (t ==Int || t == Char)){              // 基本类型变量检查
		for (i = length; i >= 0; i--){
			if (stack[i].name == name && (stack[i].k == Const || stack[i].k == Var || stack[i].k == Funcion) && (stack[i].t == Int || stack[i].t == Char) && stack[i].function_filed == path){
				cout << "变量重定义-" << endl;
				error(110);
			}
		}
	}
}