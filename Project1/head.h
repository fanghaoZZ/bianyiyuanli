#include <iostream>
#include <stdio.h>
#include <cctype>
#include <stack>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iterator>
#include <fstream>
#include <queue>
#include <sstream>  
#include <unordered_map>
#include <unordered_set>

using namespace std;


//NFA的节点
struct node
{
	string nodeName;

};
//NFA的边
struct edge
{
	node startName;	//起始点
	node endName;	//目标点
	char tranSymbol;	//转换符号
};
//NFA的组成单元，一个大的NFA单元可以是由很多小单元通过规则拼接起来
struct elem
{
	int edgeCount;	//边数
	edge edgeSet[100];	//该NFA拥有的边
	node startName;	//开始状态
	node endName; //结束状态
	char tranSymbol;
};

//创建新节点
node new_node();
//处理 a
elem act_Elem(char);
//处理a|b
elem act_Unit(elem, elem);
//组成单元拷贝函数
void elem_copy(elem&, elem);
//处理ab
elem act_join(elem, elem);
//处理 a*
elem act_star(elem);

void input(string&);

string add_join_symbol(string);	//两个单元拼接在一起相当于一个+

//infixToPostfix 类用于将中缀表达式（如 a|b）转换为后缀表达式（逆波兰表示法）
class infixToPostfix {
public:
	infixToPostfix(const string& infix_expression);

	int is_letter(char check);//判断字符是否为字母
	int ispFunc(char c);//获取操作符的栈内优先级
	int icpFunc(char c);//获取操作符的当前优先级
	void inToPost();
	string getResult();//获取转换后的后缀表达式

private:
	string infix;//存储原始的中缀表达式
	string postfix;//存储转换后的后缀表达式
	map<char, int> isp;//用于操作符优先级的映射（isp 是栈顶优先级，icp 是当前操作数优先级）
	map<char, int> icp;
};

elem express_to_NFA(string);//将正则表达式字符串转换为 NFA

void Display(elem);//显示 NFA 的状态和转移信息

void generateDotFile_NFA(const elem& nfa);//将 NFA 转换为 Dot 格式，供图形化工具（如 Graphviz）使用

// 定义DFA的状态
struct DFAState {
	set<string> nfaStates;//NFA的状态集合
	set<string> originalStates;  // 记录原始NFA状态集合
	string stateName;//DFA的状态名称
	bool   isAccept=false;//判断是否为接受状态
	// 比较两个 DFAState 是否相等，供 set 排序使用
	bool operator<(const DFAState& other) const {
		return stateName < other.stateName; // 根据状态名进行排序
	}
;
	
};
// 定义 DFA 的转换关系
struct DFATransition {
	DFAState fromState;
	DFAState toState;
	char transitionSymbol;
};
// 计算 NFA 状态的ε闭包
DFAState eClosure(const set<string>& nfaStates, elem nfa);

// 计算 DFA 的状态转移
DFAState move(const DFAState& dfaState, char transitionSymbol, elem nfa);

// 检查 DFA 状态是否在状态集合中
bool isDFAStateInVector(const vector<DFAState>& dfaStates, const DFAState& targetState);

//检查转换边是否在边集合中，比如a->b是否已经在集合中
bool isTransitionInVector(DFAState, DFAState, char, vector<DFATransition>);

//NFA转换为DFA
void buildDFAFromNFA(const elem& NFA_Elem, vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);

// 显示 DFA 状态和转移关系
void displayDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions);

//生成dot文件
void generateDotFile_DFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);

bool areStatesEqual(const set<string>& s1, const set<string>& s2);

void minimizeDFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);

void displayminDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions);

void generateDotFile_minDFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);

bool simulateDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions, const string& inputString);