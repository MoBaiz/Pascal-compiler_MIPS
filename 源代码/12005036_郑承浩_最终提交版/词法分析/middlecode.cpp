#include"middlecode.h"
string inttostring(int num)
{
	strstream ss;
	string s;
	ss << num;
	ss >> s;
	return s;
	return 0;
}
void Middle::emit(_operator op, string op1, string op2, string res){
	MidCode temp;
	temp.op = op;
	temp.op1 = op1;
	temp.op2 = op2;
	temp.res = res;
//	print(temp,cout);
	MidCodes.push_back(temp);
}
void Middle::newfunction(){
	t = -1;
}
string Middle::alloc(){                               
	t++;
	return "t_"+inttostring(t);
}
void Middle::release(int n){
	t -= n;
}
void Middle::showmidcode(string flie){
	ofstream fout;
	fout.open(flie+"_midlle.txt");
	fout << left;
	fout << setw(4) << "NO" << setw(10) << "op" << setw(10) << "op1" << setw(10) << "op2" << setw(10) << "res" << endl;
	{
		for (int j = 0; j < MidCodes.size(); j++){
			fout << setw(4) << j;
			MidCode &mc = MidCodes[j];
			print(mc, fout);
		//	print(mc, cout);
		}
	}
	fout << right;
}
void Middle::print(MidCode mc, ostream &os){
	//cout << ++cur;
	static const string _operator_str[] =
	{
		"ADD", "SUB", "MUL", "DIV", "MIN",
		"J", "JEQL", "JNEQ", "JLSS", "JLEQ", "JGRE", "JGEQ",
		"ASS", "ASS_ARR", "ASS_LARR",
		"PARAMETER", "CALL_P", "CALL_F", "RETURN", "SCANF", "PRINTF_S", "PRINTF_C", "PRINTF", "LABLE"
	};
/*	//临时变量在前面加个t
	if (mc.op1 != ""&&isdigit(mc.op1[0]))
		mc.op1 = "t_" + mc.op1;
	if (mc.op2 != ""&&isdigit(mc.op2[0]))
		mc.op2 = "t_" + mc.op2;
	if (mc.res != ""&&isdigit(mc.res[0]))
		mc.res = "t_" + mc.res;
*/
	switch (mc.op)
	{
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MIN:
	case PARAMETER:
	case CALL_P:
	case CALL_F:
	case RETURN:
	case SCANF:
	case PRINTF:
	case PRINTF_S:
	case PRINTF_C:
		os << setw(10) << _operator_str[mc.op] << setw(10) << mc.op1 << setw(10) << mc.op2 << mc.res << endl;
		break;
	case J:
	case JEQL:
	case JNEQ:
	case JLSS:
	case JLEQ:
	case JGRE:
	case JGEQ:
		os << setw(10) << _operator_str[mc.op] << setw(10) << mc.op1 << setw(10) << mc.op2 << setw(10) << "lable_"+mc.res << endl;
		break;
	case ASS:
	case ASS_ARR:
	case ASS_LARR:
		os << setw(10) << _operator_str[mc.op] << setw(10) << mc.op1 << setw(10) << mc.op2 << setw(10) << mc.res << endl;
		break;
	case LABLE:
		if (isdigit(mc.op1[0]))
			os << "lable" + mc.op1 + ":" << endl;
		else os << mc.op1 + ":" << endl;
		break;
	default:;
	}
}
string Middle::getlable(){
	l++;
	return inttostring(l);
}
vector<MidCode>::iterator Middle::getit(){
	return MidCodes.end();
}
void Middle::insert_emit(int it, _operator op, string op1, string op2, string res){
	MidCode temp;
	temp.op = op;
	temp.op1 = op1;
	temp.op2 = op2;
	temp.res = res;
	MidCodes.insert(MidCodes.begin()+it,temp);
}