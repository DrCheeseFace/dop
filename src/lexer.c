#include "internal.h"

#define NO_DEFAULT_TOKEN_VALUE "-"
const char *lexer_token_tag_to_char[LEXER_TOKEN_TAG_COUNT] = {
	NO_DEFAULT_TOKEN_VALUE,
	NO_DEFAULT_TOKEN_VALUE,
	"return",
	"(",
	")",
	"{",
	"}",
	";",
	"U8",
	NO_DEFAULT_TOKEN_VALUE
};

global_variable alloc_Pool lexer_memory_pool = { 0 };

internal_function lexer_Tokens *
lexer_tokens_append_token(lexer_Tokens *tokens, size_t *tokens_length,
			  struct lexer_Token token)
{
	Err err = alloc_head_expand(&lexer_memory_pool,
				    ((*tokens_length) + 1) * sizeof(*tokens));
	if (err != OK) {
		return NULL;
	}

	(*tokens)[*tokens_length] = token;
	*tokens_length += 1;

	return tokens;
}

internal_function Bool
lexer_char_is_delimiter(const char c)
{
	for (enum lexer_TokenTag tag = LEXER_TOKEN_TAG_DELIM_OPENPAREN;
	     tag <= LEXER_TOKEN_TAG_DELIM_ENDSTATEMENT; tag++) {
		if (*lexer_token_tag_to_char[tag] == c) {
			return TRUE;
		}
	}

	return FALSE;
}

internal_function enum lexer_TokenTag
lexer_char_get_delimiter(const char c)
{
	for (enum lexer_TokenTag tag = LEXER_TOKEN_TAG_DELIM_OPENPAREN;
	     tag <= LEXER_TOKEN_TAG_DELIM_ENDSTATEMENT; tag++) {
		if (*lexer_token_tag_to_char[tag] == c) {
			return tag;
		}
	}

	__builtin_unreachable();
}

internal_function Bool
lexer_char_is_whitespace(const char c)
{
	if (c == ' ' || c == '\t' || c == '\n') {
		return TRUE;
	}
	return FALSE;
}

internal_function Bool
lexer_char_is_number(const char c)
{
	if (c >= '0' && c <= '9') {
		return TRUE;
	}

	return FALSE;
}

internal_function void
lexer_get_token_from_string_view(const char *code_string,
				 struct lexer_Token *new_token, size_t left,
				 size_t right)
{
	memset(new_token, 0, sizeof(*new_token));

	size_t string_view_length = right - left;
	if (string_view_length > MAX_IDENTIFIER_LENGTH) {
		__builtin_unreachable();
		return;
	}

	//@TODO only copy value if literal
	new_token->value[string_view_length] = '\0';
	memcpy(new_token->value, &code_string[left], right - left);

	// check delimiter token type
	if (lexer_char_is_delimiter(code_string[left])) {
		new_token->type = lexer_char_get_delimiter(code_string[left]);
		return;
	}

	// check keyword return token type
	if (strcmp((const char *)new_token->value,
		   lexer_token_tag_to_char[LEXER_TOKEN_TAG_KEYWORD_RETURN]) ==
	    0) {
		new_token->type = LEXER_TOKEN_TAG_KEYWORD_RETURN;
		return;
	}

	// check type return token type
	if (strcmp((const char *)new_token->value,
		   lexer_token_tag_to_char[LEXER_TOKEN_TAG_TYPE_U8]) == 0) {
		new_token->type = LEXER_TOKEN_TAG_TYPE_U8;
		return;
	}

	if (lexer_char_is_number(code_string[left])) {
		//@TODO verify
		new_token->type = LEXER_TOKEN_TAG_LITERAL_NUM;
	} else {
		//@TODO verify
		new_token->type = LEXER_TOKEN_TAG_LITERAL_IDENTIFIER;
	}
}

lexer_Tokens
lexer_create_tokens(const char *code_string)
{
	Err err = alloc_init(&lexer_memory_pool, getpagesize() * 4);
	if (err != OK) {
		return NULL;
	}
	lexer_Tokens tokens = alloc_alloc(&lexer_memory_pool, sizeof(*tokens));
	if (!tokens) {
		return NULL;
	}
	memset(lexer_memory_pool.pool, 0, lexer_memory_pool.capacity);

	size_t code_string_len = strlen(code_string);

	size_t tokens_length = 0;

	size_t left = 0, right = 0;

	while (left < code_string_len) {
		// seek right until delimiter or whitespace or left is delimiter
		while (!lexer_char_is_delimiter(code_string[right]) &&
		       !lexer_char_is_whitespace(code_string[right]) &&
		       !lexer_char_is_delimiter(code_string[left]) &&
		       right < code_string_len) {
			right++;
		}

		struct lexer_Token new_token = { 0 };
		lexer_get_token_from_string_view(code_string, &new_token, left,
						 right);

		tokens = *lexer_tokens_append_token(&tokens, &tokens_length,
						    new_token);

		// seeks past whitespace
		left = right;
		while (lexer_char_is_whitespace(code_string[left]) &&
		       left < code_string_len) {
			left++;
		}
		right = left + 1;
	}

	lexer_tokens_append_token(
		&tokens, &tokens_length,
		(struct lexer_Token){ .type = LEXER_TOKEN_TAG_EOF });

	return tokens;
}

void
lexer_destroy_tokens(void)
{
	alloc_free(&lexer_memory_pool);
}
