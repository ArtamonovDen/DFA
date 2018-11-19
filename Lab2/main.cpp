#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <exception>

#include "Token.h"


#define MAX_PRIORITY 'z'
#define BRACKET_PRIORITY 5

//не очень круто хардкодить максимальный приоритет операции, тк он может изменится


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

	//if (root == nullptr)
	//	return;
	//destroyTree(root->left);
	//destroyTree(root->right);
	//delete root;

	if (root != nullptr) {
		destroyTree(root->left);
		destroyTree(root->right);
		delete root;
	}
}

Node* buildTree(std::vector<Lexeme> & exp, Node * root) {

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
	return 2;//concatenation
}
std::vector<Lexeme> parseExp(const std::string& exp) {
	/*
		Takes a string-expression and returns parsed expression
		as a vector of Lexemes.
		 Lexemes contain the value and priority, so that no there are brackets included
	*/
	//UPDATE LOGIC OF PRIORITY ASSUMING: set bracket priority as the max priority + 2
	//UPDATE eliminating handling of many character variables and numbers
	//UPDATE symbol's priority is -1
	//May save all the possible DFA and NFA jump symbol

	//теперь идём только по симоволам - нет многосимвольных переменных и чисел!

	std::vector<Lexeme> result;
	std::string buf;
	int contextPriority = 0;
	bool processSym = false; //true while handling a variable
	char c;
	int it = 0;

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
			Lexeme l;
			l.value = c;
			l.priority = -1;
			result.push_back(l);
			it++;
			continue;
		}

		Lexeme l;
		l.value = c;
		l.priority = contextPriority + getOperatorPriority(c);
		result.push_back(l);
		it++;
	}

	if (contextPriority != 0)
		throw new std::runtime_error("Parsing error: Wrong brackets order");
	if (result.empty)
		throw new std::runtime_error("No regexp provided");

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

void acceptRegExp(std::string regexp) {
	std::vector<Lexeme> parsedExp;
	try {

		parsedExp = parseExp(regexp);
	}
	catch (std::runtime_error &e) {
		std::cout << e.what() << std::endl;
	}
	Node * tree = nullptr;
	tree = buildTree(parsedExp, tree);
	printTree(tree);
	destroyTree(tree);
}

int main() {

	//get reg exp
	acceptRegExp("a*");

	
	return EXIT_SUCCESS;
}


//TODO 
//enter  regexp 
//parse it 
	парсить сразу в автомат ->  в таблицу
	--------------------------
	|					|символы на вход 
	|множество состояний|
	|
	|


//build NFA
//build DFA
//UI for entering and parse words with DFA


//TREE:
//operations:
//	brackets
//	concatenate 
//	*
//	|

int 

	