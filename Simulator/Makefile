
TARGET_EXEC := NOC

BUILD_DIR := ./build

SRC_DIRS := ./src
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp') 


INC_DIRS := ./include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP

DEPS := $(OBJS:.o=.d)
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# 将目标文件链接成最终的可执行文件。
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)


# C++源代码构建步骤
$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
# $@ 代表目标文件，即最终生成的可执行文件。
# $< 第一个依赖文件（即源文件）

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)