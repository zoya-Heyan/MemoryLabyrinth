cmake_minimum_required(VERSION 3.16)
project(MemoryLabyrinth)

set(CMAKE_CXX_STANDARD 17)

# 查找 SFML
# 在 macOS 上，尝试使用 Homebrew 安装路径
if(APPLE)
    set(SFML_ROOT "/opt/homebrew/opt/sfml@2" CACHE PATH "SFML root directory")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${SFML_ROOT})
endif()

find_package(SFML 2 COMPONENTS graphics window system QUIET)

# 如果找不到，尝试手动设置路径（macOS Homebrew）
if(NOT SFML_FOUND AND APPLE)
    set(SFML_INCLUDE_DIR "/opt/homebrew/opt/sfml@2/include")
    set(SFML_LIB_DIR "/opt/homebrew/opt/sfml@2/lib")
    
    if(EXISTS ${SFML_INCLUDE_DIR})
        message(STATUS "Using SFML from: ${SFML_INCLUDE_DIR}")
        set(SFML_FOUND TRUE)
    endif()
endif()

if(NOT SFML_FOUND)
    message(FATAL_ERROR "SFML not found! Please install SFML (e.g., brew install sfml@2)")
endif()

# 添加可执行文件
add_executable(MemoryLabyrinth
    src/main.cpp
    src_modules/StoryGame.cpp
    src_modules/ParticleSystem.cpp
    src_modules/TextEffect.cpp)

# 包含目录
target_include_directories(MemoryLabyrinth PRIVATE include)

# 链接 SFML
if(SFML_FOUND AND TARGET sfml-graphics)
    target_link_libraries(MemoryLabyrinth PRIVATE sfml-graphics sfml-window sfml-system)
else()
    # 手动链接（如果 find_package 没有创建目标）
    target_include_directories(MemoryLabyrinth PRIVATE ${SFML_INCLUDE_DIR})
    target_link_directories(MemoryLabyrinth PRIVATE ${SFML_LIB_DIR})
    target_link_libraries(MemoryLabyrinth PRIVATE 
        sfml-graphics 
        sfml-window 
        sfml-system
    )
endif()