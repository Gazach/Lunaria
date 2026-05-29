#ifndef BUILTIN_H
#define BUILTIN_H

#include "../common/value.h"
#include "environment.h"
#include <stdlib.h>

Value *builtin_print(Value **args, int arg_count);

bool is_builtin(const char *name);

Value *call_builtin(const char *name, Value **args, int arg_count);

#endif // BUILTIN_H