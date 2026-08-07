#include "../internal.h"
#include <mr_utils/mrt_test.h>

MRT_TEST_GROUP(sanity_check)
{
	MRT_ASSERT(1 == 1, "sanity check");

	lexer_Tokens tokens = lexer_create_tokens("test string");

	lexer_destroy_tokens(tokens);
	return;
}

MRT_TEST_GROUP(test_lexer_basic_tokenize)
{
	const char *code_string = "U8 main() {return 3;}";

	lexer_Tokens tokens = lexer_create_tokens(code_string);

	/* expected: [
        /   typeu8,
	/   identifier(main),
        /   openparen,
        /   closeparen,
        /   opencurly,
        /   keywordreturn,
        /   literalnum(3),
        /   closecurley
        /  ]
        */

	MRT_ASSERT(tokens != NULL, "tokens array should not be NULL");

	MRT_ASSERT(tokens[0].type == LEXER_TOKEN_TAG_TYPE_U8,
		   "token 0 = U8 type");

	MRT_ASSERT(tokens[1].type == LEXER_TOKEN_TAG_LITERAL_IDENTIFIER,
		   "token 1 = an identifier");
	MRT_ASSERT(strcmp((char *)tokens[1].value, "main") == 0,
		   "token 1 value = 'main'");

	MRT_ASSERT(tokens[2].type == LEXER_TOKEN_TAG_DELIM_OPENPAREN,
		   "token 2 = open parenthesis");

	MRT_ASSERT(tokens[3].type == LEXER_TOKEN_TAG_DELIM_CLOSEPAREN,
		   "token 3 = close parenthesis");

	MRT_ASSERT(tokens[4].type == LEXER_TOKEN_TAG_DELIM_OPENCURLY,
		   "token 4 = open curly brace");

	MRT_ASSERT(tokens[5].type == LEXER_TOKEN_TAG_KEYWORD_RETURN,
		   "token 5 = return keyword");

	MRT_ASSERT(tokens[6].type == LEXER_TOKEN_TAG_LITERAL_NUM,
		   "token 6 = number literal");
	MRT_ASSERT(strcmp((char *)tokens[6].value, "3") == 0,
		   "token 6 value = '3'");

	MRT_ASSERT(tokens[7].type == LEXER_TOKEN_TAG_DELIM_ENDSTATEMENT,
		   "token 7 = end statement");

	MRT_ASSERT(tokens[8].type == LEXER_TOKEN_TAG_DELIM_CLOSECURLY,
		   "token 8 = close curly brace");

	MRT_ASSERT(tokens[9].type == LEXER_TOKEN_TAG_EOF,
		   "token 9 = EOF marker");

	lexer_destroy_tokens(tokens);
}

int
main(void)
{
	MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
	MrtContext *ctx = mrt_ctx_create(logger);

	MRT_REGISTER_TEST_GROUP(ctx, sanity_check);
	MRT_REGISTER_TEST_GROUP(ctx, test_lexer_basic_tokenize);

#ifdef DEBUG
	Err err = mrt_ctx_run(ctx, FALSE);
#else
	Err err = mrt_ctx_run(ctx, TRUE);
#endif

	mrt_ctx_destroy(ctx);
	mrl_destroy(logger);

#ifdef DEBUG
	ASSERT(mrd_log_dump_active_allocations() == 0);
#endif

	return err;
}
