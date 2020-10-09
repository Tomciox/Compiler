#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "Absyn.H"
#include "Helper.H"

using namespace std;

/********************   Prog    ********************/

void Prog::semanticAnalysis(semanticAnalysisMemory *m) {

	m->fun_scope.insert({"printInt", new Fun(new Void(), new ListType(new Int()))});
	m->fun_scope.insert({"printString", new Fun(new Void(), new ListType(new Str()))});
	m->fun_scope.insert({"readInt", new Fun(new Int(), 0)});
	m->fun_scope.insert({"readString", new Fun(new Str(), 0)});
	m->fun_scope.insert({"error", new Fun(new Void(), 0)});

	Scope s;
	m->scopes.push_back(s);

	listtopdef_->semanticAnalysisAddFunctions(m);
	listtopdef_->semanticAnalysis(m);

	m->scopes.pop_back();
}

void Prog::generateLLVM(generateMemory *m) {
	string prolog_name = "../LLVM/prolog2.ll";
	FILE *prolog = fopen(prolog_name.c_str(), "r");
	redirect(prolog, m->filename);

	m->next_string = 10;
	listtopdef_->searchStringConstants(m);

	for (auto s : m->string_constants) {
		string idx = to_string(s.second);
		string len = to_string(s.first.size() + 1);
		generateCode(m, "@.str." + idx + " = private unnamed_addr constant [" + len +
		             " x i8] c\"" + s.first + "\\00\", align 1\n");
	}

	m->functions.insert({"printInt", new Void()});
	m->functions.insert({"printString", new Void()});
	m->functions.insert({"readInt", new Int()});
	m->functions.insert({"readString", new Str()});
	m->functions.insert({"error", new Void()});

	listtopdef_->generateLLVMAddFunctions(m);
	listtopdef_->generateLLVM(m);
}

/********************   FnDef    ********************/

void FnDef::semanticAnalysisAddFunctions(semanticAnalysisMemory *m) {
	string i(ident_);
	if (listarg_ != 0) {
		auto d = new Fun(type_->clone(), listarg_->cast());
		m->fun_scope.insert({i, d});
	}
	else {
		auto d = new Fun(type_->clone(), 0);
		m->fun_scope.insert({i, d});
	}
}

void FnDef::semanticAnalysis(semanticAnalysisMemory *m) {
	string name(ident_);
	m->function_name = name;
	m->ret = type_->clone();
	Scope s;

	auto l = listarg_;
	while (l != 0) {
		string i(l->arg_->getIdent());
		if (s.find(i) != s.end()) {
			throwError(m, "Redeclaration of argument \'" + i + "\'");
		}
		else {
			if (l->arg_->getType()->isVoid()) {
				throwError(m, "Argument cannot be of \'" + l->arg_->getType()->toString() + "\' type");
			}
			s.insert({i, new AnalysisValue(l->arg_->getType())});
		}
		l = l->listarg_;
	}

	m->scopes.push_back(s);
	m->branch_return = type_->isVoid();
	auto ls = block_->getListStmt();
	if (ls != 0) {
		ls->semanticAnalysis(m);
	}
	if (!m->branch_return) {
		throwError(m, "Not all possible branches end with return statement");
	}
	m->scopes.pop_back();
}


void FnDef::generateLLVMAddFunctions(generateMemory *m) {
	string i(ident_);
	// TODO clone ???
	m->functions.insert({i, type_->clone()});
}

void FnDef::searchStringConstants(generateMemory *m) {
	if (block_ != 0) {
		block_->searchStringConstants(m);
	}
}

void FnDef::generateLLVM(generateMemory *m) {
	m->fresh_label = 0;
	m->fresh_block = 0;
	m->generate_return = false;
	Context c;

	// TODO wydzielić do osobnej funkcji define ???
	string name(ident_);
	generateCode(m, "define " + type_->toLLVM() + " @" + name + "(");

	auto l = listarg_;
	while (l != 0) {
		// TODO niekoniecznie ?
		m->fresh_label++;
		generateCode(m, l->arg_->getType()->toLLVM());
		l = l->listarg_;

		if (l != 0) {
			generateCode(m, ", ");
		}
	}

	generateCode(m, ") {\n");

	generateLabel(m, m->fresh_block++);

	l = listarg_;
	int i = 0;
	while (l != 0) {
		auto arg = l->arg_;
		auto type = arg->getType();

		// alloc nowego rejestru odpowiedniego typu
		auto reg = generateAlloca(m, type);

		// dodanie pod odpowiednim identyfikatorem rejestru z zaalokowanym miejscem
		string ident(arg->getIdent());
		c.insert({ident, reg});

		// zapisanie pod zaalokowanym miejscem przekazanej wartości argumentu
		string i_ = to_string(i++);
		generateCode(m, TAB + "store " + type->toLLVM() + " %" + i_ + ", " +
		             reg->typeString() + " " + reg->toLLVM() + "\n");

		l = l->listarg_;
	}
	m->context.push_back(c);

	auto ls = block_->getListStmt();
	if (ls != 0) {
		ls->generateLLVM(m);
	}

	if (m->generate_return) {
		if (type_->isVoid()) {
			generateVReturn(m);
		}
		else if (type_->isInt()) {
			generateReturn(m, new IntConst(0));
		}
		else if (type_->isBool()) {
			generateReturn(m, new BoolConst(false));
		}
		else if (type_->isStr()) {
			int label = m->fresh_label++;

			generateCode(m, TAB + "%t" + to_string(label) +
			             " = bitcast [1 x i8]* @.str.5" + " to i8*\n");

			generateCode(m, TAB + "ret " + type_->toLLVM() + " %t" + to_string(label) + "\n");
		}
	}

	m->context.pop_back();

	generateCode(m, "}\n\n");
}

/********************   ListTopDef    ********************/

void ListTopDef::semanticAnalysisAddFunctions(semanticAnalysisMemory *m) {
	topdef_->semanticAnalysisAddFunctions(m);
	if (listtopdef_ != 0) {
		listtopdef_->semanticAnalysisAddFunctions(m);
	}
}

void ListTopDef::semanticAnalysis(semanticAnalysisMemory *m) {
	topdef_->semanticAnalysis(m);
	if (listtopdef_ != 0) {
		listtopdef_->semanticAnalysis(m);
	}
}

void ListTopDef::generateLLVMAddFunctions(generateMemory *m) {
	topdef_->generateLLVMAddFunctions(m);
	if (listtopdef_ != 0) {
		listtopdef_->generateLLVMAddFunctions(m);
	}
}

void ListTopDef::generateLLVM(generateMemory *m) {
	topdef_->generateLLVM(m);
	if (listtopdef_ != 0) {
		listtopdef_->generateLLVM(m);
	}
}

void ListTopDef::searchStringConstants(generateMemory *m) {
	topdef_->searchStringConstants(m);
	if (listtopdef_ != 0) {
		listtopdef_->searchStringConstants(m);
	}
}

/********************   Ar    ********************/

Type *Ar::getType() const {
	return type_->clone();
}

char *Ar::getIdent() const {
	return ident_;
}

/********************   ListArg    ********************/

ListType* ListArg::cast() {
	if (listarg_ == 0) {
		return new ListType(arg_->getType());
	}
	return new ListType(arg_->getType(), listarg_->cast());

}

/********************   Blk    ********************/

void Blk::semanticAnalysis(semanticAnalysisMemory *m) {
	Scope s;
	m->scopes.push_back(s);
	if (liststmt_ != 0) {
		liststmt_->semanticAnalysis(m);
	}
	m->scopes.pop_back();
}

void Blk::searchStringConstants(generateMemory *m) {
	if (liststmt_ != 0) {
		liststmt_->searchStringConstants(m);
	}
}

void Blk::generateLLVM(generateMemory *m) {
	Context c;
	m->context.push_back(c);
	if (liststmt_ != 0) {
		liststmt_->generateLLVM(m);
	}
	m->context.pop_back();
}

ListStmt *Blk::getListStmt() {
	return liststmt_;
}

/********************   ListStmt    ********************/

void ListStmt::semanticAnalysis(semanticAnalysisMemory *m) {
	auto l = this;
	while (l != 0) {
		l->stmt_->semanticAnalysis(m);
		l = l->liststmt_;
	}
}

void ListStmt::searchStringConstants(generateMemory *m) {
	auto l = this;
	while (l != 0) {
		l->stmt_->searchStringConstants(m);
		l = l->liststmt_;
	}
}

void ListStmt::generateLLVM(generateMemory *m) {
	auto l = this;
	while (l != 0) {
		l->stmt_->generateLLVM(m);
		l = l->liststmt_;
	}
}

/********************   BStmt    ********************/

void BStmt::semanticAnalysis(semanticAnalysisMemory *m) {
	block_->semanticAnalysis(m);
}

void BStmt::searchStringConstants(generateMemory *m) {
	block_->searchStringConstants(m);
}

void BStmt::generateLLVM(generateMemory *m) {
	block_->generateLLVM(m);
}

/********************   Decl    ********************/

void Decl::semanticAnalysis(semanticAnalysisMemory *m) {
	auto l = listitem_;
	while (l != 0) {
		if (type_->isVoid()) {
			throwError(m, "Cannot declare variables of \'" + type_->toString() + "\' type");
		}

		m->assign = type_->clone();
		l->item_->semanticAnalysis(m);
		l = l->listitem_;
	}
}

void Decl::searchStringConstants(generateMemory *m) {
	auto l = listitem_;
	while (l != 0) {
		m->assign = type_->clone();
		l->item_->searchStringConstants(m);
		l = l->listitem_;
	}
}

void Decl::generateLLVM(generateMemory *m) {
	auto l = listitem_;
	while (l != 0) {
		m->assign = type_->clone();
		l->item_->generateLLVM(m);
		l = l->listitem_;
	}
}

/********************   Ass    ********************/

void Ass::semanticAnalysis(semanticAnalysisMemory *m) {
	string i(ident_);
	for (auto rit = m->scopes.rbegin(); rit != m->scopes.rend(); ++rit) {
		if (rit->find(i) != rit->end()) {
			auto lhs = rit->find(i)->second;
			auto rhs = expr_->semanticAnalysis(m);
			if (*lhs->getType() != *rhs->getType()) {
				throwError(m, "Assignment of variable \'" + i + "\',\n" +
				           "invalid conversion from \'" + rhs->getType()->toString() +
				           "\' to \'" + lhs->getType()->toString() + "\'");
			}
			return;
		}
	}
	throwError(m, "Assignment of undeclared variable \'" + i + "\'");
}

void Ass::searchStringConstants(generateMemory *m) {
	expr_->searchStringConstants(m);
}

void Ass::generateLLVM(generateMemory *m) {
	string i(ident_);
	for (auto rit = m->context.rbegin(); rit != m->context.rend(); ++rit) {
		if (rit->find(i) != rit->end()) {
			Result *res = expr_->generateLLVM(m);
			Pointer *reg = rit->find(i)->second;
			generateStore(m, res, reg);
			return;
		}
	}
}

/********************   Incr    ********************/

void Incr::semanticAnalysis(semanticAnalysisMemory *m) {
	auto t = findVariableType(m, ident_);
	if (!t->getType()->isInt()) {
		throwError(m, "Increment argument is of \'" + t->getType()->toString() + "\' type");
	}
}

void Incr::generateLLVM(generateMemory *m) {
	string i(ident_);
	for (auto rit = m->context.rbegin(); rit != m->context.rend(); ++rit) {
		if (rit->find(i) != rit->end()) {
			Pointer *reg = rit->find(i)->second;
			Value *value = generateLoad(m, reg);
			Result *incremented = generateAdd(m, value, new Plus(), new IntConst(1));
			generateStore(m, incremented, reg);
			return;
		}
	}
}

/********************   Decr    ********************/

void Decr::semanticAnalysis(semanticAnalysisMemory *m) {
	auto t = findVariableType(m, ident_);
	if (!t->getType()->isInt()) {
		throwError(m, "Decrement argument is of \'" + t->getType()->toString() + "\' type");
	}
}

void Decr::generateLLVM(generateMemory *m) {
	string i(ident_);
	for (auto rit = m->context.rbegin(); rit != m->context.rend(); ++rit) {
		if (rit->find(i) != rit->end()) {
			Pointer *reg = rit->find(i)->second;
			Value *value = generateLoad(m, reg);
			Result *incremented = generateAdd(m, value, new Plus(), new IntConst(-1));
			generateStore(m, incremented, reg);
			return;
		}
	}
}

/********************   Ret    ********************/

void Ret::semanticAnalysis(semanticAnalysisMemory *m) {
	auto r = expr_->semanticAnalysis(m);
	if (*r->getType() != *m->ret) {
		throwError(m, "Return statement is not of \'" + m->ret->toString() + "\' type");
	}
	m->branch_return = true;
}

void Ret::searchStringConstants(generateMemory *m) {
	expr_->searchStringConstants(m);
}

void Ret::generateLLVM(generateMemory *m) {
	Result *res = expr_->generateLLVM(m);
	generateReturn(m, res);
}

/********************   VRet    ********************/

void VRet::semanticAnalysis(semanticAnalysisMemory *m) {
	if (!m->ret->isVoid()) {
		throwError(m, "Return statement is not of \'" + m->ret->toString() + "\' type");
	}
	m->branch_return = true;
}

void VRet::generateLLVM(generateMemory *m) {
	generateVReturn(m);
}

/********************   Cond    ********************/

void Cond::semanticAnalysis(semanticAnalysisMemory *m) {
	auto r = expr_->semanticAnalysis(m);
	if (!r->getType()->isBool()) {
		throwError(m, "If condition is of \'" + r->getType()->toString() + "\' type");
	}

	if (r->isBoolConst()) {
		auto c = static_cast<AnalysisBoolConst*>(r);

		if (c->value) {
			bool branch_save = m->branch_return;
			stmt_->semanticAnalysis(m);
			m->branch_return = m->branch_return || branch_save;
		}
		else {
			bool branch_save = m->branch_return;
			stmt_->semanticAnalysis(m);
			m->branch_return = branch_save;
		}
	}
	else {
		bool branch_save = m->branch_return;
		stmt_->semanticAnalysis(m);
		m->branch_return = branch_save;
	}
}

void Cond::searchStringConstants(generateMemory *m) {
	expr_->searchStringConstants(m);
	stmt_->searchStringConstants(m);
}

void Cond::generateLLVM(generateMemory *m) {
	int inside = m->fresh_block++;
	int outside = m->fresh_block++;

	m->if_true = inside;
	m->if_false = outside;

	Result *res = expr_->generateLLVM(m);
	generateBranch(m, res, inside, outside);

	m->if_true = 0;
	m->if_false = 0;

	generateLabel(m, inside);
	stmt_->generateLLVM(m);
	generateJump(m, outside);
	generateLabel(m, outside);
}

/********************   CondElse    ********************/

void CondElse::searchStringConstants(generateMemory *m) {
	expr_->searchStringConstants(m);
	stmt_1->searchStringConstants(m);
	stmt_2->searchStringConstants(m);
}

void CondElse::semanticAnalysis(semanticAnalysisMemory *m) {
	auto r = expr_->semanticAnalysis(m);
	if (!r->getType()->isBool()) {
		throwError(m, "If condition is of \'" + r->getType()->toString() + "\' type");
	}

	if (r->isBoolConst()) {
		auto c = static_cast<AnalysisBoolConst*>(r);

		if (c->value) {
			bool branch_save = m->branch_return;
			m->branch_return = false;
			stmt_1->semanticAnalysis(m);
			bool branch_first = m->branch_return;
			m->branch_return = false;
			stmt_2->semanticAnalysis(m);
			m->branch_return = branch_save || branch_first;
		}
		else {
			bool branch_save = m->branch_return;
			m->branch_return = false;
			stmt_1->semanticAnalysis(m);
			m->branch_return = false;
			stmt_2->semanticAnalysis(m);
			bool branch_second = m->branch_return;
			m->branch_return = branch_save || branch_second;
		}
	}
	else {
		bool branch_save = m->branch_return;
		m->branch_return = false;
		stmt_1->semanticAnalysis(m);
		bool branch_first = m->branch_return;
		m->branch_return = false;
		stmt_2->semanticAnalysis(m);
		bool branch_second = m->branch_return;
		m->branch_return = branch_save || (branch_first && branch_second);
	}
}

void CondElse::generateLLVM(generateMemory *m) {


	int first = m->fresh_block++;
	int second = m->fresh_block++;
	int outside = m->fresh_block++;

	m->if_true = first;
	m->if_false = second;

	Result *r = expr_->generateLLVM(m);
	generateBranch(m, r, first, second);

	m->if_true = 0;
	m->if_false = 0;

	generateLabel(m, first);
	stmt_1->generateLLVM(m);
	generateJump(m, outside);

	generateLabel(m, second);
	stmt_2->generateLLVM(m);
	generateJump(m, outside);

	generateLabel(m, outside);
}

/********************   While    ********************/

void While::semanticAnalysis(semanticAnalysisMemory *m) {
	auto r = expr_->semanticAnalysis(m);
	if (!r->getType()->isBool()) {
		throwError(m, "While condition is of \'" + r->getType()->toString() + "\' type");
	}
	stmt_->semanticAnalysis(m);
}

void While::searchStringConstants(generateMemory *m) {
	expr_->searchStringConstants(m);
	stmt_->searchStringConstants(m);
}

void While::generateLLVM(generateMemory *m) {
	int condition = m->fresh_block++;
	int loop = m->fresh_block++;
	int outside = m->fresh_block++;

	generateJump(m, condition);


	generateLabel(m, condition);

	m->if_true = loop;
	m->if_false = outside;

	Result *res = expr_->generateLLVM(m);
	generateBranch(m, res, loop, outside);

	m->if_true = 0;
	m->if_false = 0;

	generateLabel(m, loop);
	stmt_->generateLLVM(m);
	generateJump(m, condition);

	generateLabel(m, outside);
}

/********************   SExp    ********************/

void SExp::semanticAnalysis(semanticAnalysisMemory *m) {
	expr_->semanticAnalysis(m);
}

void SExp::searchStringConstants(generateMemory *m) {
	expr_->searchStringConstants(m);
}

void SExp::generateLLVM(generateMemory *m) {
	expr_->generateLLVM(m);
}

/********************   NoInit    ********************/

char* NoInit::getIdent() {
	return ident_;
}

void NoInit::semanticAnalysis(semanticAnalysisMemory *m) {
	auto &s = m->scopes.back();
	string i(ident_);
	if (s.find(i) != s.end()) {
		throwError(m, "Redeclaration of variable \'" + i + "\'");
	}
	// TODO clone ??? ??? ???
	s.insert({i, new AnalysisValue(m->assign)});
}

void NoInit::searchStringConstants(generateMemory *m) {
	if (m->assign->isStr()) {
		string EMPTY = "";
		addStringToConstants(m, EMPTY.c_str());
	}
}

void NoInit::generateLLVM(generateMemory *m) {
	auto &c = m->context.back();
	string i(ident_);

	Pointer *reg = generateAlloca(m, m->assign);
	c.insert({i, reg});

	if (m->assign->isInt()) {
		generateStore(m, new IntConst(0), reg);
	}
}

/********************   Init    ********************/

char* Init::getIdent() {
	return ident_;
}

void Init::semanticAnalysis(semanticAnalysisMemory *m) {
	auto &s = m->scopes.back();
	string i(ident_);
	if (s.find(i) != s.end()) {
		throwError(m, "Redeclaration of variable \'" + i + "\'");
	}
	// TODO clone ???
	s.insert({i, new AnalysisValue(m->assign)});

	auto t = expr_->semanticAnalysis(m);
	if (*t->getType() != *m->assign) {
		throwError(m, "Initialization of variable \'" + i + "\',\n" +
		           "invalid conversion from \'" + t->getType()->toString() + "\' to \'" + m->assign->toString() + "\'");
	}
}

void Init::searchStringConstants(generateMemory *m) {
	expr_->searchStringConstants(m);
}

void Init::generateLLVM(generateMemory *m) {
	auto &c = m->context.back();
	string i(ident_);

	Result *res = expr_->generateLLVM(m);
	Pointer *reg = generateAlloca(m, m->assign);
	c.insert({i, reg});

	generateStore(m, res, reg);
}

/********************   EVar    ********************/

AnalysisResult *EVar::semanticAnalysis(semanticAnalysisMemory *m) {
	return findVariableType(m, ident_);
}

Result *EVar::generateLLVM(generateMemory *m) {
	Pointer *a = findVariable(m, ident_);
	auto r = generateLoad(m, a);
	return r;
}

/********************   ELitInt    ********************/

AnalysisResult *ELitInt::semanticAnalysis(semanticAnalysisMemory *m) {
	return new AnalysisIntConst(integer_);
}

Result *ELitInt::generateLLVM(generateMemory *m) {
	return new IntConst(integer_);
}

/********************   ELitTrue    ********************/

AnalysisResult *ELitTrue::semanticAnalysis(semanticAnalysisMemory *m) {
	return new AnalysisBoolConst(true);
}

Result *ELitTrue::generateLLVM(generateMemory *m) {
	return new BoolConst(true);
}

/********************   ELitFalse    ********************/

AnalysisResult *ELitFalse::semanticAnalysis(semanticAnalysisMemory *m) {
	return new AnalysisBoolConst(false);
}

Result *ELitFalse::generateLLVM(generateMemory *m) {
	return new BoolConst(false);
}

/********************   EApp    ********************/

AnalysisResult *EApp::semanticAnalysis(semanticAnalysisMemory *m) {
	string i(ident_);
	auto t = findFunctionType(m, ident_);
	if (!t->isFunction()) {
		throwError(m, "Identifier \'" + i + "\' cannot be used as a function");
	}

	auto casted = static_cast<Fun*>(t);
	auto required = casted->listtype_;
	auto passed = listexpr_;

	int idx = 1;
	while (required != 0 && passed != 0) {
		auto r = required->type_;
		auto p = passed->expr_->semanticAnalysis(m);
		if (*r != *p->getType()) {
			string s = to_string(idx++);
			throwError(m, "Invalid conversion from \'" + p->getType()->toString() + "\' to \'" + r->toString()
			           + "\' passing " + s + " argument of function \'" + i + "\'");
		}
		required = required->listtype_;
		passed = passed->listexpr_;
	}

	if (required != 0) {
		throwError(m, "Too few arguments to function \'" + i + "\'");
	}

	else if (passed != 0) {
		throwError(m, "Too many arguments to function \'" + i + "\'");
	}

	return new AnalysisValue(casted->type_);
}

void EApp::searchStringConstants(generateMemory *m) {
	auto l = listexpr_;
	while (l != 0) {
		l->expr_->searchStringConstants(m);
		l = l->listexpr_;
	}
}

Result *EApp::generateLLVM(generateMemory *m) {
	vector<Result*>results;

	auto l = listexpr_;
	while (l != 0) {
		auto result = l->expr_->generateLLVM(m);
		results.push_back(result);
		l = l->listexpr_;
	}

	int label = 0;
	Type *t = findFunctionReturnType(m, ident_);
	string ident(ident_);

	if (!t->isVoid()) {
		label = m->fresh_label++;
		string label_ = to_string(label);
		generateCode(m, TAB + "%t" + label_ + " = call " + t->toLLVM() + " @" + ident + "(");
	}
	else {
		generateCode(m, TAB + "call " + t->toLLVM() + " @" + ident + "(");
	}

	for (int i = 0; i < results.size(); ++i) {
		generateCode(m, results[i]->typeString() + " " + results[i]->toLLVM());
		if (i != results.size() - 1) {
			generateCode(m, ", ");
		}
	}

	generateCode(m, ")\n");

	return new Value(t->clone(), label);
}

/********************   EString    ********************/

AnalysisResult *EString::semanticAnalysis(semanticAnalysisMemory *m) {
	return new AnalysisValue(new Str());
}

void EString::searchStringConstants(generateMemory *m) {
	addStringToConstants(m, string_);
}

Result *EString::generateLLVM(generateMemory *m) {
	return generateBitcast(m, string_);
}

/********************   Neg    ********************/

AnalysisResult *Neg::semanticAnalysis(semanticAnalysisMemory *m) {
	auto e = expr_->semanticAnalysis(m);
	if (e->getType()->isInt()) {
		if (e->isValue()) {
			return new AnalysisValue(new Int());
		}
		else {
			auto casted = static_cast<AnalysisIntConst*>(e);
			return new AnalysisIntConst(-casted->value);
		}
	}
	throwError(m, "Neg expression is not of type Int");
}

Result *Neg::generateLLVM(generateMemory *m) {
	auto r = expr_->generateLLVM(m);
	return generateAdd(m, new IntConst(0), new Minus(), r);
}

/********************   Not    ********************/

AnalysisResult *Not::semanticAnalysis(semanticAnalysisMemory *m) {
	auto e = expr_->semanticAnalysis(m);
	if (e->getType()->isBool()) {
		if (e->isValue()) {
			return new AnalysisValue(new Bool());
		}
		else {
			auto casted = static_cast<AnalysisBoolConst*>(e);
			return new AnalysisBoolConst(!casted->value);
		}
	}
	throwError(m, "Not expression is not of type Bool");
}

Result *Not::generateLLVM(generateMemory *m) {
	auto r = expr_->generateLLVM(m);
	return generateAdd(m, new BoolConst(1), new Minus(), r);
}

/********************   EMul    ********************/

AnalysisResult *EMul::semanticAnalysis(semanticAnalysisMemory *m) {
	auto e1 = expr_1->semanticAnalysis(m);
	auto e2 = expr_2->semanticAnalysis(m);

	if (e1->getType()->isInt() && e2->getType()->isInt()) {
		if (e1->isValue() || e2->isValue()) {
			return new AnalysisValue(new Int());
		}
		else {
			auto casted1 = static_cast<AnalysisIntConst*>(e1);
			auto casted2 = static_cast<AnalysisIntConst*>(e2);
			return new AnalysisIntConst(casted1->value * casted2->value);
		}
	}
	throwError(m, "Mul expression is not of type Int");
}

Result *EMul::generateLLVM(generateMemory *m) {
	auto r1 = expr_1->generateLLVM(m);
	auto r2 = expr_2->generateLLVM(m);
	return generateMul(m, r1, mulop_, r2);
}

/********************   EAdd    ********************/

AnalysisResult *EAdd::semanticAnalysis(semanticAnalysisMemory *m) {
	auto e1 = expr_1->semanticAnalysis(m);
	auto e2 = expr_2->semanticAnalysis(m);

	if (e1->getType()->isInt() && e1->getType()->isInt()) {
		if (e1->isValue() || e2->isValue()) {
			return new AnalysisValue(new Int());
		}
		else {
			auto casted1 = static_cast<AnalysisIntConst*>(e1);
			auto casted2 = static_cast<AnalysisIntConst*>(e2);
			return new AnalysisIntConst(casted1->value + casted2->value);
		}
	}
	else if (e1->getType()->isStr() && e2->getType()->isStr()) {
		return new AnalysisValue(new Str());
	}
	throwError(m, "Add expression between types \'" +
	           e1->getType()->toString() + "\' and \'" +
	           e2->getType()->toString() + "\'");
}

void EAdd::searchStringConstants(generateMemory *m) {
	expr_1->searchStringConstants(m);
	expr_2->searchStringConstants(m);
}

Result *EAdd::generateLLVM(generateMemory *m) {
	auto r1 = expr_1->generateLLVM(m);
	auto r2 = expr_2->generateLLVM(m);
	return generateAdd(m, r1, addop_, r2);
}

/********************   ERel    ********************/

AnalysisResult *ERel::semanticAnalysis(semanticAnalysisMemory *m) {
	auto e1 = expr_1->semanticAnalysis(m);
	auto e2 = expr_2->semanticAnalysis(m);

	if (*e1->getType() != *e2->getType()) {
		throwError(m, "Rel expression arguments cannot be of different types \'" +
		           e1->getType()->toString() + "\' and \'" +
		           e2->getType()->toString() + "\'");
	}
	else if (e1->getType()->isInt() || e1->getType()->isBool()) {
		if (e1->isValue() || e2->isValue()) {
			return new AnalysisValue(new Bool());
		}
		else if (e1->getType()->isInt()) {
			auto c1 = static_cast<AnalysisIntConst*>(e1);
			auto c2 = static_cast<AnalysisIntConst*>(e2);
			return new AnalysisBoolConst(relop_->calculate(c1->value, c2->value));
		}
		else if (e1->getType()->isBool()) {
			auto c1 = static_cast<AnalysisBoolConst*>(e1);
			auto c2 = static_cast<AnalysisBoolConst*>(e2);
			return new AnalysisBoolConst(relop_->calculate((int)c1->value, (int)c2->value));
		}
	}
	else if (e1->getType()->isStr()) {
		if (relop_->isEquals()) {
			return new AnalysisValue(new Bool());
		}
		else {
			throwError(m, "Rel expression \'" + relop_->toString() +
			           "\' cannot be performed on arguments of type \'" +
			           e1->getType()->toString() + "\'");
		}
	}
}

void ERel::searchStringConstants(generateMemory *m) {
	expr_1->searchStringConstants(m);
	expr_2->searchStringConstants(m);
}

Result *ERel::generateLLVM(generateMemory *m) {
	auto e1 = expr_1->generateLLVM(m);
	auto e2 = expr_2->generateLLVM(m);
	return generateRel(m, e1, relop_, e2);
}

/********************   EAnd    ********************/

AnalysisResult *EAnd::semanticAnalysis(semanticAnalysisMemory *m) {
	auto e1 = expr_1->semanticAnalysis(m);
	auto e2 = expr_2->semanticAnalysis(m);

	if (e1->getType()->isBool() && e2->getType()->isBool()) {
		if (e1->isValue() || e2->isValue()) {
			return new AnalysisValue(new Bool);
		}
		else {
			auto c1 = static_cast<AnalysisBoolConst*>(e1);
			auto c2 = static_cast<AnalysisBoolConst*>(e2);
			return new AnalysisBoolConst(c1->value && c2->value);
		}
	}
	throwError(m, "And expression argument is not of \'Bool\' type");
}

Result *EAnd::generateLLVM(generateMemory *m) {
	if (m->if_true == 0) {
		int l1 = m->fresh_block++;
		int l2 = m->fresh_block++;
		int l3 = m->fresh_block++;

		generateJump(m, l1);

		generateLabel(m, l1);
		auto r1 = expr_1->generateLLVM(m);
		int from1 = m->last_generated_label;
		generateBranch(m, r1, l2, l3);

		generateLabel(m, l2);
		auto r2 = expr_2->generateLLVM(m);
		int from2 = m->last_generated_label;
		generateJump(m, l3);

		generateLabel(m, l3);
		return generatePhi(m, new BoolConst(false), from1, r2, from2);
	}
	else {
		auto r1 = expr_1->generateLLVM(m);

		int label = m->fresh_block++;
		generateBranch(m, r1, label, m->if_false);
		generateLabel(m, label);

		return expr_2->generateLLVM(m);
	}
}

/********************   EOr    ********************/

AnalysisResult *EOr::semanticAnalysis(semanticAnalysisMemory *m) {
	auto e1 = expr_1->semanticAnalysis(m);
	auto e2 = expr_2->semanticAnalysis(m);
	if (e1->getType()->isBool() && e2->getType()->isBool()) {
		if (e1->isValue() || e2->isValue()) {
			return new AnalysisValue(new Bool);
		}
		else {
			auto c1 = static_cast<AnalysisBoolConst*>(e1);
			auto c2 = static_cast<AnalysisBoolConst*>(e2);
			return new AnalysisBoolConst(c1->value || c2->value);
		}
	}
	throwError(m, "Or expression argument is not of type Bool");
}

Result *EOr::generateLLVM(generateMemory *m) {
	if (m->if_true == 0) {
		int l1 = m->fresh_block++;
		int l2 = m->fresh_block++;
		int l3 = m->fresh_block++;

		generateJump(m, l1);

		generateLabel(m, l1);
		auto r1 = expr_1->generateLLVM(m);
		int from1 = m->last_generated_label;
		generateBranch(m, r1, l3, l2);

		generateLabel(m, l2);
		auto r2 = expr_2->generateLLVM(m);
		int from2 = m->last_generated_label;
		generateJump(m, l3);

		generateLabel(m, l3);
		return generatePhi(m, new BoolConst(true), from1, r2, from2);
	}
	else {
		auto r1 = expr_1->generateLLVM(m);

		int label = m->fresh_block++;
		generateBranch(m, r1, m->if_true, label);
		generateLabel(m, label);

		return expr_2->generateLLVM(m);
	}
}

