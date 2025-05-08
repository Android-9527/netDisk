cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 17)

# 包含头文件目录
include_directories(../include)
include_directories(/home/ub20/download/cppjieba/include/cppjieba)         # cppjieba 目录
include_directories(/home/ub20/download/cppjieba/deps/limonp/include)  

# 添加源文件
add_executable(MyProject
    # test2.cpp
    test.cpp
    ../src/DictProducer.cpp
    ../src/WordQuery.cpp
    ../src/WordSegmenter.cpp
)
# 添加源文件
# add_executable(test
#     test2.cpp
    
# )






