#ifndef _INTERNAL_H
#define _INTERNAL_H

#define _DEFAULT_SOURCE
#include <unistd.h>

#include <mr_utils.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_IDENTIFIER_LENGTH 30

//
// ALLOCATOR
//
typedef struct {
	uint8_t *pool;
	size_t capacity;
	size_t offset;
} alloc_Pool;

Err alloc_init(alloc_Pool *pool, size_t capacity);
Err alloc_free(alloc_Pool *pool);

void *alloc_alloc(alloc_Pool *pool, size_t size);
Err alloc_head_expand(alloc_Pool *pool, size_t size);

void alloc_reset(alloc_Pool *pool);

typedef unsigned char ast_Identifier[MAX_IDENTIFIER_LENGTH + 1];

//
// LEXER
//

enum lexer_TokenTag {
	LEXER_TOKEN_TAG_LITERAL_NUM,
	LEXER_TOKEN_TAG_LITERAL_IDENTIFIER,

	LEXER_TOKEN_TAG_KEYWORD_RETURN,

	LEXER_TOKEN_TAG_DELIM_OPENPAREN,
	LEXER_TOKEN_TAG_DELIM_CLOSEPAREN,
	LEXER_TOKEN_TAG_DELIM_OPENCURLY,
	LEXER_TOKEN_TAG_DELIM_CLOSECURLY,
	LEXER_TOKEN_TAG_DELIM_ENDSTATEMENT,

	LEXER_TOKEN_TAG_TYPE_U8,

	LEXER_TOKEN_TAG_EOF,

	LEXER_TOKEN_TAG_COUNT,
};

struct lexer_Token {
	enum lexer_TokenTag type;
	ast_Identifier value;
};

typedef struct lexer_Token *lexer_Tokens;

lexer_Tokens lexer_create_tokens(const char *s);
void lexer_destroy_tokens(lexer_Tokens tokens);

//
// AST
//

typedef size_t ast_TokenRef;

// expressions
enum ast_ExpressionKind {
	AST_EXPRESSION_KIND_LITERAL_NUMBER,
	AST_EXPRESSION_KIND_IDENTIFIER,
};

struct ast_Expression {
	enum ast_ExpressionKind kind;
	ast_TokenRef token;
};

// statements
enum ast_StatementKind {
	AST_STATEMENT_RETURN,
};

struct ast_Statement {
	enum ast_StatementKind kind;
	union {
		struct {
			ast_TokenRef return_keyword;
			struct ast_Expression *expression;
		} ret;
	} as;
};

// blocks
struct ast_Block {
	struct ast_Statement **items;
	size_t count;
	size_t capacity;
};

// declarations
enum ast_DeclarationKind {
	AST_DECLARATION_KIND_FUNCTION,
};

struct ast_Declaration {
	enum ast_DeclarationKind kind;
	union {
		struct {
			ast_TokenRef return_type;
			ast_TokenRef name;
			struct ast_Block body;
		} func;
	} as;
};

// program
struct ast_Program {
	struct ast_Declaration **items;
	size_t count;
	size_t capacity;
};

Err ast_init(void);
Err ast_free(void);

struct ast_Expression *ast_expression_create(enum ast_ExpressionKind kind,
					     ast_TokenRef token);

struct ast_Statement *ast_return_statement_create(ast_TokenRef return_keyword,
						  struct ast_Expression *expr);

struct ast_Block ast_block_init(void);
void ast_block_push_statement(struct ast_Block *block,
			      struct ast_Statement *statement);

struct ast_Declaration *
ast_function_declaration_create(ast_TokenRef return_type, ast_TokenRef name,
				struct ast_Block body);

struct ast_Program ast_program_init(void);
void ast_program_push_declaration(struct ast_Program *program,
				  struct ast_Declaration *declaration);

#endif //_INTERNAL_H
