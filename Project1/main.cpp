#include "head.h"

int main() {
    int i = 3;
    string Regular_Expression;
    elem NFA_Elem;
    //1.输入正则表达式
    input(Regular_Expression);
    if (Regular_Expression.length() > 1)    Regular_Expression = add_join_symbol(Regular_Expression);
    infixToPostfix Solution(Regular_Expression);
    //2.中缀转后缀
    cout << "后缀表达式为：";
    Regular_Expression = Solution.getResult();
    cout << Regular_Expression << endl;
    //3.表达式转NFA
    NFA_Elem = express_to_NFA(Regular_Expression);
    //4.打印NFA
    Display(NFA_Elem);
    //5.生成NFAdot文件
    generateDotFile_NFA(NFA_Elem);

    // 6.初始化 DFA 状态集合和转换关系
    vector<DFAState> dfaStates; //用于存储所有的DFA状态
    vector<DFATransition> dfaTransitions; //用于存储DFA状态之间的转移
    set<string> nfaInitialStateSet;   //存储NFA的初始状态

    //7.从NFA构造DFA
    buildDFAFromNFA(NFA_Elem, dfaStates, dfaTransitions);
    //8.显示 DFA
    displayDFA(dfaStates, dfaTransitions);

    //9.生成DFAdot文件
    generateDotFile_DFA(dfaStates, dfaTransitions);

    //10.进行 DFA 最小化
    minimizeDFA(dfaStates, dfaTransitions);

    //11.显示最小化后的 DFA
    displayminDFA(dfaStates, dfaTransitions);
    //12.生成minDFAdot文件
    generateDotFile_minDFA(dfaStates, dfaTransitions);
    // 13. 模拟DFA验证输入字符串的接受性
    while (i)
    {
        string inputString;
        cout << "请输入一个字符串进行验证: ";
        cin >> inputString;

        bool isAccepted = simulateDFA(dfaStates, dfaTransitions, inputString);
        if (isAccepted) {
            cout << "该字符串被接受！" << endl;
        }
        else {
            cout << "该字符串被拒绝！" << endl;
        }
        i--;
    }
    
   

    return 0;
}

