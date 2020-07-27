CXX = g++
CXXFLAGS = -I. -Iinclude -std=c++11 -Os -fno-exceptions -fno-rtti -fno-stack-protector -fmerge-all-constants -nostdlib++
EXEFLAGS = -ffunction-sections -fdata-sections

SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp,src/%.o,$(SRCS))

TESTS = $(wildcard test/*.cpp)
TEST_EXES = $(patsubst test/%.cpp,test/%.exe,$(TESTS))
TEST_DSYMS = $(patsubst test/%.exe,test/%.exe.dSYM,$(TEST_EXES))

clean:
	rm -rf $(TEST_EXES) $(TEST_DSYMS) $(OBJS)

jasmine: $(OBJS) jasmine.cpp
	$(CXX) $(CXXFLAGS) $(EXEFLAGS) $^ -o $@

test: $(TEST_EXES)
	for f in $^; do echo " - running test" $$f && ./$$f; done 

%.exe: %.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(EXEFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@