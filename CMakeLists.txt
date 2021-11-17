# 声明要求的cmake版本
cmake_minimum_required(VERSION 3.10)

# 工程名
project(HTTP)

# 添加标准支持
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS -pthread)

set(CXX_FLAGS
-g
-DCHECK_PTHREAD_RETURN_VALUE
-Wall #大部分警告
-Wextra # 一些额外的警告
# -Werror # 当出现警告时转变为错误，停止编译
-Wconversion # 一些可能改变值的隐式转换，给出警告
-Wno-unused-parameter #函数中出现未使用的参数，不给出警告
-Wold-style-cast #c风格的变换给出警告
-Woverloaded-virtual # 如果函数的声明隐藏住了基类的虚函数，就给出警告
-Wpointer-arith #对函数指针或者void*类型的指针进行算术操作时给出警告
-Wshadow #当一个局部变量遮盖了另一个局部变量，或全剧变量时给出警告
-WWrite-strings # 规定字符串常量的类型是const char[length],因此把这样的地址复制给non-const char*将产生警告，同时避免向字符串常量写入的行为
)

# 设置编辑器
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_BUILD_TYPE "Debug")

# 设置头文件
include_directories(./Mutex)
add_subdirectory(./Mutex)
# 添加需要链接的库目录
link_directories(./Mutex)
# 目标
set(SRC main.cpp http.cpp)
add_executable(demo ${SRC})
# 设置需要链接的库文件名称
target_link_libraries(demo libMutex.so)