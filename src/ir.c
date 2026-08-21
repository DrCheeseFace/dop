#include "./internal.h"

internal_function void
ir_add_statement(struct ast_Statement *item)
{
	switch (item->kind) {
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
	// @TODO when we add more types, refactor this
	LLVMTypeRef return_type = LLVMInt8TypeInContext(context);

	LLVMTypeRef function_type =
		LLVMFunctionType(return_type, NULL, 0, FALSE);

	LLVMValueRef function =
		LLVMAddFunction(module, (char *)func.name, function_type);

	LLVMBasicBlockRef entry =
		LLVMAppendBasicBlockInContext(context, function, "");

	LLVMPositionBuilderAtEnd(builder, entry);

	for (size_t i = 0; i < func.body.count; i++) {
		//@TODO refactor ast_ReturnStatement to include information about type to avoid this TRAAAASH
		if (func.body.items[i]->kind == AST_STATEMENT_KIND_RETURN) {
			LLVMBuildRet(
				builder,
				LLVMConstInt(return_type,
					     atoll((char *)func.body.items[i]
							   ->as.ret.expression
							   ->value),
					     FALSE));
			continue;
		}

		ir_add_statement(func.body.items[i]);
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
