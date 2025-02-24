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


//NFA�Ľڵ�
struct node
{
	string nodeName;

};
//NFA�ı�
struct edge
{
	node startName;	//��ʼ��
	node endName;	//Ŀ���
	char tranSymbol;	//ת������
};
//NFA����ɵ�Ԫ��һ�����NFA��Ԫ�������ɺܶ�С��Ԫͨ������ƴ������
struct elem
{
	int edgeCount;	//����
	edge edgeSet[100];	//��NFAӵ�еı�
	node startName;	//��ʼ״̬
	node endName; //����״̬
	char tranSymbol;
};

//�����½ڵ�
node new_node();
//���� a
elem act_Elem(char);
//����a|b
elem act_Unit(elem, elem);
//��ɵ�Ԫ��������
void elem_copy(elem&, elem);
//����ab
elem act_join(elem, elem);
//���� a*
elem act_star(elem);

void input(string&);

string add_join_symbol(string);	//������Ԫƴ����һ���൱��һ��+

//infixToPostfix �����ڽ���׺���ʽ���� a|b��ת��Ϊ��׺���ʽ���沨����ʾ����
class infixToPostfix {
public:
	infixToPostfix(const string& infix_expression);

	int is_letter(char check);//�ж��ַ��Ƿ�Ϊ��ĸ
	int ispFunc(char c);//��ȡ��������ջ�����ȼ�
	int icpFunc(char c);//��ȡ�������ĵ�ǰ���ȼ�
	void inToPost();
	string getResult();//��ȡת����ĺ�׺���ʽ

private:
	string infix;//�洢ԭʼ����׺���ʽ
	string postfix;//�洢ת����ĺ�׺���ʽ
	map<char, int> isp;//���ڲ��������ȼ���ӳ�䣨isp ��ջ�����ȼ���icp �ǵ�ǰ���������ȼ���
	map<char, int> icp;
};

elem express_to_NFA(string);//��������ʽ�ַ���ת��Ϊ NFA

void Display(elem);//��ʾ NFA ��״̬��ת����Ϣ

void generateDotFile_NFA(const elem& nfa);//�� NFA ת��Ϊ Dot ��ʽ����ͼ�λ����ߣ��� Graphviz��ʹ��

// ����DFA��״̬
struct DFAState {
	set<string> nfaStates;//NFA��״̬����
	set<string> originalStates;  // ��¼ԭʼNFA״̬����
	string stateName;//DFA��״̬����
	bool   isAccept=false;//�ж��Ƿ�Ϊ����״̬
	// �Ƚ����� DFAState �Ƿ���ȣ��� set ����ʹ��
	bool operator<(const DFAState& other) const {
		return stateName < other.stateName; // ����״̬����������
	}
;
	
};
// ���� DFA ��ת����ϵ
struct DFATransition {
	DFAState fromState;
	DFAState toState;
	char transitionSymbol;
};
// ���� NFA ״̬�Ħűհ�
DFAState eClosure(const set<string>& nfaStates, elem nfa);

// ���� DFA ��״̬ת��
DFAState move(const DFAState& dfaState, char transitionSymbol, elem nfa);

// ��� DFA ״̬�Ƿ���״̬������
bool isDFAStateInVector(const vector<DFAState>& dfaStates, const DFAState& targetState);

//���ת�����Ƿ��ڱ߼����У�����a->b�Ƿ��Ѿ��ڼ�����
bool isTransitionInVector(DFAState, DFAState, char, vector<DFATransition>);

//NFAת��ΪDFA
void buildDFAFromNFA(const elem& NFA_Elem, vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);

// ��ʾ DFA ״̬��ת�ƹ�ϵ
void displayDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions);

//����dot�ļ�
void generateDotFile_DFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);

bool areStatesEqual(const set<string>& s1, const set<string>& s2);

void minimizeDFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);

void displayminDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions);

void generateDotFile_minDFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);

bool simulateDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions, const string& inputString);