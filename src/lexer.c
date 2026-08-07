#include "internal.h"

internal_function lexer_Tokens *
lexer_tokens_append_token(lexer_Tokens *tokens, size_t *tokens_length,
			  struct lexer_Token token)
{
	lexer_Tokens temp = *tokens;
	temp = realloc(temp, (*tokens_length + 1) * sizeof(*tokens));
	if (!temp) {
		return NULL;
	}

	*tokens = temp;
	(*tokens)[*tokens_length] = token;
	*tokens_length += 1;

	return tokens;
}

lexer_Tokens
lexer_create_tokens(const char *s)
{
	ignore s;
	lexer_Tokens tokens = calloc(1, sizeof(*tokens));
	if (!tokens) {
		return NULL;
	}

	size_t tokens_length = 0;

	/* char *head, *tail = s; */
	/* ignore head; */
	/* ignore tail; */
	lexer_tokens_append_token(
		&tokens, &tokens_length,
		(struct lexer_Token){ .type = LEXER_TOKEN_TAG_EOF });

	return tokens;
}

void
lexer_destroy_tokens(lexer_Tokens tokens)
{
	free(tokens);
}
