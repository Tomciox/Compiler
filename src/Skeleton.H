#ifndef SKELETON_HEADER
#define SKELETON_HEADER
/* You might want to change the above name. */

#include "Absyn.H"

class Skeleton : public Visitor
{
 public:

  void visitProgram(Program* program); /* abstract class */
  void visitProg(Prog* prog);
  void visitTopDef(TopDef* topdef); /* abstract class */
  void visitFnDef(FnDef* fndef);
  void visitListTopDef(ListTopDef* listtopdef);  void visitArg(Arg* arg); /* abstract class */
  void visitAr(Ar* ar);
  void visitListArg(ListArg* listarg);  void visitBlock(Block* block); /* abstract class */
  void visitBlk(Blk* blk);
  void visitListStmt(ListStmt* liststmt);  void visitStmt(Stmt* stmt); /* abstract class */
  void visitEmpty(Empty* empty);
  void visitBStmt(BStmt* bstmt);
  void visitDecl(Decl* decl);
  void visitAss(Ass* ass);
  void visitIncr(Incr* incr);
  void visitDecr(Decr* decr);
  void visitRet(Ret* ret);
  void visitVRet(VRet* vret);
  void visitCond(Cond* cond);
  void visitCondElse(CondElse* condelse);
  void visitWhile(While* while);
  void visitSExp(SExp* sexp);
  void visitItem(Item* item); /* abstract class */
  void visitNoInit(NoInit* noinit);
  void visitInit(Init* init);
  void visitListItem(ListItem* listitem);  void visitType(Type* type); /* abstract class */
  void visitInt(Int* int);
  void visitStr(Str* str);
  void visitBool(Bool* bool);
  void visitVoid(Void* void);
  void visitListType(ListType* listtype);  void visitExpr(Expr* expr); /* abstract class */
  void visitEVar(EVar* evar);
  void visitELitInt(ELitInt* elitint);
  void visitELitTrue(ELitTrue* elittrue);
  void visitELitFalse(ELitFalse* elitfalse);
  void visitEApp(EApp* eapp);
  void visitEString(EString* estring);
  void visitNeg(Neg* neg);
  void visitNot(Not* not);
  void visitEMul(EMul* emul);
  void visitEAdd(EAdd* eadd);
  void visitERel(ERel* erel);
  void visitEAnd(EAnd* eand);
  void visitEOr(EOr* eor);
  void visitListExpr(ListExpr* listexpr);  void visitAddOp(AddOp* addop); /* abstract class */
  void visitPlus(Plus* plus);
  void visitMinus(Minus* minus);
  void visitMulOp(MulOp* mulop); /* abstract class */
  void visitTimes(Times* times);
  void visitDiv(Div* div);
  void visitMod(Mod* mod);
  void visitRelOp(RelOp* relop); /* abstract class */
  void visitLTH(LTH* lth);
  void visitLE(LE* le);
  void visitGTH(GTH* gth);
  void visitGE(GE* ge);
  void visitEQU(EQU* equ);
  void visitNE(NE* ne);


  void visitIdent(String s);
  void visitInteger(Integer i);
  void visitDouble(Double d);
  void visitChar(Char c);
  void visitString(String s);
};

#endif

