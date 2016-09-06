#include"lex.h"
#include"error.h"
#include"table.h"
char ch = ' ';
int line = 1;
string token;
string line_cache1,line_cache2;
symset sym;
symloc temp;
string word[45] = {
	"arraysym", "constsym", "endsym", "integersym", "thensym", "beginsym",
	"dosym", "forsym", "ofsym", "tosym", "casesym", "downtosym", "functionsym",
	"proceduresym", "varsym", "charsym", "elsesym", "ifsym", "readsym", "writesym",
	"add", "times", "sub", "slash", "lparen", "rparen", "eql", "comma", "period", "smark", "dmark",
	"lss", "leq", "geq", "gre", "neq", "colon", "semicolon", "lbrack", "rbrack", "becomes", "ident", "number", "str", "character"
};

lex::lex(string file_name){
	SymSet["array"] = arraysym;       //有多余 
	SymSet["const"]=constsym;
	SymSet["end"] = endsym;
	SymSet["integer"]=integersym;
	SymSet["then"]=thensym;
	SymSet["begin"] = beginsym;
	SymSet["do"] = dosym;
	SymSet["for"] = forsym;
	SymSet["of"] = ofsym;
	SymSet["to"] = tosym;
	SymSet["case"] = casesym;
	SymSet["downto"] = downtosym;
	SymSet["function"] = functionsym;
	SymSet["procedure"] = proceduresym;
	SymSet["var"] = varsym;
	SymSet["char"] = charsym;
	SymSet["else"] = elsesym;
	SymSet["if"] = ifsym;
	SymSet["read"] = readsym;
	SymSet["write"] = writesym;
	SymSet["+"] = add;
	SymSet["*"] = times;
	SymSet["-"] = sub;
	SymSet["/"] = slash;
	SymSet["("] = lparen;
	SymSet[")"] = rparen;
	SymSet["="] = eql;
	SymSet[","] = comma;
	SymSet["."] = period;
	SymSet["'"] = smark;
	SymSet["\""] = dmark;
	SymSet["<"] = lss;
	SymSet["<="] = leq;
	SymSet[">="] = geq;
	SymSet[">"] = gre;
	SymSet["<>"] = neq;
	SymSet[":"] = colon;
	SymSet[";"] = semicolon;
	SymSet["["] = lbrack;
	SymSet["]"] = rbrack;
	SymSet[":="] = becomes;
	fin.open(file_name);
	if (!fin)
	{
		cerr << "Can't open the file" << endl;
		exit(0);
	}
}
void lex::getch(){
	if ((ch = fin.get()) != EOF){
		if (ch >= 'A'&&ch <= 'Z')
			ch = ch +32;
		line_cache1 += ch;
	}
			//printf("%c", ch);
	else{
		printf("%c\n", ch);
		printf("Program Incomplete");
		fin.close();
		exit(0);
	}
}
void lex::get_normal_ch(){
	if ((ch = fin.get()) != EOF){
		line_cache1 += ch;;
	}
	//printf("%c", ch);
	else{
		printf("%c\n", ch);
		printf("Program Incomplete");
		fin.close();
		exit(0);
	}
}
void lex::decache(){
	while (Queue.size() != 0){
		getsym();
	}
}
string chartostring(char a){
	stringstream stream;
	stream << a;
	return stream.str();
}
void lex::getsym(){
	if (size() == 0){
		token = "";
		while (ch == ' ' || ch == '\n' || ch == '\t'){
			if (ch == '\n'){
				line++;
				line_cache2 = line_cache1;
				line_cache1.clear();
			}
			getch();
		}
		if (isletter(ch)){//字母开头只可能是标识符或者保留字
			do{
				token += ch;
				getch();
			} while (isletter(ch) || isnum(ch));
			auto it = SymSet.find(token);
			if (it != SymSet.end())
				sym = it->second;
			else sym = ident;
			//cout << "------------------------------"+token<< endl;
		}
		else if (isnum(ch)){             //todo  数字过大
			int num = 0;
			stringstream ss;
			while (isnum(ch)){
				num = 10 * num + ch + '0';
				sym = number;
				token += ch;
				getch();
			}
		}
		else if (ch == ':'){
			getch();
			if (ch == '='){
				sym = becomes;
				getch();
			}
			else{
				sym = colon;
			}
		}
		else if (ch == '<'){
			getch();
			if (ch == '='){
				sym = leq;
				getch();
			}
			else if (ch == '>'){
				sym = neq;
				getch();
			}
			else
				sym = lss;
		}
		else if (ch == '>'){
			getch();
			if (ch == '='){
				sym = geq;
				getch();
			}
			else sym = gre;
		}
		else if (ch == '"'){
			get_normal_ch();
			do{
				token += ch;
				get_normal_ch();
			} while (ch == 32 || ch == 33 || ch >= 35 && ch <= 126);
			sym = str;
			if (ch == '"'){
				getch();
				sym = str;
			}
			else printf("字符串出现非法字符");
		}
		else if (ch == '\''){       //字符
			get_normal_ch();
			if (isletter(ch) || isnum(ch)){
				token += ch;
				getch();
			}
			else error(4);
			if (ch == '\''){
				getch();
				sym = character;
			}
			else error(4);
		}
		else if (ch == '.'){
			sym = period;
		}
		else{
			string str;
			stringstream stream;
			stream << ch;
			str = stream.str();
			auto it = SymSet.find(str);
			if (it != SymSet.end()) //其他在sym集中的单个符号
				sym = it->second;
			//else
			//	token = chartostring(ch);
			getch();
		}
	}
	else sym = dequeue();
}
bool lex::isletter(char a){
	return ((a >= 'a'&&a <= 'z') || (a >= 'A'&&a <= 'Z')) ? true : false;
}
bool lex::isnum(char a){
	return (a >= '0'&&a <= '9') ? true : false;
}
symset lex::queue(){
	getsym();
	temp.sym = sym;
	temp.line = line;
	if (Queue.size()!=0)
	line = Queue.front().line;  //todo行数 显示有些问题 换行/n
	Queue.push_back(temp);
	return sym;
}
symset lex::dequeue(){  //返回出队后队列长度
	temp = Queue.front();
	Queue.pop_front();
	line = temp.line;
	return temp.sym;
}
int lex::size(){
	return Queue.size();
}