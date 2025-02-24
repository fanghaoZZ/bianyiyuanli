#include "head.h"

int nodeNum = 0;

//�����½ڵ�
node new_node()
{
	node newNode;
	newNode.nodeName = nodeNum + 65;//�������ô�д��ĸ��ʾ
	nodeNum++;
	return newNode;
}

//��������������ʽ
void input(string& RE)
{
	cout << "����������ʽ��  ����������() * |;�ַ�����a~z A~Z��" << endl;
	cin >> RE;
}

//��ɵ�Ԫ��������
void elem_copy(elem& dest, elem source)
{
	for (int i = 0; i < source.edgeCount; i++) {
		dest.edgeSet[dest.edgeCount + i] = source.edgeSet[i];
	}
	dest.edgeCount += source.edgeCount;
}

//���� a
elem act_Elem(char c)
{
	//�½ڵ�
	node startNode = new_node();
	node endNode = new_node();

	//�±�
	edge newEdge;
	newEdge.startName = startNode;
	newEdge.endName = endNode;
	newEdge.tranSymbol = c;

	//��NFA���Ԫ�أ�С��NFAԪ��/��Ԫ)
	elem newElem;
	newElem.edgeCount = 0;	//��ʼ״̬
	newElem.edgeSet[newElem.edgeCount++] = newEdge;
	newElem.startName = newElem.edgeSet[0].startName;
	newElem.endName = newElem.edgeSet[0].endName;

	return newElem;
}
//����a|b
elem act_Unit(elem fir, elem sec)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;

	//����µ�״̬�ڵ�
	node startNode = new_node();
	node endNode = new_node();

	//����e1����������AB����ʼ��A��
	edge1.startName = startNode;
	edge1.endName = fir.startName;
	edge1.tranSymbol = '#';

	//����e2����������CD����ʼ��C��
	edge2.startName = startNode;
	edge2.endName = sec.startName;
	edge2.tranSymbol = '#';

	//����e3������AB���յ���յ㣩
	edge3.startName = fir.endName;
	edge3.endName = endNode;
	edge3.tranSymbol = '#';

	//����e4������CD���յ���յ㣩
	edge4.startName = sec.endName;
	edge4.endName = endNode;
	edge4.tranSymbol = '#';

	//��fir��sec�ϲ�
	elem_copy(newElem, fir);
	elem_copy(newElem, sec);

	//�¹�����4����
	newElem.edgeSet[newElem.edgeCount++] = edge1;
	newElem.edgeSet[newElem.edgeCount++] = edge2;
	newElem.edgeSet[newElem.edgeCount++] = edge3;
	newElem.edgeSet[newElem.edgeCount++] = edge4;

	newElem.startName = startNode;
	newElem.endName = endNode;

	return newElem;
}
//���� N(s)N(t)
elem act_join(elem fir, elem sec)
{
	//��fir�Ľ���״̬��sec�Ŀ�ʼ״̬�ϲ�����sec�ı߸��Ƹ�fir����fir����
	//��sec��������StartState��ͷ�ı�ȫ���޸�
	for (int i = 0; i < sec.edgeCount; i++) {
		if (sec.edgeSet[i].startName.nodeName.compare(sec.startName.nodeName) == 0)
		{
			sec.edgeSet[i].startName = fir.endName; //�ñ�e1�Ŀ�ʼ״̬����N(t)����ʼ״̬
		}
		else if (sec.edgeSet[i].endName.nodeName.compare(sec.startName.nodeName) == 0) {
			sec.edgeSet[i].endName = fir.endName; //�ñ�e2�Ľ���״̬����N(t)����ʼ״̬
		}
	}
	sec.startName = fir.endName;

	elem_copy(fir, sec);

	//��fir�Ľ���״̬����Ϊsec�Ľ���״̬
	fir.endName = sec.endName;
	return fir;
}
//����a*
elem act_star(elem Elem)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;

	//�����״̬�ڵ�
	node startNode = new_node();
	node endNode = new_node();

	//e1
	edge1.startName = startNode;
	edge1.endName = endNode;
	edge1.tranSymbol = '#';	//�հ�ȡ�մ�

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

	//������Ԫ
	elem_copy(newElem, Elem);

	//���¹����������߼���EdgeSet
	newElem.edgeSet[newElem.edgeCount++] = edge1;
	newElem.edgeSet[newElem.edgeCount++] = edge2;
	newElem.edgeSet[newElem.edgeCount++] = edge3;
	newElem.edgeSet[newElem.edgeCount++] = edge4;

	//����NewElem����ʾ״̬�ͽ���״̬
	newElem.startName = startNode;
	newElem.endName = endNode;

	return newElem;
}
//�ж��Ƿ�����ĸ
int is_letter(char check) {
	if (check >= 'a' && check <= 'z' || check >= 'A' && check <= 'Z')
		return true;
	return false;
}
//������ӷ���
string add_join_symbol(string add_string)
{
	int length = add_string.size();
	int return_string_length = 0;
	char* return_string = new char[2 * length + 2];//���������
	char first, second;
	for (int i = 0; i < length - 1; i++)
	{
		first = add_string.at(i);
		second = add_string.at(i + 1);
		return_string[return_string_length++] = first;
		//Ҫ�ӵĿ�������ab �� *b �� a( �� )b �����
		//���ڶ�������ĸ����һ������'('��'|'��Ҫ���
		if (first != '(' && first != '|' && is_letter(second))
		{
			return_string[return_string_length++] = '+';
		}
		//���ڶ�����'(',��һ������'|'��'(',ҲҪ��
		else if (second == '(' && first != '|' && first != '(')
		{
			return_string[return_string_length++] = '+';
		}
	}
	//�����һ���ַ�д��second
	return_string[return_string_length++] = second;
	return_string[return_string_length] = '\0';
	string STRING(return_string);
	cout << "��'+'��ı��ʽ��" << STRING << endl;
	return STRING;
}

//����ĸ���Ԫ�ض���
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
		cerr << "error: ����δ֪���ţ�" << endl;
		exit(1);  // �쳣�˳�
	}
	return priority;
}

int infixToPostfix::icpFunc(char c) {
	int priority = icp.count(c) ? icp[c] : -1;
	if (priority == -1) {
		cerr << "error: ����δ֪���ţ�" << endl;
		exit(1);  // �쳣�˳�
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
			if (ispFunc(c1) < icpFunc(c2)) { // ջ�����������ȼ����ڵ�ǰ�ַ�������ǰ�ַ���ջ
				stack.push(c2);
				loc++;
			}
			else if (ispFunc(c1) > icpFunc(c2)) {  // ջ�����������ȼ����ڵ�ǰ�ַ�����ջ����������ջ����ӵ���׺���ʽ
				postfix += c1;
				stack.pop();
			}
			else {
				if (c1 == '#' && c2 == '#') { // �������� #�����ʽ����
					break;
				}
				stack.pop(); //��������������������ʱ������������ų�ջ�������Ų���ջ
				loc++;
			}
		}
	}
}

string infixToPostfix::getResult() {
	postfix = ""; // ��ս��
	inToPost();
	return postfix;
}

//���ʽתNFA������,�������յ�NFA����
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

//��ӡNFA
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

//����NFAdot�ļ�
void generateDotFile_NFA(const elem& nfa) {
	std::ofstream dotFile("nfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph NFA {\n";
		dotFile << "  rankdir=LR;  // ���򲼾�\n\n";
		dotFile << " node [shape = circle];   // ״̬�ڵ�\n\n";

		dotFile << nfa.endName.nodeName << " [shape=doublecircle];\n";
		// ��� NFA ״̬
		dotFile << "  " << nfa.startName.nodeName << " [label=\"Start State: " << nfa.startName.nodeName << "\"];\n";
		dotFile << "  " << nfa.endName.nodeName << " [label=\"End State: " << nfa.endName.nodeName << "\"];\n";

		// ��� NFA ת��
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


// ���� NFA ״̬�Ħűհ�
DFAState eClosure(const set<string>& nfaStates, elem nfa) {
	DFAState eClosureState;
	eClosureState.nfaStates = nfaStates;

	stack<string> stateStack;

	// ��ʼ��ջ������ʼ״̬����ջ���ʼnfaState��ֻ��NFA_Elem.startName
	for (const string& nfaState_name : nfaStates) {
		stateStack.push(nfaState_name);
	}

	while (!stateStack.empty()) {
		string currentState = stateStack.top();
		stateStack.pop();

		// ���� NFA �ı�
		for (int i = 0; i < nfa.edgeCount; i++) {
			edge currentEdge = nfa.edgeSet[i];

			// ����ߵ���ʼ״̬�ǵ�ǰ״̬�����ұߵ�ת��������#����ô��Ŀ��״̬����űհ�
			if (currentEdge.startName.nodeName == currentState && currentEdge.tranSymbol == '#') {
				// ���Ŀ��״̬�Ƿ��Ѿ��ڦűհ��У������ظ����
				if (eClosureState.nfaStates.find(currentEdge.endName.nodeName) == eClosureState.nfaStates.end()) {
					eClosureState.nfaStates.insert(currentEdge.endName.nodeName);
					// ��Ŀ��״̬����ջ�Ա��һ������
					stateStack.push(currentEdge.endName.nodeName);
				}
			}
		}
	}

	// Ϊ�űհ�����һ��Ψһ������
	for (const string& nfaState_name : eClosureState.nfaStates) {
		eClosureState.stateName += nfaState_name;
	}

	return eClosureState;
}

//move����
DFAState move(const DFAState& dfaState, char transitionSymbol, elem nfa) {
	DFAState nextState;

	// ���� DFAState �е�ÿ�� NFA ״̬
	for (const string& nfaState_name : dfaState.nfaStates) {
		// ������������� NFA ״̬�ı�
		for (int i = 0; i < nfa.edgeCount; i++) {
			edge currentEdge = nfa.edgeSet[i];

			// ����ߵ���ʼ״̬�ǵ�ǰ״̬���ұߵ�ת�����ŵ���������ţ���Ŀ��״̬���� nextState
			if (currentEdge.startName.nodeName == nfaState_name && currentEdge.tranSymbol == transitionSymbol && currentEdge.tranSymbol != '#') {
				nextState.nfaStates.insert(currentEdge.endName.nodeName);
			}
		}
	}

	// Ϊ nextState ����һ��Ψһ������
	for (const string& nfaState_name : nextState.nfaStates) {
		nextState.stateName += nfaState_name;
	}

	return nextState;
}

// ��� DFA ״̬�Ƿ���״̬������,��dfaStates����û���ҵ�targetState
bool isDFAStateInVector(const vector<DFAState>& dfaStates, const DFAState& targetState) {
	for (const DFAState& state : dfaStates) {
		if (state.stateName == targetState.stateName) {
			return true; // �ҵ�ƥ���״̬
		}
	}
	return false; // û���ҵ�ƥ���״̬
}

//���ת�����Ƿ��ڱ߼����У�����a->b�Ƿ��Ѿ��ڼ�����
bool isTransitionInVector(DFAState dfaState, DFAState dfaNextState, char symbol, vector<DFATransition> dfaTransitions)
{
	for (const DFATransition& transition : dfaTransitions) {
		if (transition.fromState.stateName == dfaState.stateName && dfaNextState.stateName == dfaNextState.stateName && symbol == transition.transitionSymbol) {
			return true;	//�ҵ�ƥ���״̬
		}
	}
	return false;
}

void buildDFAFromNFA(const elem& NFA_Elem, vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	// ��ʼ�� DFA ״̬���Ϻ�ת����ϵ
	set<string> nfaInitialStateSet;
	nfaInitialStateSet.insert(NFA_Elem.startName.nodeName);
	DFAState dfaInitialState = eClosure(nfaInitialStateSet, NFA_Elem); // ���� NFA ��ʼ״̬�� �űհ�

	// �ж��Ƿ���� NFA �� endName ״̬��ȷ���Ƿ�Ϊ����״̬
	for (const string& nfaState : dfaInitialState.nfaStates) {
		if (nfaState == NFA_Elem.endName.nodeName) {
			dfaInitialState.isAccept = true;  // ������� endName�����Ϊ����״̬
			break;
		}
	}

	dfaStates.push_back(dfaInitialState);

	// ��ʼ���� DFA
	for (int i = 0; i < dfaStates.size(); i++) {
		DFAState dfaState = dfaStates[i];
		for (int j = 0; j < NFA_Elem.edgeCount; j++) {
			char symbol = NFA_Elem.edgeSet[j].tranSymbol;
			DFAState nextState = move(dfaState, symbol, NFA_Elem);
			DFAState dfaNextState = eClosure(nextState.nfaStates, NFA_Elem);

			if (!nextState.nfaStates.empty()) {
				// �����һ��״̬��Ϊ�գ����� DFA ״̬�����л�δ��ӣ������ DFA ״̬����
				if (!isDFAStateInVector(dfaStates, dfaNextState)) {
					// ����Ƿ�Ϊ����״̬
					for (const string& nfaState : dfaNextState.nfaStates) {
						if (nfaState == NFA_Elem.endName.nodeName) {
							dfaNextState.isAccept = true;  // ������� endName�����Ϊ����״̬
							break;
						}
					}
					dfaStates.push_back(dfaNextState);
				}

				// ���ڱ�ҲҪȥ��
				if (!isTransitionInVector(dfaState, dfaNextState, symbol, dfaTransitions)) {
					dfaTransitions.push_back({ dfaState, dfaNextState, symbol });
				}
			}
		}
	}
}

// ��ʾ DFA ״̬��ת�ƹ�ϵ��������ʼ�ͽ���״̬
void displayDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions) {
	cout << "DFA States:" << endl;

	for (const DFAState& state : dfaStates) {
		cout << "State " << state.stateName << " (NFA States: ";
		for (const string& nfaState_name : state.nfaStates) {
			cout << nfaState_name << " ";
		}
		cout << ")";

		// ��ʾ�Ƿ��ǽ���״̬
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

//����DFA��dot�ļ�
void generateDotFile_DFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	std::ofstream dotFile("dfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph DFA {\n";
		dotFile << "  rankdir=LR;  // ���򲼾�\n\n";
		dotFile << " node [shape = circle];   // ��ʼ״̬\n\n";

		// ��ǽ���״̬
		for (const auto& state : dfaStates) {
			if (state.isAccept) {
				dotFile << "  " << state.stateName << " [shape=doublecircle];\n";
			}
		}

		// ���DFA״̬
		for (const auto& state : dfaStates) {
			dotFile << "  " << state.stateName;
			dotFile << " [label=\"State " << state.stateName;
			if (state.stateName == dfaStates.front().stateName) dotFile << "\\n(startState)";
			if (state.isAccept) dotFile << "\\n(acceptState)";
			dotFile << "\"];\n";
		}
		dotFile << "\n";

		// ���DFAת��
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
    return s1 == s2;  // ʹ�� == ���������鼯���Ƿ����
}

void minimizeDFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	unordered_map<string, set<DFAState>> stateGroups;
	set<DFAState> acceptStates;
	set<DFAState> rejectStates;

	// ��״̬��Ϊ����״̬�ͷǽ���״̬
	for (auto& state : dfaStates) {
		if (state.isAccept) {
			acceptStates.insert(state);
		}
		else {
			rejectStates.insert(state);
		}
	}

	// ��ʼ״̬���飺���ݽ���״̬�ͷǽ���״̬
	stateGroups["accept"] = acceptStates;
	stateGroups["reject"] = rejectStates;

	bool splitOccurred = true;
	while (splitOccurred) {
		splitOccurred = false;
		unordered_map<string, set<DFAState>> newStateGroups;

		// ��ÿ��״̬�����ϸ��
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

				// ʹ��ת�ƹ�ϵ��Ψһ��ʶ������
				string transitionKey;
				for (auto& entry : transitionMap) {
					transitionKey += entry.first + entry.second;
				}

				transitionGroups[transitionKey].insert(state);
			}

			// ������ڶ����ͬ��ת���飬˵����Ҫ����ϸ��
			if (transitionGroups.size() > 1) {
				splitOccurred = true;
			}

			// ��ϸ�ֺ��״̬����µ��µ�״̬������
			for (auto& transitionGroup : transitionGroups) {
				newStateGroups[transitionGroup.first] = transitionGroup.second;
			}
		}

		// ����״̬����
		stateGroups = newStateGroups;
	}

	// �����µ� DFA ״̬�ͼ�¼ԭʼ״̬����
	vector<DFAState> newDFAStates;
	unordered_map<string, DFAState> stateMapping;
	int newStateCount = 0;

	for (auto& groupEntry : stateGroups) {
		DFAState newState;
		newState.stateName = "a" + to_string(newStateCount++);
		newState.isAccept = groupEntry.second.begin()->isAccept;

		// ��¼ԭʼ״̬����
		for (auto& state : groupEntry.second) {
			newState.originalStates.insert(state.stateName);
		}

		newDFAStates.push_back(newState);

		for (auto& state : groupEntry.second) {
			stateMapping[state.stateName] = newState;
		}
	}

	// �����µ� DFA ת�ƹ�ϵ��ȷ��û���ظ���ת��
	vector<DFATransition> newTransitions;
	unordered_set<string> addedTransitions;  // ���ڼ�¼�Ѿ���ӵ�ת�ƣ������ظ�

	for (auto& transition : dfaTransitions) {
		DFAState newFromState = stateMapping[transition.fromState.stateName];
		DFAState newToState = stateMapping[transition.toState.stateName];

		// Ϊת������Ψһ�ı�ʶ
		string transitionKey = newFromState.stateName + "-" + string(1, transition.transitionSymbol) + "->" + newToState.stateName;

		// ������ת���Ƿ��Ѿ���ӹ�
		if (addedTransitions.find(transitionKey) == addedTransitions.end()) {
			DFATransition newTransition = { newFromState, newToState, transition.transitionSymbol };
			newTransitions.push_back(newTransition);
			addedTransitions.insert(transitionKey);  // ��¼���ת���Ѿ���ӹ�
		}
	}

	// ��С����� DFA ״̬��ת��
	dfaStates = newDFAStates;
	dfaTransitions = newTransitions;
}

void displayminDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions) {
    cout << "DFA ״̬����\n";
    for (const auto& state : dfaStates) {
        cout << "״̬: " << state.stateName;
        cout << " (NFA States: ";
        for (const auto& nfaState : state.originalStates) {
            cout << nfaState << " ";
        }
        cout << ")";
        cout << (state.isAccept ? " (����״̬)" : " (�ǽ���״̬)") << endl;
    }

    cout << "DFA ת�ƹ�ϵ��\n";
    for (const auto& transition : dfaTransitions) {
        cout << transition.fromState.stateName << " --" << transition.transitionSymbol << "--> "
            << transition.toState.stateName << endl;
    }
}
//����DFA��dot�ļ�
void generateDotFile_minDFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	std::ofstream dotFile("mindfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph DFA {\n";
		dotFile << "  rankdir=LR;  // ���򲼾�\n\n";
		dotFile << " node [shape = circle];   // ״̬�ڵ�\n\n";

		// ��ǽ���״̬
		for (const auto& state : dfaStates) {
			if (state.isAccept) {
				dotFile << "  " << state.stateName << " [shape=doublecircle];\n";
			}
		}

		// ���DFA״̬
		for (const auto& state : dfaStates) {
			dotFile << "  " << state.stateName;
			dotFile << " [label=\"State " << state.stateName;
			if (state.isAccept) dotFile << "\\n(acceptState)";
			dotFile << "\"];\n";
		}
		dotFile << "\n";

		// ��ǵڶ���״̬ (��dfaStates[1]) Ϊ��ʼ״̬
		// ����ڶ���״̬Ϊa1
		if (dfaStates.size() > 1) {
			dotFile << "  " << dfaStates[1].stateName << " [shape=circle, style=bold, color=red]; // ��ʼ״̬\n";
		}

		// ���DFAת��
		for (const auto& transition : dfaTransitions) {
			dotFile << "  " << transition.fromState.stateName << " -> "
				<< transition.toState.stateName
				<< " [label=\"" << transition.transitionSymbol << "\"];\n";
		}

		// ������ʼ״̬����ͷָ��ڶ���״̬a1��
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

// DFA ģ��
bool simulateDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions, const string& inputString) {
	// ����ʼ״̬��Ϊ�ڶ���״̬��ע�������� 0 ��ʼ�������±� 1 �ǵڶ���״̬
	DFAState currentState = dfaStates[1];  // ��ʼ״̬��Ϊ�ڶ���״̬

	// ���������ַ�����ÿ���ַ�
	for (char ch : inputString) {
		bool foundTransition = false;

		// ���ҵ�ǰ״̬��ת��
		for (const auto& transition : dfaTransitions) {
			if (transition.fromState.stateName == currentState.stateName && transition.transitionSymbol == ch) {
				currentState = transition.toState;  // ���µ�ǰ״̬
				foundTransition = true;
				break;
			}
		}

		// ����Ҳ���ת�ƣ����ַ������ܱ�����
		if (!foundTransition) {
			return false;  // û����Ч��ת�ƣ��ܾ������ַ���
		}
	}

	// ������״̬�ǽ���״̬�����������
	return currentState.isAccept;
}


