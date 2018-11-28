#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <exception>
#include <map>
#include <set>
#include <list>
#include <queue>
#include "Token.h"


#define MAX_PRIORITY 'z'
#define BRACKET_PRIORITY 5 //TODO smt with that

std::map<char, std::set<int> > etalon_Transitions;
std::map<char, std::set<int> > dfa_etalon_Inputs;
int kostyl_max_state = 0;


class NFA {
public:
	std::map<int, std::map<char, std::set<int>>> states = {};
	int startState;
	int finalState;	
	   	
	std::map<char, std::set<int>> inputMap;
	std::vector<char> inputs;
	//TODO список inputov
	std::map<char, std::set<int>> getTransitionMap() {
		if (inputMap.empty()) {
			inputMap = states[0];
		}
		return inputMap;
	}

	std::vector<char> getInputs() {
		if (inputs.empty()) {
			for (auto i : states[0]) {
				inputs.push_back(i.first);
			}

		}
		return inputs;
	}
};

class DFA {
public:
	std::map<int, std::map<char, std::set<int>>> states = {};
	int startState;
	std::set<int> finalStates;
	std::vector<char> inputs;

	bool is_empty() {
		return states.empty();
	}

	bool is_final(int state) {
		return (this->finalStates.find(state) != this->finalStates.end());
	}

	bool is_input(char c) {

		for (char t : this->getInputs()) {
			if (c == t)
				return true;
		}
		return false;

	}

	std::vector<char> getInputs() {
		if (inputs.empty()) {
			for (auto i : states[0]) {
				inputs.push_back(i.first);
			}

		}
		return inputs;
	}

	void printDFA() {
		std::cout << "\n\nDFA:\n";
		std::cout << "\nFinal: ";
		for (auto i : finalStates)
			std::cout << i << " ";

		std::cout << "\n        ";
		for (auto t : states[0]) {

			std::cout << t.first << "       ";
		}
		std::cout << '\n';

		for (auto & state : states) {
			std::cout << "State " << state.first << ": ";
			for (auto & tr : state.second) {
				for (auto s : tr.second) {
					std::cout << s << " ";
				}
				std::cout << "        ";
			}
			std::cout << std::endl;
		}


		
	}
	
	void clear() {
		states.clear();
		startState = -1;
		finalStates.clear();
		inputs.clear();
	}
};

struct Lexeme {
	std::string value;
	int priority;
};
struct Node {
	Node * left = nullptr;
	Node * right = nullptr;
	Token token; 
};

void destroyTree(Node* root) {
	if (root != nullptr) {
		destroyTree(root->left);
		destroyTree(root->right);
		delete root;
	}
}

Node* buildTree(std::vector<Lexeme> & exp, Node * root) {

	if (exp.empty()) {
		//??
	}

	if (root == nullptr) {
		root = new Node;
		root->left = nullptr;
		root->right = nullptr;
	}

	//find minimum priority value
	if (exp.size() == 1) {
		root->token = Token(exp[0].value);
		return root;
	}

	std::vector<Lexeme>::iterator it = exp.begin();
	std::vector<Lexeme>::iterator pos = it;

	for (it; it != exp.end(); ++it) {
		if (it->priority <= pos->priority) {
			pos = it;
		}
	}
	
	std::vector<Lexeme> left(exp.begin(), pos);
	std::vector<Lexeme> right(pos + 1, exp.end());

	root->token = Token(pos->value);
	root->left = !left.empty() ? buildTree(left, root->left) : nullptr;
	root->right = !right.empty() ? buildTree(right, root->right) : nullptr;
	return root;
}

 int getOperatorPriority(char c) {	 
	if (c == '|')
		return 1;
	if (c == '*')
		return 3;
	if (c == '_')
		return 2;
	return -1; //symbol
}

std::vector<Lexeme> parseExp(const std::string& exp) throw (std::runtime_error) {
	/*
		Takes a string-expression and returns parsed expression
		as a vector of Lexemes.
		 Lexemes contain the value and priority, so that no there are brackets included
	*/

	std::set<char> transSet;
	std::vector<Lexeme> result;
	std::string buf;
	int contextPriority = 0;
	char c;
	int it = 0;
	int max_priority = -2;
	int curPriprity = 0;

	//TODO preprocessing: concatenation -> _ ???

	c = '-'; // empty transition symbol
	transSet.insert(c);
	etalon_Transitions[c] = {};

	while (it < exp.length()) {
		c = exp[it];

		if (c == '(') {
			contextPriority+=BRACKET_PRIORITY;
			it++;
			continue;
		}

		if (c == ')') {
			contextPriority -= BRACKET_PRIORITY;;
			it++;
			continue;
		}
		
		if (std::isalpha(c)) {
			if (transSet.find(c) == transSet.end()) {
				transSet.insert(c);
				etalon_Transitions[c] = {};
			}
		}

		Lexeme l;
		l.value = c;
		curPriprity = getOperatorPriority(c);
		l.priority =  curPriprity == -1 ? curPriprity : contextPriority + curPriprity;
		if (l.priority > max_priority) max_priority = l.priority;
		result.push_back(l);		
		it++;
	}

	
	if (result.empty())
		throw  std::runtime_error("No regexp provided");
	if (contextPriority != 0)
		throw  std::runtime_error("Parsing error: Wrong brackets order");

	for (auto & v : result)
		v.priority = v.priority == -1 ? (max_priority+1): v.priority;

	return result;
}

void printTree(Node* root) {
	static std::string space = "";
	space += " ";
	if (root != nullptr) {
		printTree(root->left);
		std::cout << space + "-> " + root->token.field << '\n';
		printTree(root->right);
	}
	space.pop_back();
}

void printNFA(std::map<int, std::map<char, std::set<int>> >  & States, std::pair<int, int> sf) {
	std::cout << "\n\nNFA:\n";
	std::cout << "Start: " << sf.first << "\n" << "Final: " << sf.second << "\n";
	std::cout << "        ";
	for (auto t : States[0]) {
		
		std::cout << t.first << "       ";
	}
	std::cout << '\n';

	for (auto & state : States) {
		std::cout << "State " << state.first << ": ";
		for (auto & tr : state.second) {
			for (auto s : tr.second) {
				std::cout << s << " ";
			}
			std::cout <<  "        ";
		}
		std::cout << std::endl;
	}

}

std::pair<int, int> buildNFA(Node* root, std::map<int, std::map<char, std::set<int>> >  & States) {
	static int stateCounter = 0;

	//States - таблица состояний.
	//Transitions - таблица перехода из состояния по символу из алфавита

	//на каждом этапе подсасывает новые состояния к старым в зависимости от операции
	//TODO переделать токен!!! char поле

	//#cond
	if (root == nullptr)
	{
		kostyl_max_state = stateCounter;
		return { -1,-1 };
	}
	
	std::pair<int, int> boundsLeft = buildNFA(root->left, States);
	
	std::pair<int, int> boundsRight = buildNFA(root->right, States);
	
	

	//# check token
	char c = root->token.cfield;

	int finalState;
	int startState;
	if (std::isalpha(c))	{
		//# build M(a) 

		//States[stateCounter] = {};
		startState = stateCounter;
		finalState = stateCounter + 1;

		States[stateCounter] = etalon_Transitions;
		States[stateCounter+1] = etalon_Transitions;
		States[stateCounter][c].insert(stateCounter + 1); //переход по a
		/*startState = stateCounter;
		finalState = stateCounter + 1;*/
	
		stateCounter+=2;

		return { startState,finalState };
	}

	if (c == '|') {
		//# build M(a|b)
		startState = stateCounter;
		finalState = stateCounter + 1;

		States[startState] = etalon_Transitions;
		States[finalState] = etalon_Transitions;

		States[startState]['-'].insert(boundsLeft.first);// start -> start_a  
		States[startState]['-'].insert(boundsRight.first);//start -> start_b

		States[boundsLeft.second]['-'].insert(finalState);//finish_a -> finish
		States[boundsRight.second]['-'].insert(finalState); //finish_b -> finish

		stateCounter += 2;

		return { startState,finalState };
	}

	if (c == '_') {
		//# build M(ab)
		States[boundsLeft.second]['-'].insert(boundsRight.first);// finish_a ->start_b

		startState = boundsLeft.first;
		finalState = boundsRight.second;

		return { startState,finalState };
	}

	if (c == '*') {
		//assume LEFT subtree with value and right subtree empty
		//TODO проверка на пустоту поддерева. Используем то, что не пусто
		int start_a, final_a;
		if (boundsLeft.first==-1) {
			start_a = boundsRight.first;
			final_a = boundsRight.second;
		}
		else {
			start_a = boundsLeft.first;
			final_a = boundsLeft.second;
		}

		startState = stateCounter;
		finalState = stateCounter + 1;

		States[startState] = etalon_Transitions;
		States[finalState] = etalon_Transitions;
		
		States[startState]['-'].insert(finalState);//start -> finish
		States[startState]['-'].insert(start_a);//start -> start_a
		States[final_a]['-'].insert(start_a);//finish_a -> start_a
		States[final_a]['-'].insert(finalState);//finish_a -> finish

		stateCounter += 2;

		return { startState,finalState };
			
	}

	return { -1,-1 };
}

void deepSearch(int startState, char input, std::set<int>& unionStates,NFA &nfa) {
	std::map<int, std::map<char, std::set<int>>>&  statesTable = nfa.states;
	
	if (!statesTable[startState][input].empty()) {

		for (int nextState : statesTable[startState][input]){
			unionStates.insert(nextState);
			deepSearch(nextState, input, unionStates, nfa);
		}

	}
}

void notdeepSearch(int startState, char input, std::set<int>& unionStates, NFA &nfa) {
	std::map<int, std::map<char, std::set<int>>>&  statesTable = nfa.states;

	if (!statesTable[startState][input].empty()) {

		for (int nextState : statesTable[startState][input]) {
			unionStates.insert(nextState);
		}

	}
}

DFA skbuildDFA(NFA& nfa) {
	dfa_etalon_Inputs = etalon_Transitions;
	dfa_etalon_Inputs.erase('-');

	std::map<int, std::set<int>> dfaStatesMap;// key - dfa States, value - the set of nfa states

	std::vector<char> allInputs;
	for (auto i : dfa_etalon_Inputs)
		allInputs.push_back(i.first);


	std::queue<std::set<int>> Q;
	std::set<int> set;
	std::set<int> unionSet;
	DFA dfa;

	int stCount = 0;
	int linkState = 0;
	int curState = 0;
	bool f = true;

	//#init start set 
	unionSet.insert(nfa.startState);
	deepSearch(nfa.startState, '-', unionSet, nfa);

	dfaStatesMap[stCount] = unionSet;
	Q.push(unionSet);

	while (!Q.empty()) {
		set = Q.front();
		dfa.states[curState] = dfa_etalon_Inputs;

		for (char inp : allInputs) {
			unionSet.clear();

			//# create new union states as e-closure(move(s)) by inp
			for (int s : set) {
				notdeepSearch(s, inp, unionSet, nfa);//move(s,inp)
			}
			std::set<int> TunionSet = unionSet;
			for (int s : TunionSet) {
				deepSearch(s, '-', unionSet, nfa);//e-closure(move(s,inp))
			}

			if (unionSet.empty()) 
				continue;
			
			//# verify set to be unique
			linkState = ++stCount;
			f = true;
			for (int i = 0; i < dfaStatesMap.size(); i++) {
				if (dfaStatesMap[i] == unionSet) {
					f = false;
					linkState = i;
					stCount--;
					break;
				}
			}

			if (f) {				
				Q.push(unionSet);
				//#final state check
				if (unionSet.find(nfa.finalState) != unionSet.end())
					dfa.finalStates.insert(stCount);
				dfaStatesMap[stCount] = unionSet;
			}

			dfa.states[curState][inp].insert(linkState);
		}
		Q.pop();
		curState++;

	}
	return dfa;
}

void handleRegExp(std::string regexp,DFA & dfa) {

	//# parse regexp
	std::vector<Lexeme> parsedExp;
	try {
		parsedExp = parseExp(regexp);
	}
	catch (std::runtime_error const &e) {
		std::cout << e.what() << std::endl;
		return;
	}

	
	//# build tree
	Node * tree = nullptr;
	tree = buildTree(parsedExp, tree);


	//# create NFA
	std::map<int, std::map<char, std::set<int>> > States;
	std::pair<int, int> sf = buildNFA(tree, States);

	NFA nfa;
	nfa.states = States;
	nfa.startState = sf.first;
	nfa.finalState = sf.second;

	printNFA(States,sf);

	destroyTree(tree);

	//#create DFA
	dfa = skbuildDFA(nfa);
	dfa.printDFA();

}

bool confundus(const std::string& s, DFA &dfa) {
	int curState = dfa.startState;
	std::set<int> finalStates = dfa.finalStates;
	std::set<int> buf;
	bool fit = true;

	for (char c : s) {		
		
		if (!dfa.is_input(c) ) {
			//нет инпута
			return false;
		}

		buf = dfa.states[curState][c];

		if (buf.empty()) {
			//нет перехода  
			return false;
		}

		//есть переход
		for (int i : buf) {
			curState = i;
		}
		

	}
	return dfa.is_final(curState);

	


}


int main() {

	std::string s;
	DFA dfa;	
	do {
		std::cout << "Enter regular expression\n>> ";
		std::cin >> s;
		handleRegExp(s, dfa);
	} while (dfa.is_empty());

	std::cout << std::endl << "Fitting strings\n";

	while (1) {
		std::cout << ">> ";
		std::cin >> s;
		if (s == "exit")
			break;

		std::cout << "Fit: " << (confundus(s, dfa) ? "yes" : "no") << std::endl;

	}

	return EXIT_SUCCESS;
}	