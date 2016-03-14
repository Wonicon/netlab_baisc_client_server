CFLAGS := -Wall -Werror -Wfatal-errors
CFLAGS += -std=c11
CFLAGS += -O0
CFLAGS += -ggdb3
CFLAGS += -MMD

CLIENT := client

TEMP := build

CLIENT_SRC := $(shell find src/$(CLIENT) * -type f -name "*.c")
CLIENT_OBJ := $(CLIENT_SRC:%.c=$(TEMP)/%.o)
CLIENT_DEP := $(CLIENT_SRC:%.c=$(TEMP)/%.d)

-include $(DEP)

$(CLIENT): $(CLIENT_OBJ)
	@$(CC) $^ -o $@
	@echo +ld $^

$(TEMP)/%.o: %.c
	@mkdir -p $(TEMP)/$(dir $<)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo +cc $<

.PHONY: clean run-cli

clean:
	-rm -rf $(TEMP) 2> /dev/null

run-cli: $(CLIENT)
	./$<
