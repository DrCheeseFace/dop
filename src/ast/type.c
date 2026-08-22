#include "./internal.h"

struct ast_Type *
ast_type_primitive(ast_Context *ctx, enum ast_TypePrimitive p)
{
	struct ast_Type *t = ast_alloc(ctx, sizeof(*t));
	t->kind = AST_TYPE_KIND_PRIMITIVE;
	t->as.primitive = p;
	return t;
}
