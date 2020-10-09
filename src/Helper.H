#ifndef HELPER_HEADER
#define HELPER_HEADER

#include <map>
#include <vector>
#include "Absyn.H"

const std::string TAB = "  ";

std::string strip_ext(std::string filename);
std::string strip_dir(std::string filename);
void redirect(FILE *src, FILE *dst);

// Helpers for semantic analysis.

void throwError(semanticAnalysisMemory *m, std::string s);
AnalysisResult *findVariableType(semanticAnalysisMemory *m, char *ident_);
Type *findFunctionType(semanticAnalysisMemory *m, char *ident_);

// Helpers for code generation.

void generateCode(generateMemory *m, std::string s);
void addStringToConstants(generateMemory *m, const char *s_);
Pointer *findVariable(generateMemory *m, char *ident_);
Type *findFunctionReturnType(generateMemory *m, char *ident_);

// Code generation for LLVM instructions.

Pointer *generateAlloca(generateMemory *m, Type* t);
Value *generateLoad(generateMemory *m, Pointer *r);
void generateStore(generateMemory *m, Result *res, Pointer *reg);
void generateLabel(generateMemory *m, int label);

// Code generation for statements.

void generateReturn(generateMemory *m, Result *r);
void generateVReturn(generateMemory *m);
void generateBranch(generateMemory *m, Result *res, int l1, int l2);
void generateJump(generateMemory *m, int l);

// Code generation for expressions.

Result *generateAdd(generateMemory *m, Result *r1, AddOp *addop_, Result *r2);
Result *generateMul(generateMemory *m, Result *r1, MulOp *mulop_, Result *r2);
Result *generateRel(generateMemory *m, Result *r1, RelOp *relop_, Result *r2);
Result *generatePhi(generateMemory *m, Result *r1, int l1, Result *r2, int l2);
// Result *generateAnd(generateMemory *m, Result *r1, Result *r2);
// Result *generateOr(generateMemory *m, Result *r1, Result *r2);
Result *generateBitcast(generateMemory *m, char *s_);

#endif

