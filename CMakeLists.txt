cmake_minimum_required(VERSION 3.5)

project(jsonlib LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


#find_library(TBB REQUIRED)
#find_package(TBB REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(CTest)
enable_testing()
#add_subdirectory(src)

add_executable(${PROJECT_NAME}    main.cpp
                                  json.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
Threads::Threads
)
#TBB::tbb
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
