#ifndef _INTERNAL_H
#define _INTERNAL_H

#define _DEFAULT_SOURCE
#include <unistd.h>

#include <llvm-c/Core.h>
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

// expressions
enum ast_ExpressionKind {
	AST_EXPRESSION_KIND_LITERAL_NUMBER,
	AST_EXPRESSION_KIND_IDENTIFIER,
};

struct ast_Expression {
	enum ast_ExpressionKind kind;
	struct ast_Type *type;
	union {
		ast_Identifier identifier;
		uint64_t number;
	} as;
};

// statements
enum ast_StatementKind {
	AST_STATEMENT_KIND_RETURN,
};

// @TODO add stuff to track type and data and stuff
struct ast_ReturnStatement {
	struct ast_Expression *expression;
};

struct ast_Statement {
	enum ast_StatementKind kind;
	union {
		struct ast_ReturnStatement ret;
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

enum ast_TypeKind {
	AST_TYPE_KIND_PRIMITIVE,
	// AST_TYPE_KIND_POINTER,
	// AST_TYPE_KIND_ARRAY,
};

enum ast_TypePrimitive {
	AST_TYPE_PRIMITIVE_VOID,
	AST_TYPE_PRIMITIVE_U8,
};

struct ast_Type {
	enum ast_TypeKind kind;
	union {
		enum ast_TypePrimitive primitive;
	} as;
};

struct ast_FunctionDeclaration {
	struct ast_Type *return_type;
	ast_Identifier name;
	struct ast_Block body;
};

struct ast_Declaration {
	enum ast_DeclarationKind kind;
	union {
		struct ast_FunctionDeclaration func;
	} as;
};

// program
struct ast_Program {
	struct ast_Declaration **items;
	size_t count;
	size_t capacity;
};

typedef size_t ast_TokenRef;
typedef struct {
	lexer_Tokens tokens;
	ast_TokenRef pos;
} ast_Parser;

typedef struct {
	ast_Parser p;
	alloc_Pool memory_pool;
	struct ast_Declaration *current_function;
} ast_Context;

Err ast_init(ast_Context *ctx);
Err ast_free(ast_Context ctx);

struct ast_Program ast_parse_tokens(ast_Context *ctx, lexer_Tokens tokens);

void ast_typecheck_program(ast_Context *ctx, struct ast_Program *program);

//
// IR
//
void ir_create(struct ast_Program program);

#endif //_INTERNAL_H
