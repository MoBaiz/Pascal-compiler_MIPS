#pragma once
#include"block.h"
#include"table.h"
#include"generate.h"
#include<string>
extern symset sym;
bool isright = true;
int  errorcount = 0;
int main(){
   /* int i = 0;
	lex fuck("1.txt");
	while (sym != period){
	fuck.getsym();
	printf("%d  ", ++i);
	cout << word[sym] + "   ";
	cout << token << endl;
	}
	cout << "--------------------finish lex-------------------" << endl;*/
	string file;
	cout << "file directory:";
	cin >> file;
	lex l(file);
	table t;
	Middle m;
	Generator g(t,m);
	l.getsym();
	block Block(l, t, m);
	t.setfunctionname("main");
	Block.parse(1, "main");
	l.getsym();
	if (sym == period)
		cout << "--------------------finish parse-------------------" << endl;
	else error(38);
	if (!isright){
		cout << "program wrong,compiling stop" << endl;
		exit(1);
	}
	else{
	//	t.showqueue();
		m.showmidcode(file);
   //		t.show();
		g.generate();
		g.output(file);
		cout << "-------------------generator finish-------------------" << endl;
	}

}
