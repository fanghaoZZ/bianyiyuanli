#include "head.h"

int main() {
    int i = 3;
    string Regular_Expression;
    elem NFA_Elem;
    //1.����������ʽ
    input(Regular_Expression);
    if (Regular_Expression.length() > 1)    Regular_Expression = add_join_symbol(Regular_Expression);
    infixToPostfix Solution(Regular_Expression);
    //2.��׺ת��׺
    cout << "��׺���ʽΪ��";
    Regular_Expression = Solution.getResult();
    cout << Regular_Expression << endl;
    //3.���ʽתNFA
    NFA_Elem = express_to_NFA(Regular_Expression);
    //4.��ӡNFA
    Display(NFA_Elem);
    //5.����NFAdot�ļ�
    generateDotFile_NFA(NFA_Elem);

    // 6.��ʼ�� DFA ״̬���Ϻ�ת����ϵ
    vector<DFAState> dfaStates; //���ڴ洢���е�DFA״̬
    vector<DFATransition> dfaTransitions; //���ڴ洢DFA״̬֮���ת��
    set<string> nfaInitialStateSet;   //�洢NFA�ĳ�ʼ״̬

    //7.��NFA����DFA
    buildDFAFromNFA(NFA_Elem, dfaStates, dfaTransitions);
    //8.��ʾ DFA
    displayDFA(dfaStates, dfaTransitions);

    //9.����DFAdot�ļ�
    generateDotFile_DFA(dfaStates, dfaTransitions);

    //10.���� DFA ��С��
    minimizeDFA(dfaStates, dfaTransitions);

    //11.��ʾ��С����� DFA
    displayminDFA(dfaStates, dfaTransitions);
    //12.����minDFAdot�ļ�
    generateDotFile_minDFA(dfaStates, dfaTransitions);
    // 13. ģ��DFA��֤�����ַ����Ľ�����
    while (i)
    {
        string inputString;
        cout << "������һ���ַ���������֤: ";
        cin >> inputString;

        bool isAccepted = simulateDFA(dfaStates, dfaTransitions, inputString);
        if (isAccepted) {
            cout << "���ַ��������ܣ�" << endl;
        }
        else {
            cout << "���ַ������ܾ���" << endl;
        }
        i--;
    }
    
   

    return 0;
}

