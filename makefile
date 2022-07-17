
CPPC = /usr/local/Cellar/llvm/14.0.6_1/bin/clang++
CPPCFLAGS = -std=c++20 -Wall -Wextra -Werror -Wpedantic -I include/

SRCS = $(wildcard src/*)
OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))

TESTS = $(wildcard tests/*)
TEST_BINS = $(patsubst tests/%.cpp, build/tests/%, $(TESTS))

LIB = build/my_utils.a


# Formatting

BOLD = $$(tput bold)
YELLOW = $$(tput setaf 3)
GREEN = $$(tput setaf 2)
WHITE = $$(tput setaf 7)

CLEAR = $$(tput sgr0)


# Entrypoint Commands

.PHONY:
build: $(LIB)

.PHONY:
test: build $(TEST_BINS)
	@echo [$(BOLD)$(YELLOW)RUNNING TESTS$(CLEAR)]
	@$(foreach file, $(TEST_BINS), \
		echo && echo $(BOLD)$(WHITE)@ $(file:build/tests/%=%)$(CLEAR) && $(file) &&) \
	\
	echo && echo [$(BOLD)$(GREEN)ALL TESTS PASSED$(CLEAR)]

.PHONY:
clean:
	@echo removing build directory
	@rm -r -f build/


# Building Rules

$(LIB): build_folder $(OBJS)
	@ar rs $(LIB) $(OBJS)

.PHONY:
build_folder:
	@mkdir -p build/
	@mkdir -p build/tests

build/%.o: src/%.cpp include/*
	@echo $@
	@$(CPPC) $(CPPCFLAGS) -c $< -o $@

build/tests/%: tests/%.cpp include/*
	@echo $@
	@$(CPPC) $(CPPCFLAGS) $< $(LIB) -o $@
