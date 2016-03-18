CFLAGS := -Wall -Werror -Wfatal-errors
CFLAGS += -std=c11
CFLAGS += -O0
CFLAGS += -ggdb3
CFLAGS += -MMD
CFLAGS += -I include

CLIENT := client
SERVER := server
LIB    := lib

TEMP := build

CLIENT_SRC := $(shell find src/$(CLIENT)/* -type f -name "*.c")
CLIENT_OBJ := $(CLIENT_SRC:%.c=$(TEMP)/%.o)
CLIENT_DEP := $(CLIENT_SRC:%.c=$(TEMP)/%.d)

SERVER_SRC := $(shell find src/$(SERVER)/* -type f -name "*.c")
SERVER_OBJ := $(SERVER_SRC:%.c=$(TEMP)/%.o)
SERVER_DEP := $(SERVER_SRC:%.c=$(TEMP)/%.d)

LIB_SRC := $(shell find src/$(LIB)/* -type f -name "*.c")
LIB_OBJ := $(LIB_SRC:%.c=$(TEMP)/%.o)
LIB_DEP := $(LIB_SRC:%.c=$(TEMP)/%.d)

$(CLIENT): $(CLIENT_OBJ) $(LIB_OBJ)
	@$(CC) $^ -o $@
	@echo +ld $^

$(SERVER): $(SERVER_OBJ) $(LIB_OBJ)
	@$(CC) $^ -lpthread -o $@
	@echo +ld $^

$(TEMP)/%.o: %.c
	@mkdir -p $(TEMP)/$(dir $<)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo +cc $<

-include $(CLIENT_DEP)

-include $(SERVER_DEP)

-include $(LIB_DEP)

.PHONY: clean run-cli

clean:
	-@rm -rf $(TEMP) 2> /dev/null
	-@rm $(CLIENT) 2> /dev/null

run-cli: $(CLIENT)
	@./$<

run-server: $(SERVER)
	@./$<
