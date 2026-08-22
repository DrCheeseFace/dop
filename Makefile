SRC_LIB      := src/main.c

SRC_TEST_MAIN := src/test/test.c \
                 src/lexer.c \
                 src/ast/parse.c \
                 src/ast/typecheck.c \
                 src/ast/type.c \
                 src/ir.c \
                 src/posix/alloc.c

SRC_MR_UTILS  := src/mr_utils/src/mrd_debug.c \
                 src/mr_utils/src/mrl_logger.c \
                 src/mr_utils/src/mrs_strings.c \
                 src/mr_utils/src/mrt_test.c \
                 src/mr_utils/src/mrv_vectors.c

SRC_SPACERS   := src/mr_utils/tools/spacers.c

OBJ_LIB       := $(SRC_LIB:%.c=$(OBJ_DIR)/%.o)
OBJ_TEST_MAIN := $(SRC_TEST_MAIN:%.c=$(OBJ_DIR)/%.o)
OBJ_MR_UTILS  := $(SRC_MR_UTILS:%.c=$(OBJ_DIR)/%.o)
OBJ_SPACERS   := $(SRC_SPACERS:%.c=$(OBJ_DIR)/%.o)

ALL_MAIN_OBJS    := $(OBJ_LIB) $(OBJ_MR_UTILS)
ALL_TEST_OBJS    := $(OBJ_TEST_MAIN) $(OBJ_MR_UTILS)
ALL_SPACERS_OBJS := $(OBJ_MR_UTILS) $(OBJ_SPACERS)

.PHONY: all test run clean format format-check debug build-debug build-test-debug test-debug spacers valgrind record tags

all: $(TARGET_TEST) $(TARGET_SPACERS)

$(TARGET_TEST): $(ALL_TEST_OBJS)
	$(CC) $(ALL_TEST_OBJS) -o $@ $(LDFLAGS) $(LDLIBS) $(CFLAGS)

$(TARGET_MAIN): $(ALL_MAIN_OBJS)
	$(CC) $(ALL_MAIN_OBJS) -o $@ $(LDFLAGS) $(LDLIBS) $(CFLAGS)

$(TARGET_SPACERS): $(ALL_SPACERS_OBJS)
	$(CC) $(ALL_SPACERS_OBJS) -o $@ $(LDFLAGS) $(LDLIBS) $(CFLAGS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MD -c $< -o $@ -std=$(CSTANDARD) $(CFLAGS)

test: $(TARGET_TEST)
	./$(TARGET_TEST)

run: $(TARGET_MAIN)
	./$(TARGET_MAIN)

build-debug: $(TARGET_MAIN) $(TARGET_SPACERS)

build-test-debug: $(TARGET_TEST) $(TARGET_SPACERS)

debug: build-debug
	./$(TARGET_MAIN)

test-debug: build-test-debug
	./$(TARGET_TEST)

clean:
	rm -rf $(BUILD_DIR)
	rm -f TAGS

spacers: $(TARGET_SPACERS)

format: $(TARGET_SPACERS)
	find ./src ./test -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format -i --verbose
	git ls-files --recurse-submodules | xargs $(TARGET_SPACERS)

format-check: $(TARGET_SPACERS)
	find ./src ./test -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format --dry-run --Werror --verbose
	git ls-files --recurse-submodules | xargs $(TARGET_SPACERS)

valgrind:
	valgrind --leak-check=full --suppressions=valgrind.supp $(TARGET_TEST)

record:
	perf record -g --call-graph dwarf $(TARGET_TEST)
	perf script > chombo.perf

tags:
	ctags -e -R src/ test/

-include $(ALL_TEST_OBJS:.o=.d)
-include $(OBJ_SPACERS:.o=.d)
-include $(ALL_MAIN_OBJS:.o=.d)
