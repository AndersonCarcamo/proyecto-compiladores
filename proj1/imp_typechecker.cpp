#include "imp_typechecker.hh"

int ImpTypeChecker::getMemLocals(){
  return mem_locals;
}

ImpTypeChecker::ImpTypeChecker() {

}

void ImpTypeChecker::typecheck(Program* p) {
  env.clear();
  p->accept(this);
  return;
}

void ImpTypeChecker::visit(Program* p) {
  p->body->accept(this);
  return;
}

void ImpTypeChecker::visit(Body* b) {
  env.add_level();
  b->var_decs->accept(this);
  b->slist->accept(this);
  env.remove_level();  
  return;
}

void ImpTypeChecker::visit(VarDecList* decs) {
  list<VarDec*>::iterator it;
  for (it = decs->vdlist.begin(); it != decs->vdlist.end(); ++it) {
    (*it)->accept(this);
  }  
  return;
}

void ImpTypeChecker::visit(VarDec* vd) {
  ImpType tt = ImpValue::get_basic_type(vd->type);
  list<string>::iterator it;

  if (tt == NOTYPE) { cout << "Tipo invalido: " << vd->type << endl; exit(0);}
  
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it) {
    env.add_var(*it, tt);
    mem_locals++;
  }   
  return;
}


void ImpTypeChecker::visit(StatementList* s) {
  list<Stm*>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
  }
  return;
}

void ImpTypeChecker::visit(AssignStatement* s) {
  ImpType type = s->rhs->accept(this);
  if (!env.check(s->id)) {
    cout << "Variable " << s->id << " undefined" << endl;
    exit(0);
  }
  ImpType lhs = env.lookup(s->id);

  if (lhs != type) {
    cout << "Type Error en Assign: Tipos de variable " << s->id;
    cout << " y RHS no coinciden" << endl;
    exit(0);
  }
  env.update(s->id, type);
  return;
}

void ImpTypeChecker::visit(PrintStatement* s) {
  s->e->accept(this);
  return;
}

void ImpTypeChecker::visit(IfStatement* s) {
  ImpType tipo = s->cond->accept(this);
  if(tipo!=TBOOL){cout<< "Se esperaba un tipo booleano en la sentencia if"; exit(0);}
  s->cond->accept(this);
  s->tbody->accept(this);
  if (s->fbody != NULL)
    s->fbody->accept(this);
  return;
}

void ImpTypeChecker::visit(WhileStatement* s) {
  ImpType tcond = s->cond->accept(this);
  if(tcond!=TBOOL){cout<< "Se esperaba un tipo booleano en la sentencia while"; exit(0);}
  s->body->accept(this);
 return;
}

void ImpTypeChecker::visit(DoWhileStatement* s) {
  ImpType tcond = s->cond->accept(this);
  if(tcond!=TBOOL){cout<< "Se esperaba un tipo booleano en la sentencia do while"; exit(0);}
  s->body->accept(this);
 return;
}

ImpType ImpTypeChecker::visit(BinaryExp* e) {
  ImpType t1 = e->left->accept(this);
  ImpType t2 = e->right->accept(this);

  if (t1 != t2) {
    cout << "La operacion " << e->binopToString(e->op) << " tiene argumentos incorrectos" << endl;
    exit(0);
  }

  switch(e->op) {
  case PLUS: 
  case MINUS: 
  case MULT:
  case DIV: 
  case EXP: 
    if(t1 == TBOOL){
      cout << "ERROR: La operacion " << e->binopToString(e->op) << " no sporta bools" << endl;
      exit(0);
    }
    return TINT; break;
  case AND:
  case OR:
    if(t1 == TINT){
      cout << "ERROR: La operacion " << e->binopToString(e->op) << " no sporta ints" << endl;
      exit(0);
    }
    return TBOOL; break;
  case LT: 
  case LTEQ:
    if(t1 == TBOOL){
      cout << "ERROR: La operacion " << e->binopToString(e->op) << " no sporta bools" << endl;
      exit(0);
    }
    return TBOOL; break;
  case EQ:
    return TBOOL; break;
  }
  return ImpType();
}

ImpType ImpTypeChecker::visit(NumberExp* e) {
  ImpType t = TINT;
  return t;
}

ImpType ImpTypeChecker::visit(BoolConst* e) {
  ImpType t = TBOOL;
  return t;
}

ImpType ImpTypeChecker::visit(IdExp* e) {
  ImpType t = env.lookup(e->id);
  return t;
}

ImpType ImpTypeChecker::visit(ParenthExp* ep) {
  return ep->e->accept(this);
}

ImpType ImpTypeChecker::visit(CondExp* e) {
  ImpType btype = e->cond->accept(this);
  if(btype!=TBOOL){cout << "Se esperaba bool en cexp", exit(0);}
  ImpType ttype = e->etrue->accept(this);
  ImpType ftype = e->efalse->accept(this);
  if(ttype!=ftype){cout << "Tipo distinto en cexp",exit(0);}
  return NOTYPE;
}

