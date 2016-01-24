#子目录的Makefile直接读取其子目录就行
SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')

#以下同根目录下的makefile的相同代码的解释
CUR_SOURCE=${wildcard *.cc}
CUR_OBJS=${patsubst %.cc, %.o, $(CUR_SOURCE)}

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include
CPPFLAGS += -isystem $(USER_SRC_DIR)

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

all:$(CUR_OBJS)

$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.cc 
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -I$(USER_SRC_DIR) -I$(GTEST_DIR)/include -c $^ -o $(OBJS_DIR)/$@
ECHO:
	@echo $(SUBDIRS)
