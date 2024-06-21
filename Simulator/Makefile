

TARGET_EXEC := NOC

BUILD_DIR := ./build

SRC_DIRS := ./src
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp') 


INC_DIRS := ./include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP -g  # 加入 -g 以启用调试信息

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

# 新增一个目标来运行程序，允许生成 core 文件
run: $(BUILD_DIR)/$(TARGET_EXEC)
	ulimit -c unlimited; ./$(BUILD_DIR)/$(TARGET_EXEC)

# 这个目标是专门用来调试已经崩溃并生成了 core 文件的程序。
debug-core: 
	gdb -c core $(BUILD_DIR)/$(TARGET_EXEC)
# 使用 -c core 参数启动 GDB，这告诉 GDB 加载指定的 core 文件。Core 文件包含了程序崩溃时的内存快照、寄存器状态、程序计数器位置等信息，非常有助于分析程序崩溃的原因。


# 这个目标用于直接启动 GDB 调试正在运行的或是可重新启动的程序。
debug: $(BUILD_DIR)/$(TARGET_EXEC)  # 新增 debug 目标
	gdb $(BUILD_DIR)/$(TARGET_EXEC) 
# 这个目标用于直接启动 GDB 调试正在运行的或是可重新启动的程序。


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
	rm -f ./core*

-include $(DEPS)
