#include "./internal.h"

global_variable alloc_Pool ast_memory_pool = { 0 };
#define ast_alloc(size) alloc_alloc(&ast_memory_pool, size)

Err
ast_init(void)
{
	return alloc_init(&ast_memory_pool, getpagesize());
}

Err
ast_free(void)
{
	return alloc_free(&ast_memory_pool);
}

struct ast_Expression *
ast_expression_create(enum ast_ExpressionKind kind, ast_TokenRef token)
{
	struct ast_Expression *expression = ast_alloc(sizeof(*expression));

	expression->kind = kind;
	expression->token = token;

	return expression;
}

struct ast_Statement *
ast_return_statement_create(ast_TokenRef return_keyword,
			    struct ast_Expression *expr)
{
	struct ast_Statement *statement = ast_alloc(sizeof(*statement));
	if (!statement) {
		return NULL;
	}

	statement->kind = AST_STATEMENT_RETURN;
	statement->as.ret.return_keyword = return_keyword;
	statement->as.ret.expression = expr;

	return statement;
}

struct ast_Block
ast_block_init(void)
{
	return (struct ast_Block){ 0 };
}

void
ast_block_push_statement(struct ast_Block *block,
			 struct ast_Statement *statement)
{
	if (block->count == block->capacity) {
		size_t new_capacity = block->capacity ? block->capacity * 2 : 4;
		struct ast_Statement *new_items =
			ast_alloc(sizeof(*new_items) * new_capacity);

		if (block->items) {
			memcpy(new_items, block->items,
			       block->count * sizeof(*new_items));
		}

		block->items = &new_items;
		block->capacity = new_capacity;
	}

	block->items[block->count] = statement;
	block->count++;
}

struct ast_Declaration *
ast_function_declaration_create(ast_TokenRef return_type, ast_TokenRef name,
				struct ast_Block body)
{
	struct ast_Declaration *new_func = ast_alloc(sizeof(*new_func));
	new_func->kind = AST_DECLARATION_KIND_FUNCTION;
	new_func->as.func.return_type = return_type;
	new_func->as.func.name = name;
	new_func->as.func.body = body;

	return new_func;
}

struct ast_Program
ast_program_init(void)
{
	return (struct ast_Program){ 0 };
}

void
ast_program_push_declaration(struct ast_Program *program,
			     struct ast_Declaration *declaration)
{
	if (program->count == program->capacity) {
		size_t new_capacity =
			program->capacity ? program->capacity * 2 : 4;
		struct ast_Declaration *new_items =
			ast_alloc(sizeof(*new_items) * new_capacity);

		if (program->items) {
			memcpy(new_items, program->items,
			       program->count * sizeof(*new_items));
		}

		program->items = &new_items;
		program->capacity = new_capacity;
	}

	program->items[program->count] = declaration;
	program->count++;
}
