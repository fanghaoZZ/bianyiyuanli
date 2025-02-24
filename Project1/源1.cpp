#include "head.h"

int nodeNum = 0;

//创建新节点
node new_node()
{
	node newNode;
	newNode.nodeName = nodeNum + 65;//将名字用大写字母表示
	nodeNum++;
	return newNode;
}

//接收输入正规表达式
void input(string& RE)
{
	cout << "输入正则表达式：  （操作符：() * |;字符集：a~z A~Z）" << endl;
	cin >> RE;
}

//组成单元拷贝函数
void elem_copy(elem& dest, elem source)
{
	for (int i = 0; i < source.edgeCount; i++) {
		dest.edgeSet[dest.edgeCount + i] = source.edgeSet[i];
	}
	dest.edgeCount += source.edgeCount;
}

//处理 a
elem act_Elem(char c)
{
	//新节点
	node startNode = new_node();
	node endNode = new_node();

	//新边
	edge newEdge;
	newEdge.startName = startNode;
	newEdge.endName = endNode;
	newEdge.tranSymbol = c;

	//新NFA组成元素（小的NFA元素/单元)
	elem newElem;
	newElem.edgeCount = 0;	//初始状态
	newElem.edgeSet[newElem.edgeCount++] = newEdge;
	newElem.startName = newElem.edgeSet[0].startName;
	newElem.endName = newElem.edgeSet[0].endName;

	return newElem;
}
//处理a|b
elem act_Unit(elem fir, elem sec)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;

	//获得新的状态节点
	node startNode = new_node();
	node endNode = new_node();

	//构建e1（连接起点和AB的起始点A）
	edge1.startName = startNode;
	edge1.endName = fir.startName;
	edge1.tranSymbol = '#';

	//构建e2（连接起点和CD的起始点C）
	edge2.startName = startNode;
	edge2.endName = sec.startName;
	edge2.tranSymbol = '#';

	//构建e3（连接AB的终点和终点）
	edge3.startName = fir.endName;
	edge3.endName = endNode;
	edge3.tranSymbol = '#';

	//构建e4（连接CD的终点和终点）
	edge4.startName = sec.endName;
	edge4.endName = endNode;
	edge4.tranSymbol = '#';

	//将fir和sec合并
	elem_copy(newElem, fir);
	elem_copy(newElem, sec);

	//新构建的4条边
	newElem.edgeSet[newElem.edgeCount++] = edge1;
	newElem.edgeSet[newElem.edgeCount++] = edge2;
	newElem.edgeSet[newElem.edgeCount++] = edge3;
	newElem.edgeSet[newElem.edgeCount++] = edge4;

	newElem.startName = startNode;
	newElem.endName = endNode;

	return newElem;
}
//处理 N(s)N(t)
elem act_join(elem fir, elem sec)
{
	//将fir的结束状态和sec的开始状态合并，将sec的边复制给fir，将fir返回
	//将sec中所有以StartState开头的边全部修改
	for (int i = 0; i < sec.edgeCount; i++) {
		if (sec.edgeSet[i].startName.nodeName.compare(sec.startName.nodeName) == 0)
		{
			sec.edgeSet[i].startName = fir.endName; //该边e1的开始状态就是N(t)的起始状态
		}
		else if (sec.edgeSet[i].endName.nodeName.compare(sec.startName.nodeName) == 0) {
			sec.edgeSet[i].endName = fir.endName; //该边e2的结束状态就是N(t)的起始状态
		}
	}
	sec.startName = fir.endName;

	elem_copy(fir, sec);

	//将fir的结束状态更新为sec的结束状态
	fir.endName = sec.endName;
	return fir;
}
//处理a*
elem act_star(elem Elem)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;

	//获得新状态节点
	node startNode = new_node();
	node endNode = new_node();

	//e1
	edge1.startName = startNode;
	edge1.endName = endNode;
	edge1.tranSymbol = '#';	//闭包取空串

	//e2
	edge2.startName = Elem.endName;
	edge2.endName = Elem.startName;
	edge2.tranSymbol = '#';

	//e3
	edge3.startName = startNode;
	edge3.endName = Elem.startName;
	edge3.tranSymbol = '#';

	//e4
	edge4.startName = Elem.endName;
	edge4.endName = endNode;
	edge4.tranSymbol = '#';

	//构建单元
	elem_copy(newElem, Elem);

	//将新构建的四条边加入EdgeSet
	newElem.edgeSet[newElem.edgeCount++] = edge1;
	newElem.edgeSet[newElem.edgeCount++] = edge2;
	newElem.edgeSet[newElem.edgeCount++] = edge3;
	newElem.edgeSet[newElem.edgeCount++] = edge4;

	//构建NewElem的启示状态和结束状态
	newElem.startName = startNode;
	newElem.endName = endNode;

	return newElem;
}
//判断是否有字母
int is_letter(char check) {
	if (check >= 'a' && check <= 'z' || check >= 'A' && check <= 'Z')
		return true;
	return false;
}
//添加连接符号
string add_join_symbol(string add_string)
{
	int length = add_string.size();
	int return_string_length = 0;
	char* return_string = new char[2 * length + 2];//最多是两倍
	char first, second;
	for (int i = 0; i < length - 1; i++)
	{
		first = add_string.at(i);
		second = add_string.at(i + 1);
		return_string[return_string_length++] = first;
		//要加的可能性如ab 、 *b 、 a( 、 )b 等情况
		//若第二个是字母、第一个不是'('、'|'都要添加
		if (first != '(' && first != '|' && is_letter(second))
		{
			return_string[return_string_length++] = '+';
		}
		//若第二个是'(',第一个不是'|'、'(',也要加
		else if (second == '(' && first != '|' && first != '(')
		{
			return_string[return_string_length++] = '+';
		}
	}
	//将最后一个字符写入second
	return_string[return_string_length++] = second;
	return_string[return_string_length] = '\0';
	string STRING(return_string);
	cout << "加'+'后的表达式：" << STRING << endl;
	return STRING;
}

//类里的各类元素定义
infixToPostfix::infixToPostfix(const string& infix_expression) : infix(infix_expression), postfix("") {
	isp = { {'+', 3}, {'|', 5}, {'*', 7},  {'(', 1}, {')', 8}, {'#', 0} };
	icp = { {'+', 2}, {'|', 4}, {'*', 6}, {'(', 8}, {')', 1}, {'#', 0} };
}

int infixToPostfix::is_letter(char check) {
	if (check >= 'a' && check <= 'z' || check >= 'A' && check <= 'Z')
		return true;
	return false;
}

int infixToPostfix::ispFunc(char c) {
	int priority = isp.count(c) ? isp[c] : -1;
	if (priority == -1) {
		cerr << "error: 出现未知符号！" << endl;
		exit(1);  // 异常退出
	}
	return priority;
}

int infixToPostfix::icpFunc(char c) {
	int priority = icp.count(c) ? icp[c] : -1;
	if (priority == -1) {
		cerr << "error: 出现未知符号！" << endl;
		exit(1);  // 异常退出
	}
	return priority;
}

void infixToPostfix::inToPost() {
	string infixWithHash = infix + "#";
	stack<char> stack;
	int loc = 0;
	while (!stack.empty() || loc < infixWithHash.size()) {
		if (is_letter(infixWithHash[loc])) {
			postfix += infixWithHash[loc];
			loc++;
		}
		else {
			char c1 = (stack.empty()) ? '#' : stack.top();
			char c2 = infixWithHash[loc];
			if (ispFunc(c1) < icpFunc(c2)) { // 栈顶操作符优先级低于当前字符，将当前字符入栈
				stack.push(c2);
				loc++;
			}
			else if (ispFunc(c1) > icpFunc(c2)) {  // 栈顶操作符优先级高于当前字符，将栈顶操作符出栈并添加到后缀表达式
				postfix += c1;
				stack.pop();
			}
			else {
				if (c1 == '#' && c2 == '#') { // 遇到两个 #，表达式结束
					break;
				}
				stack.pop(); //其中右括号遇到左括号时会抵消，左括号出栈，右括号不入栈
				loc++;
			}
		}
	}
}

string infixToPostfix::getResult() {
	postfix = ""; // 清空结果
	inToPost();
	return postfix;
}

//表达式转NFA处理函数,返回最终的NFA集合
elem express_to_NFA(string expression)
{
	int length = expression.size();
	char element;
	elem Elem, fir, sec;
	stack<elem> STACK;
	for (int i = 0; i < length; i++)
	{
		element = expression.at(i);
		switch (element)
		{
		case '|':
			sec = STACK.top();
			STACK.pop();
			fir = STACK.top();
			STACK.pop();
			Elem = act_Unit(fir, sec);
			STACK.push(Elem);
			break;
		case '*':
			fir = STACK.top();
			STACK.pop();
			Elem = act_star(fir);
			STACK.push(Elem);
			break;
		case '+':
			sec = STACK.top();
			STACK.pop();
			fir = STACK.top();
			STACK.pop();
			Elem = act_join(fir, sec);
			STACK.push(Elem);
			break;
		default:
			Elem = act_Elem(element);
			STACK.push(Elem);
		}
	}
	
	Elem = STACK.top();
	STACK.pop();

	return Elem;
}

//打印NFA
void Display(elem Elem) {
	cout << "NFA States:" << endl;
	cout << "Start State: " << Elem.startName.nodeName << endl;
	cout << "End State: " << Elem.endName.nodeName << endl;

	cout << "NFA Transitions:" << endl;
	for (int i = 0; i < Elem.edgeCount; i++) {
		cout << "Edge " << i + 1 << ": ";
		cout << Elem.edgeSet[i].startName.nodeName << " --" << Elem.edgeSet[i].tranSymbol << "--> ";
		cout << Elem.edgeSet[i].endName.nodeName << endl;
	}

	cout << "End" << endl;
}

//生成NFAdot文件
void generateDotFile_NFA(const elem& nfa) {
	std::ofstream dotFile("nfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph NFA {\n";
		dotFile << "  rankdir=LR;  // 横向布局\n\n";
		dotFile << " node [shape = circle];   // 状态节点\n\n";

		dotFile << nfa.endName.nodeName << " [shape=doublecircle];\n";
		// 添加 NFA 状态
		dotFile << "  " << nfa.startName.nodeName << " [label=\"Start State: " << nfa.startName.nodeName << "\"];\n";
		dotFile << "  " << nfa.endName.nodeName << " [label=\"End State: " << nfa.endName.nodeName << "\"];\n";

		// 添加 NFA 转移
		for (int i = 0; i < nfa.edgeCount; i++) {
			const edge& currentEdge = nfa.edgeSet[i];
			dotFile << "  " << currentEdge.startName.nodeName << " -> " << currentEdge.endName.nodeName << " [label=\"" << currentEdge.tranSymbol << "\"];\n";
		}

		dotFile << "}\n";

		dotFile.close();
		std::cout << "NFA DOT file generated successfully.\n";
	}
	else {
		std::cerr << "Unable to open NFA DOT file.\n";
	}
}


// 计算 NFA 状态的ε闭包
DFAState eClosure(const set<string>& nfaStates, elem nfa) {
	DFAState eClosureState;
	eClosureState.nfaStates = nfaStates;

	stack<string> stateStack;

	// 初始化栈，将初始状态加入栈，最开始nfaState里只有NFA_Elem.startName
	for (const string& nfaState_name : nfaStates) {
		stateStack.push(nfaState_name);
	}

	while (!stateStack.empty()) {
		string currentState = stateStack.top();
		stateStack.pop();

		// 遍历 NFA 的边
		for (int i = 0; i < nfa.edgeCount; i++) {
			edge currentEdge = nfa.edgeSet[i];

			// 如果边的起始状态是当前状态，并且边的转换符号是#，那么将目标状态加入ε闭包
			if (currentEdge.startName.nodeName == currentState && currentEdge.tranSymbol == '#') {
				// 检查目标状态是否已经在ε闭包中，避免重复添加
				if (eClosureState.nfaStates.find(currentEdge.endName.nodeName) == eClosureState.nfaStates.end()) {
					eClosureState.nfaStates.insert(currentEdge.endName.nodeName);
					// 将目标状态加入栈以便进一步处理
					stateStack.push(currentEdge.endName.nodeName);
				}
			}
		}
	}

	// 为ε闭包分配一个唯一的名称
	for (const string& nfaState_name : eClosureState.nfaStates) {
		eClosureState.stateName += nfaState_name;
	}

	return eClosureState;
}

//move函数
DFAState move(const DFAState& dfaState, char transitionSymbol, elem nfa) {
	DFAState nextState;

	// 遍历 DFAState 中的每个 NFA 状态
	for (const string& nfaState_name : dfaState.nfaStates) {
		// 在这里遍历所有 NFA 状态的边
		for (int i = 0; i < nfa.edgeCount; i++) {
			edge currentEdge = nfa.edgeSet[i];

			// 如果边的起始状态是当前状态，且边的转换符号等于输入符号，将目标状态加入 nextState
			if (currentEdge.startName.nodeName == nfaState_name && currentEdge.tranSymbol == transitionSymbol && currentEdge.tranSymbol != '#') {
				nextState.nfaStates.insert(currentEdge.endName.nodeName);
			}
		}
	}

	// 为 nextState 分配一个唯一的名称
	for (const string& nfaState_name : nextState.nfaStates) {
		nextState.stateName += nfaState_name;
	}

	return nextState;
}

// 检查 DFA 状态是否在状态集合中,即dfaStates里有没有找到targetState
bool isDFAStateInVector(const vector<DFAState>& dfaStates, const DFAState& targetState) {
	for (const DFAState& state : dfaStates) {
		if (state.stateName == targetState.stateName) {
			return true; // 找到匹配的状态
		}
	}
	return false; // 没有找到匹配的状态
}

//检查转换边是否在边集合中，比如a->b是否已经在集合中
bool isTransitionInVector(DFAState dfaState, DFAState dfaNextState, char symbol, vector<DFATransition> dfaTransitions)
{
	for (const DFATransition& transition : dfaTransitions) {
		if (transition.fromState.stateName == dfaState.stateName && dfaNextState.stateName == dfaNextState.stateName && symbol == transition.transitionSymbol) {
			return true;	//找到匹配的状态
		}
	}
	return false;
}

void buildDFAFromNFA(const elem& NFA_Elem, vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	// 初始化 DFA 状态集合和转换关系
	set<string> nfaInitialStateSet;
	nfaInitialStateSet.insert(NFA_Elem.startName.nodeName);
	DFAState dfaInitialState = eClosure(nfaInitialStateSet, NFA_Elem); // 计算 NFA 初始状态的 ε闭包

	// 判断是否包含 NFA 的 endName 状态来确定是否为接受状态
	for (const string& nfaState : dfaInitialState.nfaStates) {
		if (nfaState == NFA_Elem.endName.nodeName) {
			dfaInitialState.isAccept = true;  // 如果包含 endName，标记为接受状态
			break;
		}
	}

	dfaStates.push_back(dfaInitialState);

	// 开始构建 DFA
	for (int i = 0; i < dfaStates.size(); i++) {
		DFAState dfaState = dfaStates[i];
		for (int j = 0; j < NFA_Elem.edgeCount; j++) {
			char symbol = NFA_Elem.edgeSet[j].tranSymbol;
			DFAState nextState = move(dfaState, symbol, NFA_Elem);
			DFAState dfaNextState = eClosure(nextState.nfaStates, NFA_Elem);

			if (!nextState.nfaStates.empty()) {
				// 如果下一个状态不为空，且在 DFA 状态集合中还未添加，则加入 DFA 状态集合
				if (!isDFAStateInVector(dfaStates, dfaNextState)) {
					// 检查是否为接受状态
					for (const string& nfaState : dfaNextState.nfaStates) {
						if (nfaState == NFA_Elem.endName.nodeName) {
							dfaNextState.isAccept = true;  // 如果包含 endName，标记为接受状态
							break;
						}
					}
					dfaStates.push_back(dfaNextState);
				}

				// 对于边也要去重
				if (!isTransitionInVector(dfaState, dfaNextState, symbol, dfaTransitions)) {
					dfaTransitions.push_back({ dfaState, dfaNextState, symbol });
				}
			}
		}
	}
}

// 显示 DFA 状态和转移关系，包括起始和结束状态
void displayDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions) {
	cout << "DFA States:" << endl;

	for (const DFAState& state : dfaStates) {
		cout << "State " << state.stateName << " (NFA States: ";
		for (const string& nfaState_name : state.nfaStates) {
			cout << nfaState_name << " ";
		}
		cout << ")";

		// 显示是否是接受状态
		if (state.isAccept) {
			cout << " (Accepting State)";
		}
		else {
			cout << " (Non-Accepting State)";
		}

		cout << endl;
	}

	cout << "\nDFA Transitions:" << endl;
	for (const DFATransition& transition : dfaTransitions) {
		cout << "State " << transition.fromState.stateName << " --" << transition.transitionSymbol << "--> State " << transition.toState.stateName << endl;
	}
}

//生成DFA的dot文件
void generateDotFile_DFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	std::ofstream dotFile("dfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph DFA {\n";
		dotFile << "  rankdir=LR;  // 横向布局\n\n";
		dotFile << " node [shape = circle];   // 初始状态\n\n";

		// 标记接受状态
		for (const auto& state : dfaStates) {
			if (state.isAccept) {
				dotFile << "  " << state.stateName << " [shape=doublecircle];\n";
			}
		}

		// 添加DFA状态
		for (const auto& state : dfaStates) {
			dotFile << "  " << state.stateName;
			dotFile << " [label=\"State " << state.stateName;
			if (state.stateName == dfaStates.front().stateName) dotFile << "\\n(startState)";
			if (state.isAccept) dotFile << "\\n(acceptState)";
			dotFile << "\"];\n";
		}
		dotFile << "\n";

		// 添加DFA转移
		for (const auto& transition : dfaTransitions) {
			dotFile << "  " << transition.fromState.stateName << " -> " << transition.toState.stateName << " [label=\"" << transition.transitionSymbol << "\"];\n";
		}

		dotFile << "}\n";

		dotFile.close();
		std::cout << "DFA DOT file generated successfully.\n";
	}
	else {
		std::cerr << "Unable to open DOT file.\n";
	}
}

bool areStatesEqual(const set<string>& s1, const set<string>& s2) {
    return s1 == s2;  // 使用 == 运算符来检查集合是否相等
}

void minimizeDFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	unordered_map<string, set<DFAState>> stateGroups;
	set<DFAState> acceptStates;
	set<DFAState> rejectStates;

	// 将状态分为接受状态和非接受状态
	for (auto& state : dfaStates) {
		if (state.isAccept) {
			acceptStates.insert(state);
		}
		else {
			rejectStates.insert(state);
		}
	}

	// 初始状态分组：根据接受状态和非接受状态
	stateGroups["accept"] = acceptStates;
	stateGroups["reject"] = rejectStates;

	bool splitOccurred = true;
	while (splitOccurred) {
		splitOccurred = false;
		unordered_map<string, set<DFAState>> newStateGroups;

		// 对每个状态组进行细分
		for (auto& groupEntry : stateGroups) {
			auto& group = groupEntry.second;
			unordered_map<string, set<DFAState>> transitionGroups;

			for (auto& state : group) {
				map<char, string> transitionMap;
				for (auto& transition : dfaTransitions) {
					if (transition.fromState.stateName == state.stateName) {
						transitionMap[transition.transitionSymbol] = transition.toState.stateName;
					}
				}

				// 使用转移关系的唯一标识来分组
				string transitionKey;
				for (auto& entry : transitionMap) {
					transitionKey += entry.first + entry.second;
				}

				transitionGroups[transitionKey].insert(state);
			}

			// 如果存在多个不同的转移组，说明需要继续细分
			if (transitionGroups.size() > 1) {
				splitOccurred = true;
			}

			// 将细分后的状态组更新到新的状态分组中
			for (auto& transitionGroup : transitionGroups) {
				newStateGroups[transitionGroup.first] = transitionGroup.second;
			}
		}

		// 更新状态分组
		stateGroups = newStateGroups;
	}

	// 生成新的 DFA 状态和记录原始状态集合
	vector<DFAState> newDFAStates;
	unordered_map<string, DFAState> stateMapping;
	int newStateCount = 0;

	for (auto& groupEntry : stateGroups) {
		DFAState newState;
		newState.stateName = "a" + to_string(newStateCount++);
		newState.isAccept = groupEntry.second.begin()->isAccept;

		// 记录原始状态集合
		for (auto& state : groupEntry.second) {
			newState.originalStates.insert(state.stateName);
		}

		newDFAStates.push_back(newState);

		for (auto& state : groupEntry.second) {
			stateMapping[state.stateName] = newState;
		}
	}

	// 生成新的 DFA 转移关系，确保没有重复的转移
	vector<DFATransition> newTransitions;
	unordered_set<string> addedTransitions;  // 用于记录已经添加的转移，避免重复

	for (auto& transition : dfaTransitions) {
		DFAState newFromState = stateMapping[transition.fromState.stateName];
		DFAState newToState = stateMapping[transition.toState.stateName];

		// 为转移生成唯一的标识
		string transitionKey = newFromState.stateName + "-" + string(1, transition.transitionSymbol) + "->" + newToState.stateName;

		// 检查这个转移是否已经添加过
		if (addedTransitions.find(transitionKey) == addedTransitions.end()) {
			DFATransition newTransition = { newFromState, newToState, transition.transitionSymbol };
			newTransitions.push_back(newTransition);
			addedTransitions.insert(transitionKey);  // 记录这个转移已经添加过
		}
	}

	// 最小化后的 DFA 状态和转换
	dfaStates = newDFAStates;
	dfaTransitions = newTransitions;
}

void displayminDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions) {
    cout << "DFA 状态集：\n";
    for (const auto& state : dfaStates) {
        cout << "状态: " << state.stateName;
        cout << " (NFA States: ";
        for (const auto& nfaState : state.originalStates) {
            cout << nfaState << " ";
        }
        cout << ")";
        cout << (state.isAccept ? " (接受状态)" : " (非接受状态)") << endl;
    }

    cout << "DFA 转移关系：\n";
    for (const auto& transition : dfaTransitions) {
        cout << transition.fromState.stateName << " --" << transition.transitionSymbol << "--> "
            << transition.toState.stateName << endl;
    }
}
//生成DFA的dot文件
void generateDotFile_minDFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	std::ofstream dotFile("mindfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph DFA {\n";
		dotFile << "  rankdir=LR;  // 横向布局\n\n";
		dotFile << " node [shape = circle];   // 状态节点\n\n";

		// 标记接受状态
		for (const auto& state : dfaStates) {
			if (state.isAccept) {
				dotFile << "  " << state.stateName << " [shape=doublecircle];\n";
			}
		}

		// 添加DFA状态
		for (const auto& state : dfaStates) {
			dotFile << "  " << state.stateName;
			dotFile << " [label=\"State " << state.stateName;
			if (state.isAccept) dotFile << "\\n(acceptState)";
			dotFile << "\"];\n";
		}
		dotFile << "\n";

		// 标记第二个状态 (即dfaStates[1]) 为起始状态
		// 假设第二个状态为a1
		if (dfaStates.size() > 1) {
			dotFile << "  " << dfaStates[1].stateName << " [shape=circle, style=bold, color=red]; // 起始状态\n";
		}

		// 添加DFA转移
		for (const auto& transition : dfaTransitions) {
			dotFile << "  " << transition.fromState.stateName << " -> "
				<< transition.toState.stateName
				<< " [label=\"" << transition.transitionSymbol << "\"];\n";
		}

		// 定义起始状态（箭头指向第二个状态a1）
		if (dfaStates.size() > 1) {
			dotFile << "  start [shape=point, width=0];\n";
			dotFile << "  start -> " << dfaStates[1].stateName << ";\n";
		}

		dotFile << "}\n";

		dotFile.close();
		std::cout << "DFA DOT file generated successfully.\n";
	}
	else {
		std::cerr << "Unable to open DOT file.\n";
	}
}

// DFA 模拟
bool simulateDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions, const string& inputString) {
	// 将初始状态改为第二个状态，注意索引从 0 开始，所以下标 1 是第二个状态
	DFAState currentState = dfaStates[1];  // 初始状态改为第二个状态

	// 遍历输入字符串的每个字符
	for (char ch : inputString) {
		bool foundTransition = false;

		// 查找当前状态的转移
		for (const auto& transition : dfaTransitions) {
			if (transition.fromState.stateName == currentState.stateName && transition.transitionSymbol == ch) {
				currentState = transition.toState;  // 更新当前状态
				foundTransition = true;
				break;
			}
		}

		// 如果找不到转移，则字符串不能被接受
		if (!foundTransition) {
			return false;  // 没有有效的转移，拒绝输入字符串
		}
	}

	// 如果最后状态是接受状态，则接受输入
	return currentState.isAccept;
}


