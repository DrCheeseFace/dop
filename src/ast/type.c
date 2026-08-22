#include "./internal.h"

void
ast_type_primitive(struct ast_Type *type, enum ast_TypePrimitive p)
{
	type->kind = AST_TYPE_KIND_PRIMITIVE;
	type->as.primitive = p;
}
