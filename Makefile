SRC_DIRS := ./src
BUILD_DIR := ./build
TARGET_EXEC := $(BUILD_DIR)/pesilisp

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:%.o=%.d)
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CFLAGS := $(INC_FLAGS) -MMD -MP -O2 -g
LDFLAGS := -fsanitize=leak -fsanitize=address

.PHONY: all
build: $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJS): $(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
