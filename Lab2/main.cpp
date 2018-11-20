
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <exception>
#include <map>
#include <set>
#include "Token.h"


#define MAX_PRIORITY 'z'
#define BRACKET_PRIORITY 5

std::map<char, std::set<int> > etalon_Transitions;


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
	
	//left -pos- right
	//TODO: tree with empty right or left part: c++ or a*

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


std::pair<int, int> buildNFA(Node* root, std::map<int, std::map<char, std::set<int>> >  & States) {
	static int stateCounter = 0;
	//��������� ���������
	//��� ������ � ������
	// � �� buildNFA(buildNFA(left), buildNFA(right))

	//States - ������� ���������.
	//Transitions - ������� �������� �� ��������� �� ������� �� ��������

	//�� ������ ����� ����������� ����� ��������� � ������ � ����������� �� ��������
	//���� ���-�� ����������� ��������� � ��������� ���������
	//��������� ���� ���������� ��������� ���������� ��� ������� �������� � ���������� �
	//���������� �����!!!

	//#cond

	std::pair<int, int> boundsLeft = buildNFA(root->left, States);
	std::pair<int, int> boundsRight = buildNFA(root->right, States);

	//# check token
	char c = root->token.cfield;

	int finalState;
	int startState;
	if (std::isalpha( root->token.is_operator))
	{
		//# build M(a) 
		 //���������� ��� ����!
		//States[stateCounter] = {};
		startState = stateCounter;
		finalState = stateCounter + 1;

		States[stateCounter] = etalon_Transitions;
		States[stateCounter+1] = etalon_Transitions;
		States[stateCounter][c].insert(stateCounter + 1); //������� �� a
		startState = stateCounter;
		finalState = stateCounter + 1;
	
		stateCounter+=2;

		return { startState,finalState };
	}

	if (c == '|') {
		//# build M(a|b)
		startState = stateCounter;
		finalState = stateCounter + 1;

		States[startState] = etalon_Transitions;
		States[finalState] = etalon_Transitions;

		States[startState]['-'].insert(boundsLeft.first);
		States[startState]['-'].insert(boundsRight.first);

		States[boundsLeft.second]['-'].insert(finalState);
		States[boundsRight.second]['-'].insert(finalState);

		stateCounter += 2;

		return { startState,finalState };

	}

	if (c == '_') {
		//# build M(ab)

		//assume left subtree with value and right subtree empty

	}

	if (c == '*') {
		startState
	}

	if (c == '-') {

	}
}


void acceptRegExp(std::string regexp) {
	std::vector<Lexeme> parsedExp;
	try {

		parsedExp = parseExp(regexp);
	}
	catch (std::runtime_error const &e) {
		std::cout << e.what() << std::endl;
		return;
	}



	Node * tree = nullptr;
	tree = buildTree(parsedExp, tree);
	printTree(tree);
	destroyTree(tree);

	//# create state table
}

int main() {

	//get reg exp
	acceptRegExp("(a|b*)");


	std::cin.get();
	return EXIT_SUCCESS;
}


//TODO 
//enter  regexp 
//parse it 
	//������� ����� � ������� ->  � �������
	//--------------------------
	//|					|������� �� ���� 
	//|��������� ���������|
	//|
	//|


//build NFA
//build DFA
//UI for entering and parse words with DFA


//TREE:
//operations:
//	brackets
//	concatenate 
//	*
//	|


	