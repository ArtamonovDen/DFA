#pragma once
#include <string>
#include <map>
#include <cctype>
#include <math.h>

class Token {
public:
	std::string field;
	static std::map<std::string, double(*)(double, double)> operationMap;
	static std::map<std::string, double> variableMap;
	char cfield = field[0];
	Token() { field = ""; }
	Token(std::string op) :field(op) {  }

	


	static void fillOperationMap() {
		std::pair<std::string, double(*)(double, double)> p;
		p.first = "+";
		p.second = Token::op_plus;
		operationMap.insert(p);

		p.first = "-";
		p.second = Token::op_minus;
		operationMap.insert(p);

		p.first = "*";
		p.second = Token::op_multiply;
		operationMap.insert(p);

		p.first = "/";
		p.second = Token::op_devision;
		operationMap.insert(p);

		p.first = "%";
		p.second = Token::op_mod;
		operationMap.insert(p);

		p.first = "^";
		p.second = Token::op_elevate;
		operationMap.insert(p);

		p.first = "|";
		p.second = Token::op_intDevision;
		operationMap.insert(p);
	}
	bool is_operator() {
		if (operationMap.find(field) != operationMap.end()) {
			return true;
		}
		else return false;
	}
	bool is_assigning() {
		if (field == "=") {
			return true;
		}
		return false;
	}
	double get_value() {

		if (is_operator()) {
			return NULL;
		}
		if (std::isdigit(field[0])) {
			//field is a const digit
			std::string::size_type sz;
			return std::stod(field, &sz);
		}
		else {
			return variableMap[field];
		}

	}
	double executeOperation(double a, double b) {
		if (!is_operator())
			return NULL;

		return operationMap[field](a, b);
	}
	double op_assign(std::string var, double a) {
		variableMap[var] = a;
		return a;
	}

private:
	static double op_plus(double a, double b) {
		return a + b;
	}
	static double op_minus(double a, double b) {
		return a - b;
	}
	static double op_multiply(double a, double b) {
		return a * b;
	}
	static double op_devision(double a, double b) {
		return a / b;
	}
	static double op_intDevision(double a, double b) {
		return (int)a / (int)b;
	}
	static double op_mod(double a, double b) {
		return (int)a % (int)b;
	}
	static double op_elevate(double a, double b) {
		return pow(a, b);
	}

};
