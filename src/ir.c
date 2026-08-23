#include "./internal.h"

internal_function LLVMTypeRef
ir_to_llvm_type(struct ast_Type ast_type, LLVMContextRef context)
{
	switch (ast_type.kind) {
	case AST_TYPE_KIND_PRIMITIVE: {
		switch (ast_type.as.primitive) {
		case AST_TYPE_PRIMITIVE_U8:
			return LLVMInt8TypeInContext(context);
		case AST_TYPE_PRIMITIVE_VOID:
			return LLVMVoidTypeInContext(context);
		default:
			__builtin_unreachable();
		}
	}
	default:
		__builtin_unreachable();
	}
}

internal_function LLVMValueRef
ir_build_expression(struct ast_Expression *expression, LLVMContextRef context)
{
	if (expression == NULL) {
		return NULL;
	}

	switch (expression->kind) {
	case AST_EXPRESSION_KIND_LITERAL_NUMBER: {
		LLVMTypeRef expression_type =
			ir_to_llvm_type(expression->type, context);
		return LLVMConstInt(expression_type, expression->as.number,
				    FALSE);
	}
	case AST_EXPRESSION_KIND_IDENTIFIER: {
		// @TODO lookup
		__builtin_unreachable();
	}
	default:
		__builtin_unreachable();
	}
}

internal_function void
ir_add_statement(struct ast_Statement *item, LLVMContextRef context,
		 LLVMBuilderRef builder)
{
	switch (item->kind) {
	case AST_STATEMENT_KIND_RETURN: {
		LLVMValueRef ret_val =
			ir_build_expression(item->as.ret.expression, context);

		LLVMBuildRet(builder, ret_val);

		break;
	}
	default:
		fprintf(stderr,
			"ir error: lol change this error mesages laterk\n");
		exit(1);
	}
}

internal_function void
ir_add_function_declaration(struct ast_FunctionDeclaration func,
			    LLVMContextRef context, LLVMModuleRef module,
			    LLVMBuilderRef builder)
{
	LLVMTypeRef return_type;
	switch (func.return_type.as.primitive) {
	case AST_TYPE_PRIMITIVE_U8: {
		return_type = LLVMInt8TypeInContext(context);
		break;
	}

	case AST_TYPE_PRIMITIVE_VOID: {
		return_type = LLVMVoidTypeInContext(context);
		break;
	}

	default:
		__builtin_unreachable();
	}

	LLVMTypeRef function_type =
		LLVMFunctionType(return_type, NULL, 0, FALSE);

	LLVMValueRef function =
		LLVMAddFunction(module, (char *)func.name, function_type);

	LLVMBasicBlockRef entry =
		LLVMAppendBasicBlockInContext(context, function, "");

	LLVMPositionBuilderAtEnd(builder, entry);

	for (size_t i = 0; i < func.body.count; i++) {
		ir_add_statement(func.body.items[i], context, builder);
	}
}

internal_function void
ir_add_declaration(struct ast_Declaration *declaration, LLVMContextRef context,
		   LLVMModuleRef module, LLVMBuilderRef builder)
{
	ir_add_function_declaration(declaration->as.func, context, module,
				    builder);
}

internal_function void
ir_create_program(struct ast_Program program, LLVMContextRef context,
		  LLVMModuleRef module, LLVMBuilderRef builder)
{
	for (size_t i = 0; i < program.count; i++) {
		ir_add_declaration(program.items[i], context, module, builder);
	}
}

void
ir_create(struct ast_Program program)
{
	LLVMContextRef context = LLVMContextCreate();

	// @TODO wtf does LLVMModuleCreateWithNameInContext do
	LLVMModuleRef module =
		LLVMModuleCreateWithNameInContext("root", context);

	LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);

	ir_create_program(program, context, module, builder);

	LLVMPrintModuleToFile(module, "root.ll", NULL);

	LLVMDisposeBuilder(builder);
	LLVMDisposeModule(module);
	LLVMContextDispose(context);
}
