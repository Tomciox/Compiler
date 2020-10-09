/*** BNFC-Generated Visitor Design Pattern Skeleton. ***/
/* This implements the common visitor design pattern.
   Note that this method uses Visitor-traversal of lists, so
   List->accept() does NOT traverse the list. This allows different
   algorithms to use context information differently. */

#include "Skeleton.H"

void Skeleton::visitProgram(Program* program) {} //abstract class

void Skeleton::visitProg(Prog* prog)
{
  /* Code For Prog Goes Here */

  if (prog->listtopdef_) {prog->listtopdef_->accept(this);}
}

void Skeleton::visitTopDef(TopDef* topdef) {} //abstract class

void Skeleton::visitFnDef(FnDef* fndef)
{
  /* Code For FnDef Goes Here */

  fndef->type_->accept(this);
  visitIdent(fndef->ident_);
  if (fndef->listarg_) {fndef->listarg_->accept(this);}
  fndef->block_->accept(this);
}

void Skeleton::visitListTopDef(ListTopDef* listtopdef)
{
  while(listtopdef!= 0)
  {
    /* Code For ListTopDef Goes Here */
    listtopdef->topdef_->accept(this);
    listtopdef = listtopdef->listtopdef_;
  }
}

void Skeleton::visitArg(Arg* arg) {} //abstract class

void Skeleton::visitAr(Ar* ar)
{
  /* Code For Ar Goes Here */

  ar->type_->accept(this);
  visitIdent(ar->ident_);
}

void Skeleton::visitListArg(ListArg* listarg)
{
  while(listarg!= 0)
  {
    /* Code For ListArg Goes Here */
    listarg->arg_->accept(this);
    listarg = listarg->listarg_;
  }
}

void Skeleton::visitBlock(Block* block) {} //abstract class

void Skeleton::visitBlk(Blk* blk)
{
  /* Code For Blk Goes Here */

  if (blk->liststmt_) {blk->liststmt_->accept(this);}
}

void Skeleton::visitListStmt(ListStmt* liststmt)
{
  while(liststmt!= 0)
  {
    /* Code For ListStmt Goes Here */
    liststmt->stmt_->accept(this);
    liststmt = liststmt->liststmt_;
  }
}

void Skeleton::visitStmt(Stmt* stmt) {} //abstract class

void Skeleton::visitEmpty(Empty* empty)
{
  /* Code For Empty Goes Here */

}

void Skeleton::visitBStmt(BStmt* bstmt)
{
  /* Code For BStmt Goes Here */

  bstmt->block_->accept(this);
}

void Skeleton::visitDecl(Decl* decl)
{
  /* Code For Decl Goes Here */

  decl->type_->accept(this);
  if (decl->listitem_) {decl->listitem_->accept(this);}
}

void Skeleton::visitAss(Ass* ass)
{
  /* Code For Ass Goes Here */

  visitIdent(ass->ident_);
  ass->expr_->accept(this);
}

void Skeleton::visitIncr(Incr* incr)
{
  /* Code For Incr Goes Here */

  visitIdent(incr->ident_);
}

void Skeleton::visitDecr(Decr* decr)
{
  /* Code For Decr Goes Here */

  visitIdent(decr->ident_);
}

void Skeleton::visitRet(Ret* ret)
{
  /* Code For Ret Goes Here */

  ret->expr_->accept(this);
}

void Skeleton::visitVRet(VRet* vret)
{
  /* Code For VRet Goes Here */

}

void Skeleton::visitCond(Cond* cond)
{
  /* Code For Cond Goes Here */

  cond->expr_->accept(this);
  cond->stmt_->accept(this);
}

void Skeleton::visitCondElse(CondElse* condelse)
{
  /* Code For CondElse Goes Here */

  condelse->expr_->accept(this);
  condelse->stmt_1->accept(this);
  condelse->stmt_2->accept(this);
}

void Skeleton::visitWhile(While* while)
{
  /* Code For While Goes Here */

  while->expr_->accept(this);
  while->stmt_->accept(this);
}

void Skeleton::visitSExp(SExp* sexp)
{
  /* Code For SExp Goes Here */

  sexp->expr_->accept(this);
}

void Skeleton::visitItem(Item* item) {} //abstract class

void Skeleton::visitNoInit(NoInit* noinit)
{
  /* Code For NoInit Goes Here */

  visitIdent(noinit->ident_);
}

void Skeleton::visitInit(Init* init)
{
  /* Code For Init Goes Here */

  visitIdent(init->ident_);
  init->expr_->accept(this);
}

void Skeleton::visitListItem(ListItem* listitem)
{
  while(listitem!= 0)
  {
    /* Code For ListItem Goes Here */
    listitem->item_->accept(this);
    listitem = listitem->listitem_;
  }
}

void Skeleton::visitType(Type* type) {} //abstract class

void Skeleton::visitInt(Int* int)
{
  /* Code For Int Goes Here */

}

void Skeleton::visitStr(Str* str)
{
  /* Code For Str Goes Here */

}

void Skeleton::visitBool(Bool* bool)
{
  /* Code For Bool Goes Here */

}

void Skeleton::visitVoid(Void* void)
{
  /* Code For Void Goes Here */

}

void Skeleton::visitListType(ListType* listtype)
{
  while(listtype!= 0)
  {
    /* Code For ListType Goes Here */
    listtype->type_->accept(this);
    listtype = listtype->listtype_;
  }
}

void Skeleton::visitExpr(Expr* expr) {} //abstract class

void Skeleton::visitEVar(EVar* evar)
{
  /* Code For EVar Goes Here */

  visitIdent(evar->ident_);
}

void Skeleton::visitELitInt(ELitInt* elitint)
{
  /* Code For ELitInt Goes Here */

  visitInteger(elitint->integer_);
}

void Skeleton::visitELitTrue(ELitTrue* elittrue)
{
  /* Code For ELitTrue Goes Here */

}

void Skeleton::visitELitFalse(ELitFalse* elitfalse)
{
  /* Code For ELitFalse Goes Here */

}

void Skeleton::visitEApp(EApp* eapp)
{
  /* Code For EApp Goes Here */

  visitIdent(eapp->ident_);
  if (eapp->listexpr_) {eapp->listexpr_->accept(this);}
}

void Skeleton::visitEString(EString* estring)
{
  /* Code For EString Goes Here */

  visitString(estring->string_);
}

void Skeleton::visitNeg(Neg* neg)
{
  /* Code For Neg Goes Here */

  neg->expr_->accept(this);
}

void Skeleton::visitNot(Not* not)
{
  /* Code For Not Goes Here */

  not->expr_->accept(this);
}

void Skeleton::visitEMul(EMul* emul)
{
  /* Code For EMul Goes Here */

  emul->expr_1->accept(this);
  emul->mulop_->accept(this);
  emul->expr_2->accept(this);
}

void Skeleton::visitEAdd(EAdd* eadd)
{
  /* Code For EAdd Goes Here */

  eadd->expr_1->accept(this);
  eadd->addop_->accept(this);
  eadd->expr_2->accept(this);
}

void Skeleton::visitERel(ERel* erel)
{
  /* Code For ERel Goes Here */

  erel->expr_1->accept(this);
  erel->relop_->accept(this);
  erel->expr_2->accept(this);
}

void Skeleton::visitEAnd(EAnd* eand)
{
  /* Code For EAnd Goes Here */

  eand->expr_1->accept(this);
  eand->expr_2->accept(this);
}

void Skeleton::visitEOr(EOr* eor)
{
  /* Code For EOr Goes Here */

  eor->expr_1->accept(this);
  eor->expr_2->accept(this);
}

void Skeleton::visitListExpr(ListExpr* listexpr)
{
  while(listexpr!= 0)
  {
    /* Code For ListExpr Goes Here */
    listexpr->expr_->accept(this);
    listexpr = listexpr->listexpr_;
  }
}

void Skeleton::visitAddOp(AddOp* addop) {} //abstract class

void Skeleton::visitPlus(Plus* plus)
{
  /* Code For Plus Goes Here */

}

void Skeleton::visitMinus(Minus* minus)
{
  /* Code For Minus Goes Here */

}

void Skeleton::visitMulOp(MulOp* mulop) {} //abstract class

void Skeleton::visitTimes(Times* times)
{
  /* Code For Times Goes Here */

}

void Skeleton::visitDiv(Div* div)
{
  /* Code For Div Goes Here */

}

void Skeleton::visitMod(Mod* mod)
{
  /* Code For Mod Goes Here */

}

void Skeleton::visitRelOp(RelOp* relop) {} //abstract class

void Skeleton::visitLTH(LTH* lth)
{
  /* Code For LTH Goes Here */

}

void Skeleton::visitLE(LE* le)
{
  /* Code For LE Goes Here */

}

void Skeleton::visitGTH(GTH* gth)
{
  /* Code For GTH Goes Here */

}

void Skeleton::visitGE(GE* ge)
{
  /* Code For GE Goes Here */

}

void Skeleton::visitEQU(EQU* equ)
{
  /* Code For EQU Goes Here */

}

void Skeleton::visitNE(NE* ne)
{
  /* Code For NE Goes Here */

}

void Skeleton::visitIdent(Ident i)
{
  /* Code for Ident Goes Here */
}
void Skeleton::visitInteger(Integer i)
{
  /* Code for Integers Goes Here */
}
void Skeleton::visitDouble(Double d)
{
  /* Code for Doubles Goes Here */
}
void Skeleton::visitChar(Char c)
{
  /* Code for Chars Goes Here */
}
void Skeleton::visitString(String s)
{
  /* Code for Strings Goes Here */
}

