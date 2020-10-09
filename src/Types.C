#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "Absyn.H"
#include "Helper.H"

using namespace std;



/********************   AnalysisValue    ********************/

Type *AnalysisValue::getType() const {
	return type->clone();
}

bool AnalysisValue::isValue() const {
	return true;
}

bool AnalysisValue::isIntConst() const {
	return false;
}

bool AnalysisValue::isBoolConst() const {
	return false;
}

/********************   AnalysisIntConst    ********************/

Type *AnalysisIntConst::getType() const {
	return new Int();
}

bool AnalysisIntConst::isValue() const {
	return false;
}

bool AnalysisIntConst::isIntConst() const {
	return true;
}

bool AnalysisIntConst::isBoolConst() const {
	return false;
}

/********************   AnalysisBoolConst    ********************/

Type *AnalysisBoolConst::getType() const {
	return new Bool();
}

bool AnalysisBoolConst::isValue() const {
	return false;
}

bool AnalysisBoolConst::isIntConst() const {
	return false;
}

bool AnalysisBoolConst::isBoolConst() const {
	return true;
}

/********************   Pointer    ********************/

string Pointer::toLLVM() const {
	return "%t" + to_string(address);
}

string Pointer::typeString() const {
	return type->toLLVM() + "*";
}

Type *Pointer::getType() const {
	return type->clone();
}

bool Pointer::isPointer() const {
	return true;
}

bool Pointer::isValue() const {
	return false;
}

bool Pointer::isIntConst() const {
	return false;
}

bool Pointer::isBoolConst() const {
	return false;
}

string Pointer::toString() const {
	string address_ = to_string(address);
	return "Pointer(" + type->toString() + "*, " + address_ + ")";
}

/********************   Value    ********************/

string Value::toLLVM() const {
	return "%t" + to_string(address);
}

string Value::typeString() const {
	return type->toLLVM();
}

Type *Value::getType() const {
	return type->clone();
}

bool Value::isPointer() const {
	return false;
}

bool Value::isValue() const {
	return true;
}

bool Value::isIntConst() const {
	return false;
}

bool Value::isBoolConst() const {
	return false;
}

string Value::toString() const {
	string address_ = to_string(address);
	return "Value(" + type->toString() + ", " + address_ + ")";
}

/********************   IntConst    ********************/

string IntConst::toLLVM() const {
	return to_string(value);
}

string IntConst::typeString() const {
	return "i32";
}

Type *IntConst::getType() const {
	return new Int();
}

bool IntConst::isPointer() const {
	return false;
}

bool IntConst::isValue() const {
	return false;
}

bool IntConst::isIntConst() const {
	return true;
}

bool IntConst::isBoolConst() const {
	return false;
}

string IntConst::toString() const {
	string value_ = to_string(value);
	return "IntConst(" + value_ + ")";
}

/********************   BoolConst    ********************/

string BoolConst::toLLVM() const {
	return to_string(value);
}

string BoolConst::typeString() const {
	return "i1";
}

Type *BoolConst::getType() const {
	return new Bool();
}

bool BoolConst::isPointer() const {
	return false;
}

bool BoolConst::isValue() const {
	return false;
}

bool BoolConst::isIntConst() const {
	return false;
}

bool BoolConst::isBoolConst() const {
	return true;
}

string BoolConst::toString() const {
	string value_ = to_string(value);
	return "BoolConst(" + value_ + ")";
}

/********************   Int    ********************/

const string Int::toString() const {
	return "Int";
}

const string Int::toLLVM() const {
	return "i32";
}

int Int::hashcode() const {
	return 0;
}

bool Int::isInt() const {
	return true;
}

bool Int::isStr() const {
	return false;
}

bool Int::isBool() const {
	return false;
}

bool Int::isVoid() const {
	return false;
}

bool Int::isFunction() const {
	return false;
}

/********************   Str    ********************/

const string Str::toString() const {
	return "Str";
}

const string Str::toLLVM() const {
	return "i8*";
}
int Str::hashcode() const {
	return 1;
}

bool Str::isInt() const {
	return false;
}

bool Str::isStr() const {
	return true;
}

bool Str::isBool() const {
	return false;
}

bool Str::isVoid() const {
	return false;
}

bool Str::isFunction() const {
	return false;
}

/********************   Bool    ********************/

const string Bool::toString() const {
	return "Bool";
}

const string Bool::toLLVM() const {
	return "i1";
}

int Bool::hashcode() const {
	return 2;
}

bool Bool::isInt() const {
	return false;
}

bool Bool::isStr() const {
	return false;
}

bool Bool::isBool() const {
	return true;
}

bool Bool::isVoid() const {
	return false;
}

bool Bool::isFunction() const {
	return false;
}

/********************   Void    ********************/

const string Void::toString() const {
	return "Void";
}

const string Void::toLLVM() const {
	return "void";
}

int Void::hashcode() const {
	return 3;
}

bool Void::isInt() const {
	return false;
}

bool Void::isStr() const {
	return false;
}

bool Void::isBool() const {
	return false;
}

bool Void::isVoid() const {
	return true;
}

bool Void::isFunction() const {
	return false;
}

/********************   Fun    ********************/

const string Fun::toString() const {
	string s = type_->toString() + "(";
	auto l = listtype_;
	while (l != 0) {
		s += l->type_->toString();
		l = l->listtype_;
		if (l != 0) {
			s += ", ";
		}
	}
	s += ")";
	return s;
}

const string Fun::toLLVM() const {
	throw "Fun::Unreachable";
}

int Fun::hashcode() const {
	// TODO recursive hash ???
	return 4;
}

bool Fun::isInt() const {
	return false;
}

bool Fun::isStr() const {
	return false;
}

bool Fun::isBool() const {
	return false;
}

bool Fun::isVoid() const {
	return false;
}

bool Fun::isFunction() const {
	return true;
}