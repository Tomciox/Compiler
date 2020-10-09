#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "Absyn.H"
#include "Helper.H"

using namespace std;

/********************   Plus    ********************/

const string Plus::toString() const {
	return "+";
}

const string Plus::toLLVM() const {
	return "add";
}

int Plus::calculate(int r1, int r2) const {
	return r1 + r2;
}

/********************   Minus    ********************/

const string Minus::toString() const {
	return "-";
}

const string Minus::toLLVM() const {
	return "sub";
}

int Minus::calculate(int r1, int r2) const {
	return r1 - r2;
}

/********************   Times    ********************/

const string Times::toString() const {
	return "*";
}

const string Times::toLLVM() const {
	return "mul";
}

int Times::calculate(int r1, int r2) const {
	return r1 * r2;
}

/********************   Div    ********************/

const string Div::toString() const {
	return "/";
}

const string Div::toLLVM() const {
	return "sdiv";
}

int Div::calculate(int r1, int r2) const {
	return r1 / r2;
}

/********************   Mod    ********************/

const string Mod::toString() const {
	return "%";
}

const string Mod::toLLVM() const {
	return "srem";
}

int Mod::calculate(int r1, int r2) const {
	return r1 % r2;
}

/********************   LTH    ********************/

const string LTH::toString() const {
	return "<";
}

const string LTH::toLLVM() const {
	return "slt";
}

bool LTH::calculate(int r1, int r2) const {
	return r1 < r2;
}

bool LTH::isEquals() const {
	return false;
}

/********************   LE    ********************/

const string LE::toString() const {
	return "<=";
}

const string LE::toLLVM() const {
	return "sle";
}

bool LE::calculate(int r1, int r2) const {
	return r1 <= r2;
}

bool LE::isEquals() const {
	return false;
}

/********************   GTH    ********************/

const string GTH::toString() const {
	return ">";
}

const string GTH::toLLVM() const {
	return "sgt";
}

bool GTH::calculate(int r1, int r2) const {
	return r1 > r2;
}

bool GTH::isEquals() const {
	return false;
}

/********************   GE    ********************/

const string GE::toString() const {
	return ">=";
}

const string GE::toLLVM() const {
	return "sge";
}

bool GE::calculate(int r1, int r2) const {
	return r1 >= r2;
}

bool GE::isEquals() const {
	return false;
}

/********************   EQU    ********************/

const string EQU::toString() const {
	return "==";
}

const string EQU::toLLVM() const {
	return "eq";
}

bool EQU::calculate(int r1, int r2) const {
	return r1 == r2;
}

bool EQU::isEquals() const {
	return true;
}

/********************   NE    ********************/

const string NE::toString() const {
	return "!=";
}

const string NE::toLLVM() const {
	return "ne";
}

bool NE::calculate(int r1, int r2) const {
	return r1 != r2;
}

bool NE::isEquals() const {
	return false;
}