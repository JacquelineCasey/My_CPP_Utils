
CPPC = /usr/local/Cellar/llvm/14.0.6_1/bin/clang++
CPPCFLAGS = -std=c++20 -Wall -Wextra -Werror -Wpedantic -I include/ -O3

SRCS = $(shell find src -type f -name '*.cpp' | sort)
OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))

INCLUDES = $(shell find include -type f -name '*.h')

TESTS = $(shell find tests -type f -name '*.cpp' | sort)
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

$(LIB): $(OBJS)
	@echo $@
	@ar rs $(LIB) $(OBJS) 2> /dev/null

build/%.o: src/%.cpp $(INCLUDES)
	@echo $@
	@mkdir -p $(@D)
	@$(CPPC) $(CPPCFLAGS) -c $< -o $@

build/tests/%: tests/%.cpp $(INCLUDES)
	@echo $@
	@mkdir -p $(@D)
	@$(CPPC) $(CPPCFLAGS) $< $(LIB) -o $@
