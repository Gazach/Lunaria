#include "astPrinter.h"
#include <stdio.h>

// Helper function to print indentation for better readability of the AST.

void print_indent(int indent) {
	for (int i = 0; i < indent; i++) printf("  ");
}

// Function to print an expression node in the AST with proper indentation.

void print_expr(const Expr *expr, int indent) {
	if (!expr) {
		print_indent(indent); printf("<null expr>\n");
		return;
	}
	switch (expr->type) {
		case EXPR_LITERAL:
			print_indent(indent); printf("Literal: %g\n", expr->literal.value);
			break;
		case EXPR_VARIABLE:
			print_indent(indent); printf("Variable: %s\n", expr->variable.name);
			break;
		case EXPR_BINARY:
			print_indent(indent); printf("Binary: %c\n", expr->binary.op);
			print_expr(expr->binary.left, indent + 1);
			print_expr(expr->binary.right, indent + 1);
			break;
		case EXPR_UNARY:
			print_indent(indent); printf("Unary: %c\n", expr->unary.op);
			print_expr(expr->unary.operand, indent + 1);
			break;
		case EXPR_CALL:
			print_indent(indent); printf("Call: %s\n", expr->call.name);
			for (int i = 0; i < expr->call.arg_count; i++) {
				print_expr(expr->call.args[i], indent + 1);
			}
			break;
		case EXPR_GROUPING:
			print_indent(indent); printf("Grouping\n");
			print_expr(expr->grouping.expression, indent + 1);
			break;
		case EXPR_LOGICAL:
			print_indent(indent); printf("Logical: %s\n", expr->logical.op == TOKEN_AND_AND ? "&&" : "||");
			print_expr(expr->logical.left, indent + 1);
			print_expr(expr->logical.right, indent + 1);
			break;
		default:
			print_indent(indent); printf("<unknown expr>\n");
			break;
	}
}

// Function to print a statement node in the AST with proper indentation.

void print_stmt(const Stmt *stmt, int indent) {
	if (!stmt) {
		print_indent(indent); printf("<null stmt>\n");
		return;
	}
	switch (stmt->type) {
		case STMT_EXPR:
			print_indent(indent); printf("ExprStmt\n");
			print_expr(stmt->expr_stmt.expr, indent + 1);
			break;
		case STMT_ASSIGN:
			print_indent(indent); printf("Assign: %s\n", stmt->assign_stmt.name);
			print_expr(stmt->assign_stmt.value, indent + 1);
			break;
		case STMT_IF:
			print_indent(indent); printf("If\n");
			print_indent(indent + 1); printf("Condition:\n");
			print_expr(stmt->if_stmt.condition, indent + 2);
			print_indent(indent + 1); printf("Then:\n");
			print_stmt(stmt->if_stmt.then_branch, indent + 2);
			if (stmt->if_stmt.else_branch) {
				print_indent(indent + 1); printf("Else:\n");
				print_stmt(stmt->if_stmt.else_branch, indent + 2);
			}
			for (int i = 0; i < stmt->if_stmt.elif_count; i++) {
				print_indent(indent + 1); printf("Elif Condition:\n");
				print_expr(stmt->if_stmt.elif_conditions[i], indent + 2);
				print_indent(indent + 1); printf("Elif Body:\n");
				print_stmt(stmt->if_stmt.elif_branches[i], indent + 2);
			}
			break;
		case STMT_ELSE:
			print_indent(indent); printf("Else\n");
			for (int i = 0; i < stmt->else_stmt.statement_count; i++)
				print_stmt(stmt->else_stmt.statements[i], indent + 1);
			break;
		case STMT_ELIF:
			print_indent(indent); printf("Elif\n");
			print_indent(indent + 1); printf("Condition:\n");
			print_expr(stmt->elif_stmt.condition, indent + 2);
			print_indent(indent + 1); printf("Body:\n");
			for (int i = 0; i < stmt->elif_stmt.body_count; i++)
				print_stmt(stmt->elif_stmt.body[i], indent + 2);
			break;
		case STMT_WHILE:
			print_indent(indent); printf("While\n");
			print_indent(indent + 1); printf("Condition:\n");
			print_expr(stmt->while_stmt.condition, indent + 2);
			print_indent(indent + 1); printf("Body:\n");
			print_stmt(stmt->while_stmt.body, indent + 2);
			break;
		case STMT_RETURN:
			print_indent(indent); printf("Return\n");
			print_expr(stmt->return_stmt.value, indent + 1);
			break;
		case STMT_TYPE_NOTATION:
			print_indent(indent); printf("TypeNotation: %s\n", stmt->type_notation_stmt.type_name);
			print_expr(stmt->type_notation_stmt.value, indent + 1);
			break;
		case STMT_FUNCTION_DECLARATION:
			print_indent(indent); printf("FunctionDecl: %s\n", stmt->function_declaration_stmt.name);
			print_indent(indent + 1); printf("Params:");
			for (int i = 0; i < stmt->function_declaration_stmt.param_count; i++)
				printf(" %s:%s", stmt->function_declaration_stmt.param_names[i], stmt->function_declaration_stmt.param_types[i]);
			printf("\n");
			print_indent(indent + 1); printf("ReturnType: %s\n", stmt->function_declaration_stmt.return_type);
			print_indent(indent + 1); printf("Body:\n");
			for (int i = 0; i < stmt->function_declaration_stmt.body_count; i++)
				print_stmt(stmt->function_declaration_stmt.body[i], indent + 2);
			break;
		case STMT_FOR:
			print_indent(indent); printf("For\n");
			print_indent(indent + 1); printf("Initializer:\n");
			print_stmt(stmt->for_stmt.initializer, indent + 2);
			print_indent(indent + 1); printf("Condition:\n");
			print_expr(stmt->for_stmt.condition, indent + 2);
			print_indent(indent + 1); printf("Increment:\n");
			print_expr(stmt->for_stmt.increment, indent + 2);
			print_indent(indent + 1); printf("Body:\n");
			print_stmt(stmt->for_stmt.body, indent + 2);
			break;
		case STMT_BLOCK:
			print_indent(indent); printf("Block\n");
			for (int i = 0; i < stmt->block_stmt.statement_count; i++)
				print_stmt(stmt->block_stmt.statements[i], indent + 1);
			break;
		case STMT_SWITCH:
			print_indent(indent); printf("Switch\n");
			print_indent(indent + 1); printf("Expression:\n");
			print_expr(stmt->switch_stmt.expression, indent + 2);
			print_indent(indent + 1); printf("Cases:\n");
			for (int i = 0; i < stmt->switch_stmt.case_count; i++)
				print_stmt(stmt->switch_stmt.cases[i], indent + 2);
			if (stmt->switch_stmt.default_case) {
				print_indent(indent + 1); printf("Default:\n");
				print_stmt(stmt->switch_stmt.default_case, indent + 2);
			}
			break;
		case STMT_CASE:
			print_indent(indent); printf("Case\n");
			print_expr(stmt->case_stmt.value, indent + 1);
			print_indent(indent + 1); printf("Body:\n");
			for (int i = 0; i < stmt->case_stmt.body_count; i++)
				print_stmt(stmt->case_stmt.body[i], indent + 2);
			break;
		case STMT_BREAK:
			print_indent(indent); printf("Break\n");
			break;
		case STMT_CONTINUE:
			print_indent(indent); printf("Continue\n");
			break;
		case STMT_DEFAULT:
			print_indent(indent); printf("Default\n");
			for (int i = 0; i < stmt->default_stmt.case_count; i++)
				print_stmt(stmt->default_stmt.cases[i], indent + 1);
			break;
		case STMT_IMPORT:
			print_indent(indent); printf("Import: %s\n", stmt->import_stmt.module_name);
			break;
		case STMT_CLASS_DECLARATION:
			print_indent(indent); printf("ClassDecl: %s\n", stmt->class_declaration_stmt.name);
			print_indent(indent + 1); printf("Body:\n");
			for (int i = 0; i < stmt->class_declaration_stmt.body_count; i++)
				print_stmt(stmt->class_declaration_stmt.body[i], indent + 2);
			break;
		default:
			print_indent(indent); printf("<unknown stmt>\n");
			break;
	}
}
