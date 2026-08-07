#ifndef _INTERNAL_H
#define _INTERNAL_H

#include <mr_utils.h>
#include <stdio.h>
#include <stdlib.h>

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
};

struct lexer_Token {
	enum lexer_TokenTag type;
	unsigned char value[32];
};

typedef struct lexer_Token *lexer_Tokens;

lexer_Tokens lexer_create_tokens(const char *s);
void lexer_destroy_tokens(lexer_Tokens tokens);

#endif //_INTERNAL_H
