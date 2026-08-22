#ifndef AST_INTERNAL_H
#define AST_INTERNAL_H

#include "../internal.h"

#define ast_alloc(ctx, size) alloc_alloc(&ctx->memory_pool, size)

struct ast_Type *ast_type_primitive(ast_Context *ctx, enum ast_TypePrimitive p);

#endif //!AST_INTERNAL_H
