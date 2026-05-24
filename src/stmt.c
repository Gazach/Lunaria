#include "stmt.h"
#include "expr.h"
#include <stdlib.h>

// Create a new statement node with the given type and associated data.
struct Stmt *stmt_new(StmtType type) {
    struct Stmt *s = malloc(sizeof(struct Stmt));
    s->type = type;
    return s;
}