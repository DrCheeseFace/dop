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
void lexer_destroy_tokens(void);

//
// CST
//

struct cst_Terminal {
	struct lexer_Token *token;
};

// expressions
enum cst_ExpressionKind {
	CST_EXPRESSION_LITERAL_NUMBER,
	CST_EXPRESSION_IDENTIFIER,
};

struct cst_Expression {
	enum cst_ExpressionKind kind;
};

struct cst_LiteralExpression {
	struct cst_Expression base;
	struct cst_Terminal *value;
};

struct cst_IdentifierExpression {
	struct cst_Expression base;
	struct cst_Terminal *name;
};

// statements
enum cst_StatementKind {
	CST_STATEMENT_RETURN,
};

struct cst_Statement {
	enum cst_StatementKind kind;
};

struct cst_ReturnStatement {
	struct cst_Statement base;
	struct cst_Terminal *return_keyword;
	struct cst_Expression *expression;
	struct cst_Terminal *semicolon;
};

// declarations
enum cst_DeclarationKind {
	CST_DECLARATION_FUNCTION,
};

struct cst_Declaration {
	enum cst_DeclarationKind kind;
};

struct cst_FunctionDeclaration {
	struct cst_Declaration base;
	struct cst_Terminal *return_type;
	struct cst_Terminal *identifier;
	struct cst_Terminal *open_paren;
	struct cst_Terminal *close_paren;
	struct cst_Block *body;
};

// block
struct cst_Block {
	struct cst_Terminal *open_brace;
	struct cst_Statement **statements;
	size_t statement_count;
	struct cst_Terminal *close_brace;
};

// program
struct cst_Program {
	struct cst_FunctionDeclaration **functions;
	size_t function_count;
};

#endif //_INTERNAL_H
