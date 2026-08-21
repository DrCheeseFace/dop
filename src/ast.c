#include "./internal.h"

#define ast_alloc(ctx, size) alloc_alloc(&ctx->memory_pool, size)

Err
ast_init(ast_Context *ctx)
{
	memset(ctx, 0, sizeof(*ctx));
	return alloc_init(&ctx->memory_pool, getpagesize());
}

Err
ast_free(ast_Context ctx)
{
	return alloc_free(&ctx.memory_pool);
}

internal_function struct ast_Expression *
ast_expression_create(ast_Context *ctx, enum ast_ExpressionKind kind,
		      ast_Identifier value)
{
	struct ast_Expression *expression = ast_alloc(ctx, sizeof(*expression));
	if (!expression) {
		fprintf(stderr, "ast error: OOM at ast_expression_create\n");
		exit(1);
	}

	expression->kind = kind;
	strcpy((char *)expression->value, (char *)value);

	return expression;
}

internal_function struct ast_Statement *
ast_return_statement_create(ast_Context *ctx, struct ast_Expression *expr)
{
	struct ast_Statement *statement = ast_alloc(ctx, sizeof(*statement));
	if (!statement) {
		fprintf(stderr,
			"ast error: OOM at ast_return_statement_create\n");
		exit(1);
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
ast_block_push_statement(ast_Context *ctx, struct ast_Block *block,
			 struct ast_Statement *statement)
{
	if (block->count == block->capacity) {
		size_t new_capacity = block->capacity ? block->capacity * 2 : 4;
		struct ast_Statement **new_items =
			ast_alloc(ctx, sizeof(*new_items) * new_capacity);
		if (!new_items) {
			fprintf(stderr,
				"ast error: OOM at ast_program_push_statement\n");
			exit(1);
		}

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
ast_function_declaration_create(ast_Context *ctx, enum ast_Type return_type,
				ast_Identifier name, struct ast_Block body)
{
	struct ast_Declaration *new_func = ast_alloc(ctx, sizeof(*new_func));
	if (!new_func) {
		fprintf(stderr,
			"ast error: OOM at ast_function_declaration_create\n");
		exit(1);
	}

	new_func->kind = AST_DECLARATION_KIND_FUNCTION;
	new_func->as.func.return_type = return_type;
	strcpy((char *)new_func->as.func.name, (char *)name);
	new_func->as.func.body = body;

	return new_func;
}

struct ast_Program
ast_program_init(void)
{
	return (struct ast_Program){ 0 };
}

internal_function void
ast_program_push_declaration(ast_Context *ctx, struct ast_Program *program,
			     struct ast_Declaration *declaration)
{
	if (program->count == program->capacity) {
		size_t new_capacity =
			program->capacity ? program->capacity * 2 : 4;
		struct ast_Declaration **new_items =
			ast_alloc(ctx, sizeof(*new_items) * new_capacity);

		if (!new_items) {
			fprintf(stderr,
				"ast error: OOM at ast_program_push_declaration\n");
			exit(1);
		}

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

internal_function struct lexer_Token *
ast_parser_peek(ast_Context *ctx)
{
	return &ctx->p.tokens[ctx->p.pos];
}

internal_function struct lexer_Token *
ast_parser_peek_at(ast_Context *ctx, ast_TokenRef ref)
{
	return &ctx->p.tokens[ref];
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
ast_parser_expect(ast_Context *ctx, enum lexer_TokenTag type)
{
	if (ast_parser_peek(ctx)->type != type) {
		// @TODO error handling
		fprintf(stderr, "ast error: parse error at token %zu\n",
			ctx->p.pos);
		exit(1);
	}

	return ctx->p.pos++;
}

internal_function struct ast_Expression *
ast_parse_expression(ast_Context *ctx)
{
	if (ast_parser_peek(ctx)->type == LEXER_TOKEN_TAG_LITERAL_NUM) {
		ast_TokenRef literal =
			ast_parser_expect(ctx, LEXER_TOKEN_TAG_LITERAL_NUM);
		struct lexer_Token *literal_token =
			ast_parser_peek_at(ctx, literal);
		return ast_expression_create(ctx,
					     AST_EXPRESSION_KIND_LITERAL_NUMBER,
					     literal_token->value);
	} else if (ast_parser_peek(ctx)->type ==
		   LEXER_TOKEN_TAG_LITERAL_IDENTIFIER) {
		ast_TokenRef identifier = ast_parser_expect(
			ctx, LEXER_TOKEN_TAG_LITERAL_IDENTIFIER);
		struct lexer_Token *identifier_token =
			ast_parser_peek_at(ctx, identifier);
		return ast_expression_create(ctx,
					     AST_EXPRESSION_KIND_IDENTIFIER,
					     identifier_token->value);
	}

	fprintf(stderr,
		"ast error: parse error: expected expression at token %zu\n",
		ctx->p.pos);
	exit(1);
}

internal_function struct ast_Statement *
ast_parse_statement(ast_Context *ctx)
{
	if (ast_parser_peek(ctx)->type == LEXER_TOKEN_TAG_KEYWORD_RETURN) {
		ast_parser_expect(ctx, LEXER_TOKEN_TAG_KEYWORD_RETURN);
		struct ast_Expression *expression = ast_parse_expression(ctx);
		ast_parser_expect(ctx, LEXER_TOKEN_TAG_DELIM_ENDSTATEMENT);
		return ast_return_statement_create(ctx, expression);
	}

	fprintf(stderr,
		"ast error: parse error: expected statement at token %zu\n",
		ctx->p.pos);
	exit(1);
}

internal_function struct ast_Block
ast_parse_block(ast_Context *ctx)
{
	struct ast_Block block = ast_block_init();
	ast_parser_expect(ctx, LEXER_TOKEN_TAG_DELIM_OPENCURLY);

	while (ast_parser_peek(ctx)->type != LEXER_TOKEN_TAG_DELIM_CLOSECURLY &&
	       ast_parser_peek(ctx)->type != LEXER_TOKEN_TAG_EOF) {
		struct ast_Statement *statement = ast_parse_statement(ctx);
		ast_block_push_statement(ctx, &block, statement);
	}

	ast_parser_expect(ctx, LEXER_TOKEN_TAG_DELIM_CLOSECURLY);

	return block;
}

internal_function struct ast_Declaration *
ast_parse_function_declaration(ast_Context *ctx)
{
	// @TODO hard coded type shit
	ast_parser_expect(ctx, LEXER_TOKEN_TAG_TYPE_U8);

	ast_TokenRef function_identifier =
		ast_parser_expect(ctx, LEXER_TOKEN_TAG_LITERAL_IDENTIFIER);

	struct lexer_Token *function_identifier_token =
		ast_parser_peek_at(ctx, function_identifier);

	ast_parser_expect(ctx, LEXER_TOKEN_TAG_DELIM_OPENPAREN);
	ast_parser_expect(ctx, LEXER_TOKEN_TAG_DELIM_CLOSEPAREN);

	struct ast_Block function_body = ast_parse_block(ctx);

	return ast_function_declaration_create(ctx, AST_TYPE_U8,
					       function_identifier_token->value,
					       function_body);
}

internal_function struct ast_Declaration *
ast_parse_declaration(ast_Context *ctx)
{
	return ast_parse_function_declaration(ctx);
}

internal_function struct ast_Program
ast_parse_program(ast_Context *ctx)
{
	struct ast_Program program = ast_program_init();

	while (ast_parser_peek(ctx)->type != LEXER_TOKEN_TAG_EOF) {
		struct ast_Declaration *decl = ast_parse_declaration(ctx);
		ast_program_push_declaration(ctx, &program, decl);
	}

	return program;
}

struct ast_Program
ast_parse_tokens(ast_Context *ctx, lexer_Tokens tokens)
{
	ctx->p.tokens = tokens;
	ctx->p.pos = 0;

	return ast_parse_program(ctx);
}
