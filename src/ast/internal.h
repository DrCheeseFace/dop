#ifndef AST_INTERNAL_H
#define AST_INTERNAL_H

#include "../internal.h"

#define ast_alloc(ctx, size) alloc_alloc(&ctx->memory_pool, size)

void ast_type_primitive(struct ast_Type *type, enum ast_TypePrimitive p);

#endif //!AST_INTERNAL_H
