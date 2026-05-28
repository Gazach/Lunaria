#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "expr.h"
#include "stmt.h"

void print_expr(const Expr *expr, int indent);
void print_stmt(const Stmt *stmt, int indent);
void print_indent(int indent);

#endif
