#include "./internal.h"

internal_function struct ast_Type *
ast_typecheck_expression(ast_Context *ctx, struct ast_Expression *expr)
{
	switch (expr->kind) {
	case AST_EXPRESSION_KIND_LITERAL_NUMBER:
		expr->type = ast_type_primitive(ctx, AST_TYPE_PRIMITIVE_U8);
		break;

	case AST_EXPRESSION_KIND_IDENTIFIER:
		// @TODO look up identifier in a symbol table
		expr->type = ast_type_primitive(ctx, AST_TYPE_PRIMITIVE_U8);
		break;

	default: {
		__builtin_unreachable();
	}
	}
	return expr->type;
}

internal_function void
ast_typecheck_statement(ast_Context *ctx, struct ast_Statement *stmt)
{
	struct ast_Type *expected = ctx->current_function->as.func.return_type;

	switch (stmt->kind) {
	case AST_STATEMENT_KIND_RETURN: {
		struct ast_Type *got =
			ast_typecheck_expression(ctx, stmt->as.ret.expression);

		// primitive comparison for now
		if (got->kind != expected->kind ||
		    (got->kind == AST_TYPE_KIND_PRIMITIVE &&
		     got->as.primitive != expected->as.primitive)) {
			fprintf(stderr, "type error: return type mismatch\n");
			exit(1);
		}
		break;
	}
	default: {
		__builtin_unreachable();
	}
	}
}

internal_function void
ast_typecheck_function(ast_Context *ctx, struct ast_Declaration *decl)
{
	if (decl->kind != AST_DECLARATION_KIND_FUNCTION) {
		return;
	}

	ctx->current_function = decl;
	struct ast_FunctionDeclaration *func = &decl->as.func;

	for (size_t i = 0; i < func->body.count; i++) {
		ast_typecheck_statement(ctx, func->body.items[i]);
	}

	ctx->current_function = NULL;
}

void
ast_typecheck_program(ast_Context *ctx, struct ast_Program *program)
{
	for (size_t i = 0; i < program->count; i++) {
		ast_typecheck_function(ctx, program->items[i]);
	}
}
