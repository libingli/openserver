#设置编译器
CC = g++
#最终bin文件的名字，可以更改为自己需要的
BIN = gtest
LIBS = -lpthread -ldl 

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread

#debug文件夹里的makefile文件需要最后执行，所以这里需要执行的子目录要排除debug文件夹，这里使用awk排除了debug文件夹，读取剩下的文件夹
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
#记住当前工程的根目录路径
GTEST_DIR = $(shell pwd)
USER_SRC_DIR = $(GTEST_DIR)/../src

#目标文件所在的目录
OBJS_DIR = $(GTEST_DIR)/build

#获取当前目录下的c文件集，放在变量USER_SRC中
USER_SRC = $(shell ls $(USER_SRC_DIR)/*.c | grep -v main)
#将对应的c文件名转为o文件后放在下面的USER_OBJS变量中
USER_OBJS = ${patsubst %.c, %.o, $(USER_SRC)}

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include
CPPFLAGS += -isystem $(USER_SRC_DIR)

#将以下变量导出到子shell中，本次相当于导出到子目录下的makefile中
export CC OBJS_DIR GTEST_DIR USER_SRC_DIR 

#注意这里的顺序，需要先执行SUBDIRS最后才能是DEBUG
all:SOURCE GTEST_TEST GTEST_SRC $(BIN) 

#递归执行子目录下的makefile文件，这是递归执行的关键
$(SUBDIRS):
	make -C $@
GTEST_TEST:
	make -C $(GTEST_DIR)/test
GTEST_SRC:
	make -C $(GTEST_DIR)/src

#将c文件编译为o文件，并放在指定放置目标文件的目录中即OBJS_DIR
$(USER_OBJS):%.o:%.c
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $^ -o $@

SOURCE:
	@cp $(USER_SRC_DIR)/*.c $(OBJS_DIR); rm -rf $(OBJS_DIR)/main.c;
	@find $(OBJS_DIR) -name "*.c" | awk -F "." '{print $$1}' | xargs -i -t mv {}.c  {}.cc;
	@make -C $(OBJS_DIR)

$(BIN):
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(OBJS_DIR)/*.o $(OBJS_DIR)/gtest_main.a -o $@ $(LIBS)

clean:
	@rm -f $(OBJS_DIR)/*.o $(BIN)
	@rm -f $(OBJS_DIR)/*.cc
