#include "./internal.h"

// @TODO add checks for expected type and size of literal
internal_function void
ast_typecheck_expression(struct ast_Expression *expr)
{
	switch (expr->kind) {
	case AST_EXPRESSION_KIND_LITERAL_NUMBER:
		ast_type_primitive(&expr->type, AST_TYPE_PRIMITIVE_U8);
		break;

	case AST_EXPRESSION_KIND_IDENTIFIER:
		// @TODO look up identifier in a symbol table
		ast_type_primitive(&expr->type, AST_TYPE_PRIMITIVE_U8);
		break;

	default: {
		__builtin_unreachable();
	}
	}
}

internal_function void
ast_typecheck_statement(ast_Context *ctx, struct ast_Statement *stmt)
{
	struct ast_Type expected = ctx->current_function->as.func.return_type;

	switch (stmt->kind) {
	case AST_STATEMENT_KIND_RETURN: {
		Bool is_void =
			(expected.kind == AST_TYPE_KIND_PRIMITIVE &&
			 expected.as.primitive == AST_TYPE_PRIMITIVE_VOID);

		if (is_void) {
			if (stmt->as.ret.expression != NULL) {
				fprintf(stderr,
					"type error: void function cannot return a value\n");
				exit(1);
			}
			break;
		}

		// non-void must have a value
		if (stmt->as.ret.expression == NULL) {
			fprintf(stderr,
				"type error: missing return value in non-void function\n");
			exit(1);
		}

		ast_typecheck_expression(stmt->as.ret.expression);
		struct ast_Type got = stmt->as.ret.expression->type;

		if (got.kind != expected.kind ||
		    (got.kind == AST_TYPE_KIND_PRIMITIVE &&
		     got.as.primitive != expected.as.primitive)) {
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
		if (program->items[i]->kind == AST_DECLARATION_KIND_FUNCTION) {
			ast_typecheck_function(ctx, program->items[i]);
		}
	}
}
