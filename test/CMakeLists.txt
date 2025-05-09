cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 17)

# 包含头文件目录
include_directories(../include)
include_directories(/home/ub20/download/cppjieba/include/cppjieba)         # cppjieba 目录
include_directories(/home/ub20/download/cppjieba/deps/limonp/include)  

# 添加源文件
# add_executable(MyProject
#     # test2.cpp
#     test.cpp
#     ../src/DictProducer.cpp
#     ../src/WordQuery.cpp
#     ../src/WordSegmenter.cpp
# )
# 添加源文件
# add_executable(test
#     test2.cpp
    
# )

# 查找所有 .cc 文件
file(GLOB SRC_FILES 
    "../src/*.cc"
    # "../src/*.cpp"  # 同时包含 .cpp 文件
)

# 添加可执行文件，包含全部源文件
add_executable(MyProject
            # 主测试文件
    ${SRC_FILES}    # 所有源文件
)

# 添加线程库链接
target_link_libraries(MyProject pthread)




