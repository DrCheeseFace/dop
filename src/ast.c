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

internal_function struct ast_Expression *
ast_expression_create(enum ast_ExpressionKind kind, ast_TokenRef token)
{
	struct ast_Expression *expression = ast_alloc(sizeof(*expression));

	expression->kind = kind;
	expression->token = token;

	return expression;
}

internal_function struct ast_Statement *
ast_return_statement_create(struct ast_Expression *expr)
{
	struct ast_Statement *statement = ast_alloc(sizeof(*statement));
	if (!statement) {
		return NULL;
	}

	statement->kind = AST_STATEMENT_KIND_RETURN;
	statement->as.ret.expression = expr;

	return statement;
}

internal_function struct ast_Block
ast_block_init(void)
{
	return (struct ast_Block){ 0 };
}

internal_function void
ast_block_push_statement(struct ast_Block *block,
			 struct ast_Statement *statement)
{
	if (block->count == block->capacity) {
		size_t new_capacity = block->capacity ? block->capacity * 2 : 4;
		struct ast_Statement **new_items =
			ast_alloc(sizeof(*new_items) * new_capacity);

		if (block->items) {
			memcpy(new_items, block->items,
			       block->count * sizeof(*new_items));
		}

		block->items = new_items;
		block->capacity = new_capacity;
	}

	block->items[block->count] = statement;
	block->count++;
}

internal_function struct ast_Declaration *
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

internal_function void
ast_program_push_declaration(struct ast_Program *program,
			     struct ast_Declaration *declaration)
{
	if (program->count == program->capacity) {
		size_t new_capacity =
			program->capacity ? program->capacity * 2 : 4;
		struct ast_Declaration **new_items =
			ast_alloc(sizeof(*new_items) * new_capacity);

		if (program->items) {
			memcpy(new_items, program->items,
			       program->count * sizeof(*new_items));
		}

		program->items = new_items;
		program->capacity = new_capacity;
	}

	program->items[program->count] = declaration;
	program->count++;
}

typedef struct {
	lexer_Tokens tokens;
	size_t pos;
} Parser;

internal_function struct lexer_Token *
ast_parser_peek(Parser *p)
{
	return &p->tokens[p->pos];
}

/* internal_function Bool */
/* ast_parser_match(Parser *p, enum lexer_TokenTag type) */
/* { */
/* 	if (ast_parser_peek(p)->type == type) { */
/* 		p->pos++; */
/* 		return TRUE; */
/* 	} */
/* 	return FALSE; */
/* } */

internal_function ast_TokenRef
ast_parser_expect(Parser *p, enum lexer_TokenTag type)
{
	if (ast_parser_peek(p)->type != type) {
		// @TODO error handling
		fprintf(stderr, "parse error at token %zu\n", p->pos);
		exit(1);
	}

	return p->pos++;
}

internal_function struct ast_Expression *
ast_parse_expression(Parser *p)
{
	if (ast_parser_peek(p)->type == LEXER_TOKEN_TAG_LITERAL_NUM) {
		ast_TokenRef literal =
			ast_parser_expect(p, LEXER_TOKEN_TAG_LITERAL_NUM);
		return ast_expression_create(AST_EXPRESSION_KIND_LITERAL_NUMBER,
					     literal);
	} else if (ast_parser_peek(p)->type ==
		   LEXER_TOKEN_TAG_LITERAL_IDENTIFIER) {
		ast_TokenRef identifier = ast_parser_expect(
			p, LEXER_TOKEN_TAG_LITERAL_IDENTIFIER);
		return ast_expression_create(AST_EXPRESSION_KIND_IDENTIFIER,
					     identifier);
	}

	fprintf(stderr, "parse error: expected expression at token %zu\n",
		p->pos);
	exit(1);
}

internal_function struct ast_Statement *
ast_parse_statement(Parser *p)
{
	if (ast_parser_peek(p)->type == LEXER_TOKEN_TAG_KEYWORD_RETURN) {
		ast_parser_expect(p, LEXER_TOKEN_TAG_KEYWORD_RETURN);
		struct ast_Expression *expression = ast_parse_expression(p);
		ast_parser_expect(p, LEXER_TOKEN_TAG_DELIM_ENDSTATEMENT);
		return ast_return_statement_create(expression);
	}

	fprintf(stderr, "parse error: expected statement at token %zu\n",
		p->pos);
	exit(1);
}

internal_function struct ast_Block
ast_parse_block(Parser *p)
{
	struct ast_Block block = ast_block_init();
	ast_parser_expect(p, LEXER_TOKEN_TAG_DELIM_OPENCURLY);

	while (ast_parser_peek(p)->type != LEXER_TOKEN_TAG_DELIM_CLOSECURLY &&
	       ast_parser_peek(p)->type != LEXER_TOKEN_TAG_EOF) {
		struct ast_Statement *statement = ast_parse_statement(p);
		ast_block_push_statement(&block, statement);
	}

	ast_parser_expect(p, LEXER_TOKEN_TAG_DELIM_CLOSECURLY);

	return block;
}

internal_function struct ast_Declaration *
ast_parse_function_declaration(Parser *p)
{
	ast_TokenRef function_return_type =
		ast_parser_expect(p, LEXER_TOKEN_TAG_TYPE_U8);
	ast_TokenRef function_identifier =
		ast_parser_expect(p, LEXER_TOKEN_TAG_LITERAL_IDENTIFIER);

	ast_parser_expect(p, LEXER_TOKEN_TAG_DELIM_OPENPAREN);
	ast_parser_expect(p, LEXER_TOKEN_TAG_DELIM_CLOSEPAREN);

	struct ast_Block function_body = ast_parse_block(p);

	return ast_function_declaration_create(
		function_return_type, function_identifier, function_body);
}

internal_function struct ast_Declaration *
ast_parse_declaration(Parser *p)
{
	return ast_parse_function_declaration(p);
}

internal_function struct ast_Program
ast_parse_program(Parser *p)
{
	struct ast_Program program = ast_program_init();

	while (ast_parser_peek(p)->type != LEXER_TOKEN_TAG_EOF) {
		struct ast_Declaration *decl = ast_parse_declaration(p);
		ast_program_push_declaration(&program, decl);
	}

	return program;
}

struct ast_Program
ast_parse_tokens(lexer_Tokens tokens)
{
	Parser p = { .tokens = tokens, .pos = 0 };

	return ast_parse_program(&p);
}
