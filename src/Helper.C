#include <map>
#include <vector>
#include "Absyn.H"
#include "Helper.H"

using namespace std;

string strip_ext(string filename) {
  return filename.substr(0, filename.rfind('.'));
}

string strip_dir(string filename) {
  return filename.substr(filename.rfind('/') + 1);
}

void redirect(FILE *src, FILE *dst) {
  char c;

  while ((c = fgetc(src)) != EOF) {
    fputc(c, dst);
  }
}

void throwError(semanticAnalysisMemory *m, string s) {
  fprintf(stderr, "ERROR\n");
  fprintf(stderr, "In function: \'%s\'.\n", m->function_name.c_str());
  fprintf(stderr, "%s.\n", s.c_str());
  throw "ERROR";
}

AnalysisResult *findVariableType(semanticAnalysisMemory *m, char *ident_) {
  string i(ident_);
  for (auto rit = m->scopes.rbegin(); rit != m->scopes.rend(); ++rit) {
    if (rit->find(i) != rit->end()) {
      return rit->find(i)->second;
    }
  }
  throwError(m, "Usage of undeclared variable identifier \'" + i + "\'");
}

Type *findFunctionType(semanticAnalysisMemory *m, char *ident_) {
  string i(ident_);
  if (m->fun_scope.find(i) != m->fun_scope.end()) {
    return m->fun_scope.find(i)->second;
  }
  throwError(m, "Usage of undeclared function identifier \'" + i + "\'");
}

void generateCode(generateMemory *m, string s) {
  fprintf(m->filename, "%s", s.c_str());
  m->generate_return = true;
}

void addStringToConstants(generateMemory *m, const char *s_) {
  auto &c = m->string_constants;
  string s(s_);

  if (c.find(s) == c.end()) {
    m->string_constants.insert({s, m->next_string++});
  }
}

Pointer *findVariable(generateMemory *m, char *ident_) {
  string i(ident_);
  for (auto rit = m->context.rbegin(); rit != m->context.rend(); ++rit) {
    if (rit->find(i) != rit->end()) {
      return rit->find(i)->second;
    }
  }
  // unreachable
  throw "findVariable::ERROR";
}

Type *findFunctionReturnType(generateMemory *m, char *ident_) {
  string i(ident_);
  return m->functions.find(i)->second;
}


Pointer *generateAlloca(generateMemory *m, Type* t) {
  int label = m->fresh_label++;

  generateCode(m, TAB + "%t" + to_string(label) + " = alloca " + t->toLLVM() + "\n");

  return new Pointer(t, label);
}

Value *generateLoad(generateMemory *m, Pointer *r) {
  int label = m->fresh_label++;

  generateCode(m, TAB + "%t" + to_string(label) + " = load " + r->type->toLLVM() + ", "
               + r->typeString() + " " + r->toLLVM() + "\n");

  return new Value(r->getType(), label);
}

void generateStore(generateMemory *m, Result *res, Pointer *reg) {
  generateCode(m, TAB + "store " + res->typeString() + " " + res->toLLVM() + ", " +
               reg->typeString() + " " + reg->toLLVM() + "\n");
}

void generateLabel(generateMemory *m, int label) {
  generateCode(m, "L" + to_string(label) + ":\n");
  m->last_generated_label = label;
}

void generateReturn(generateMemory *m, Result *r) {
  if (!r->isPointer()) {
    generateCode(m, TAB + "ret " + r->typeString() + " " + r->toLLVM() + "\n");
  }
  else {
    Pointer *a = static_cast<Pointer*>(r);
    Result *r = generateLoad(m, a);
    generateReturn(m, r);
  }
  m->generate_return = false;
}

void generateVReturn(generateMemory *m) {
  generateCode(m, TAB + "ret void\n");
  m->generate_return = false;
}

void generateBranch(generateMemory *m, Result *res, int l1, int l2) {
  generateCode(m, TAB + "br i1 " + res->toLLVM() + ", label \%L" + to_string(l1) +
               ", label \%L" + to_string(l2) + "\n");
}

void generateJump(generateMemory *m, int l) {
  generateCode(m, TAB + "br label \%L" + to_string(l) + "\n");
}

Result *generateAdd(generateMemory *m, Result *r1, AddOp *addop_, Result *r2) {
  if (r1->isValue() || r2->isValue()) {
    int label = m->fresh_label++;

    if (r1->getType()->isInt() || r1->getType()->isBool()) {
      generateCode(m, TAB + "%t" + to_string(label) + " = " +
                   addop_->toLLVM() + " " + r1->typeString() + " " +
                   r1->toLLVM() + ", " + r2->toLLVM() + "\n");
    }
    else if (r1->getType()->isStr()) {
      generateCode(m, TAB + "%t" + to_string(label) + " = call i8* @concat(i8* " +
                   r1->toLLVM() + ", i8* " + r2->toLLVM() + ")\n");
    }
    return new Value(r1->getType(), label);
  }
  auto casted1 = static_cast<IntConst*>(r1);
  auto casted2 = static_cast<IntConst*>(r2);
  return new IntConst(addop_->calculate(casted1->value, casted2->value));
}

Result *generateMul(generateMemory *m, Result *r1, MulOp *mulop_, Result *r2) {
  if (r1->isValue() || r2->isValue()) {
    int label = m->fresh_label++;

    generateCode(m, TAB + "%t" + to_string(label) + " = " +
                 mulop_->toLLVM() + " " + r1->typeString() + " " +
                 r1->toLLVM() + ", " + r2->toLLVM() + "\n");

    return new Value(r1->getType(), label);
  }
  auto casted1 = static_cast<IntConst*>(r1);
  auto casted2 = static_cast<IntConst*>(r2);
  return new IntConst(mulop_->calculate(casted1->value, casted2->value));
}

Result *generateRel(generateMemory *m, Result *r1, RelOp *relop_, Result *r2) {
  if (r1->getType()->isInt() || r1->getType()->isBool()) {
    if (r1->isValue() || r2->isValue()) {
      int label = m->fresh_label++;

      generateCode(m, TAB + "%t" + to_string(label) + " = icmp " +
                   relop_->toLLVM() + " " + r1->typeString() + " " +
                   r1->toLLVM() + ", " + r2->toLLVM() + "\n");

      return new Value(new Bool(), label);
    }
    else if (r1->getType()->isInt()) {
      auto c1 = static_cast<IntConst*>(r1);
      auto c2 = static_cast<IntConst*>(r2);
      return new BoolConst(relop_->calculate(c1->value, c2->value));
    }
    else if (r1->getType()->isBool()) {
      auto c1 = static_cast<BoolConst*>(r1);
      auto c2 = static_cast<BoolConst*>(r2);
      return new BoolConst(relop_->calculate((int)c1->value, (int)c2->value));
    }
  }
  else if (r1->getType()->isStr()) {
    int label = m->fresh_label++;

    generateCode(m, TAB + "%t" + to_string(label) + " = call i32 @strcmp(i8* " +
                 r1->toLLVM() + ", i8* " + r2->toLLVM() + ")\n");

    int label2 = m->fresh_label++;
    generateCode(m, TAB + "%t" + to_string(label2) +
                 " = icmp eq i32 0, %t" + to_string(label) + "\n");

    return new Value(new Bool(), label2);
  }
}

Result *generatePhi(generateMemory *m, Result *r1, int l1, Result *r2, int l2) {
  int label = m->fresh_label++;

  generateCode(m, TAB + "%t" + to_string(label) + " = phi i1 [ " +
               r1->toLLVM() + ", %L" + to_string(l1) + " ], [ " +
               r2->toLLVM() + ", %L" + to_string(l2) + " ]\n");

  return new Value(new Bool(), label);
}

// Result *generateAnd(generateMemory *m, Result *r1, Result *r2) {
//   int label = m->fresh_label++;

//   generateCode(m, TAB + "%t" + to_string(label) + " = and " +
//                r1->typeString() + " " + r1->toLLVM() + ", " + r2->toLLVM() + "\n");

//   return new Value(new Bool(), label);
// }

// Result *generateOr(generateMemory *m, Result *r1, Result *r2) {
//   int label = m->fresh_label++;

//   generateCode(m, TAB + "%t" + to_string(label) + " = or " +
//                r1->typeString() + " " + r1->toLLVM() + ", " + r2->toLLVM() + "\n");

//   return new Value(new Bool(), label);
// }

Result *generateBitcast(generateMemory *m, char *s_) {
  int label = m->fresh_label++;

  string s(s_);
  string idx = to_string(m->string_constants.find(s)->second);
  string len = to_string(s.size() + 1);

  generateCode(m, TAB + "%t" + to_string(label) + " = bitcast [" +
               len + " x i8]* @.str." + idx + " to i8*\n");

  return new Value(new Str(), label);
}