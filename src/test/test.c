#include "../internal.h"
#include <mr_utils/mrt_test.h>

MRT_TEST_GROUP(sanity_check)
{
	MRT_ASSERT(1 == 1, "sanity check");

	lexer_Tokens tokens = lexer_create_tokens("test string");

	lexer_destroy_tokens(tokens);
	return;
}

int
main(void)
{
	MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
	MrtContext *ctx = mrt_ctx_create(logger);

	MRT_REGISTER_TEST_GROUP(ctx, sanity_check);

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
