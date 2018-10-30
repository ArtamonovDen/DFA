#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

#include "Token.h"


#define MAX_PRIORITY 'z'



struct Lexeme {
	std::string value;
	std::string priority;
};
struct Node {
	Node * left = nullptr;
	Node * right = nullptr;
	Token token; 
};

void destroyTree(Node* root) {

	if (root == nullptr)
		return;
	destroyTree(root->left);
	destroyTree(root->right);
	delete root;
}

Node* buildTree(std::vector<Lexeme> & exp, Node * root) {

	if (root == nullptr) {
		root = new Node;
		root->left = nullptr;
		root->right = nullptr;
	}

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
	root->left = buildTree(left, root->left);
	root->right = buildTree(right, root->right);
	return root;
}


char getOperatorPriority(char c) {
	if (c == '|')
		return 'a';
	if (c == '*')
		return 'b';
	return 'z';
}
std::vector<Lexeme> parseExp(const std::string& exp) {
	/*
		Takes a string-expression and returns parsed expression
		as a vector of Lexemes.
		 Lexemes contain the value and priority, so that no brackets included
	*/

	std::vector<Lexeme> result;
	std::string buf;
	int contextPriority = 0;
	bool processSym = false; //true while handling a variable
	char c;
	unsigned int it = 0;

	while (it < exp.length()) {
		c = exp[it];
		if (processSym) {

			if (std::isalpha(c))
			{
				buf += c;
				it++;
				continue;
			}
			else {
				Lexeme l;
				l.value = buf;
				l.priority = std::to_string(contextPriority) + MAX_PRIORITY;
				result.push_back(l);
				buf.clear();
				processSym = false;
			}
		}

		if (c == '(') {
			contextPriority++;
			processSym = false;
			it++;
			continue;
		}
		if (c == ')') {
			processSym = false;
			contextPriority--;
			it++;
			continue;
		}

		if (std::isalpha(c)) {
			//start variable processing
			processSym = true;
			buf += c;
			it++;
			continue;
		}

		//operator processing
		Lexeme l;
		l.value = c;
		l.priority = std::to_string(contextPriority) + getOperatorPriority(c);
		result.push_back(l);

		it++;
	}


	if (processSym) {
		Lexeme l;
		l.value = buf;
		l.priority = std::to_string(contextPriority) + MAX_PRIORITY;
		result.push_back(l);

	}
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
	std::vector<Lexeme> parsedExp = parseExp(regexp);
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