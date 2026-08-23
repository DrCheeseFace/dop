#include "../internal.h"
#include <mr_utils/mrt_test.h>

MRT_TEST_GROUP(sanity_check)
{
	MRT_ASSERT(1 == 1, "sanity check");
}

MRT_TEST_GROUP(test_lexer_basic_tokenize)
{
	const char *code_string = "u8 main() {return 3;}";
	lexer_Tokens tokens = lexer_create_tokens(code_string);

	/* expected: [
        /   typeu8,
	/   identifier(main),
        /   openparen,
        /   closeparen,
        /   opencurly,
        /   keywordreturn,
        /   literalnum(3),
        /   semicolon,
        /   closecurley
        /   EOF
        /  ]
        */

	MRT_ASSERT(tokens != NULL, "tokens array should not be NULL");

	MRT_ASSERT(tokens[0].kind == LEXER_TOKEN_TAG_TYPE_U8,
		   "token 0 = U8 kind");

	MRT_ASSERT(tokens[1].kind == LEXER_TOKEN_TAG_LITERAL_IDENTIFIER,
		   "token 1 = an identifier");
	MRT_ASSERT(strcmp((char *)tokens[1].value, "main") == 0,
		   "token 1 value = 'main'");

	MRT_ASSERT(tokens[2].kind == LEXER_TOKEN_TAG_DELIM_OPENPAREN,
		   "token 2 = open parenthesis");

	MRT_ASSERT(tokens[3].kind == LEXER_TOKEN_TAG_DELIM_CLOSEPAREN,
		   "token 3 = close parenthesis");

	MRT_ASSERT(tokens[4].kind == LEXER_TOKEN_TAG_DELIM_OPENCURLY,
		   "token 4 = open curly brace");

	MRT_ASSERT(tokens[5].kind == LEXER_TOKEN_TAG_KEYWORD_RETURN,
		   "token 5 = return keyword");

	MRT_ASSERT(tokens[6].kind == LEXER_TOKEN_TAG_LITERAL_NUM,
		   "token 6 = number literal");
	MRT_ASSERT(strcmp((char *)tokens[6].value, "3") == 0,
		   "token 6 value = '3'");

	MRT_ASSERT(tokens[7].kind == LEXER_TOKEN_TAG_DELIM_ENDSTATEMENT,
		   "token 7 = end statement");

	MRT_ASSERT(tokens[8].kind == LEXER_TOKEN_TAG_DELIM_CLOSECURLY,
		   "token 8 = close curly brace");

	MRT_ASSERT(tokens[9].kind == LEXER_TOKEN_TAG_EOF,
		   "token 9 = EOF marker");

	lexer_destroy_tokens(tokens);
}

MRT_TEST_GROUP(test_ast_basic_parse)
{
	const char *code_string = "u8 main() {return 3;}";
	lexer_Tokens tokens = lexer_create_tokens(code_string);

	ast_Context ctx;
	MRT_ASSERT(ast_init(&ctx) == OK, "ast_init OK");

	struct ast_Program program = ast_parse_tokens(&ctx, tokens);
	/* expected
         * program[0]
         *   funcdecl
         *     return_type: token[0]  (type_u8)
         *     name:        token[1]  (identifier "main")
         *     body:
         *          block[0]
         *               stmt return
         *                 return_keyword: token[5]  (keyword_return)
         *                 expression:
         *                      expr literal_number
         *                        token: token[6]  (literal_num "3")
         */

	lexer_destroy_tokens(tokens);

	MRT_ASSERT(program.count == 1, "program declaration count = 1");
	MRT_ASSERT(program.capacity == 4, "program declaration cap = 4");

	MRT_ASSERT(program.items[0]->kind == AST_DECLARATION_KIND_FUNCTION,
		   "function declaration kind");

	MRT_ASSERT(program.items[0]->as.func.return_type.kind ==
			   AST_TYPE_KIND_PRIMITIVE,
		   "function declaration return type kind");
	MRT_ASSERT(program.items[0]->as.func.return_type.as.primitive ==
			   AST_TYPE_PRIMITIVE_U8,
		   "function declaration return type is u8");

	MRT_ASSERT(strcmp((const char *)program.items[0]->as.func.name,
			  "main") == 0,
		   "function declaration identifier name");

	MRT_ASSERT(program.items[0]->as.func.body.count == 1,
		   "function body count = 1");
	MRT_ASSERT(program.items[0]->as.func.body.capacity == 4,
		   "function body capacity = 4");
	MRT_ASSERT(program.items[0]->as.func.body.items[0]->kind ==
			   AST_STATEMENT_KIND_RETURN,
		   "statement kind");
	MRT_ASSERT(program.items[0]->as.func.body.items[0]
				   ->as.ret.expression->kind ==
			   AST_EXPRESSION_KIND_LITERAL_NUMBER,
		   "statement expression kind");
	MRT_ASSERT(program.items[0]->as.func.body.items[0]
				   ->as.ret.expression->as.number == 3,
		   "statement expression token value");

	ast_free(ctx);
}

MRT_TEST_GROUP(test_ast_multiple_statements)
{
	const char *code = "u8 foo() {return a; return 42;}";
	lexer_Tokens tokens = lexer_create_tokens(code);

	ast_Context ctx;
	MRT_ASSERT(ast_init(&ctx) == OK, "ast_init OK");

	struct ast_Program program = ast_parse_tokens(&ctx, tokens);

	lexer_destroy_tokens(tokens);

	MRT_ASSERT(program.count == 1, "one decl");
	MRT_ASSERT(program.items[0]->as.func.body.count == 2, "two stmts");

	// return identifier
	struct ast_Statement *s1 = program.items[0]->as.func.body.items[0];
	MRT_ASSERT(s1->kind == AST_STATEMENT_KIND_RETURN, "stmt 1 kind");
	MRT_ASSERT(s1->as.ret.expression->kind ==
			   AST_EXPRESSION_KIND_IDENTIFIER,
		   "expr 1 is identifier");
	MRT_ASSERT(strcmp((char *)s1->as.ret.expression->as.identifier, "a") ==
			   0,
		   "identifier value");

	// return number
	struct ast_Statement *s2 = program.items[0]->as.func.body.items[1];
	MRT_ASSERT(s2->as.ret.expression->kind ==
			   AST_EXPRESSION_KIND_LITERAL_NUMBER,
		   "expr 2 is number");

	ast_free(ctx);
}

MRT_TEST_GROUP(test_ast_multiple_functions)
{
	const char *code_string = "u8 f() {return 1;} void g() {return;}";
	lexer_Tokens tokens = lexer_create_tokens(code_string);

	ast_Context ctx;
	MRT_ASSERT(ast_init(&ctx) == OK, "ast_init OK");

	struct ast_Program program = ast_parse_tokens(&ctx, tokens);

	lexer_destroy_tokens(tokens);

	MRT_ASSERT(program.count == 2, "program declaration count = 2");

	// f() test
	MRT_ASSERT(program.items[0]->kind == AST_DECLARATION_KIND_FUNCTION,
		   "first declaration is a function");

	MRT_ASSERT(program.items[0]->as.func.return_type.kind ==
			   AST_TYPE_KIND_PRIMITIVE,
		   "first function return type kind is primitive");

	MRT_ASSERT(program.items[0]->as.func.return_type.as.primitive ==
			   AST_TYPE_PRIMITIVE_U8,
		   "first function return type is U8");

	MRT_ASSERT(strcmp((const char *)program.items[0]->as.func.name, "f") ==
			   0,
		   "first function name is 'f'");

	MRT_ASSERT(program.items[0]->as.func.body.count == 1,
		   "first function body count = 1");
	MRT_ASSERT(program.items[0]->as.func.body.items[0]->kind ==
			   AST_STATEMENT_KIND_RETURN,
		   "first function statement is return");
	MRT_ASSERT(program.items[0]->as.func.body.items[0]
				   ->as.ret.expression->kind ==
			   AST_EXPRESSION_KIND_LITERAL_NUMBER,
		   "first function return expression is literal number");
	MRT_ASSERT(program.items[0]->as.func.body.items[0]
				   ->as.ret.expression->as.number == 1,
		   "first function returns literal '1'");

	// g() test
	MRT_ASSERT(program.items[1]->kind == AST_DECLARATION_KIND_FUNCTION,
		   "second declaration is a function");

	MRT_ASSERT(program.items[1]->as.func.return_type.kind ==
			   AST_TYPE_KIND_PRIMITIVE,
		   "second function return type kind is primitive");

	MRT_ASSERT(program.items[1]->as.func.return_type.as.primitive ==
			   AST_TYPE_PRIMITIVE_VOID,
		   "second function return type is U8");

	MRT_ASSERT(strcmp((const char *)program.items[1]->as.func.name, "g") ==
			   0,
		   "second function name is 'g'");

	MRT_ASSERT(program.items[1]->as.func.body.count == 1,
		   "second function body count = 1");
	MRT_ASSERT(program.items[1]->as.func.body.items[0]->kind ==
			   AST_STATEMENT_KIND_RETURN,
		   "second function statement is return");
	MRT_ASSERT(program.items[1]->as.func.body.items[0]->as.ret.expression ==
			   NULL,
		   "second function return expression is null");

	ast_free(ctx);
}

MRT_TEST_GROUP(test_compile_return_only)
{
	const char *code_string = "u8 main(){return 1;} void bruh(){return;}";
	lexer_Tokens tokens = lexer_create_tokens(code_string);

	ast_Context ctx;
	MRT_ASSERT(ast_init(&ctx) == OK, "ast_init OK");

	struct ast_Program ast = ast_parse_tokens(&ctx, tokens);

	lexer_destroy_tokens(tokens);

	ir_create(ast);

	ast_free(ctx);
}

int
main(void)
{
	MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
	MrtContext *ctx = mrt_ctx_create(logger);

	MRT_REGISTER_TEST_GROUP(ctx, sanity_check);
	MRT_REGISTER_TEST_GROUP(ctx, test_lexer_basic_tokenize);
	MRT_REGISTER_TEST_GROUP(ctx, test_ast_basic_parse);
	MRT_REGISTER_TEST_GROUP(ctx, test_ast_multiple_statements);
	MRT_REGISTER_TEST_GROUP(ctx, test_ast_multiple_functions);

	MRT_REGISTER_TEST_GROUP(ctx, test_compile_return_only);

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
