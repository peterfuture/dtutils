# CMake version
cmake_minimum_required (VERSION 2.8)

# project
project (dtutils)

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# DEFS
ADD_DEFINITIONS(-DENABLE_LINUX)
#ADD_DEFINITIONS(-DENABLE_ANDROID)

# target - lib
INCLUDE_DIRECTORIES(include)
AUX_SOURCE_DIRECTORY(src SRC_UTILS)
ADD_LIBRARY(dtutils STATIC ${SRC_UTILS})
if(BUILD_FOR_ANDROID)
    MESSAGE("Android pthread prebuild to libc")
else()
    TARGET_LINK_LIBRARIES(dtutils pthread)
endif()

# target - test
ADD_EXECUTABLE(test_list test/test_list.c)
TARGET_LINK_LIBRARIES(test_list dtutils)
ADD_EXECUTABLE(test_event test/test_event.c)
TARGET_LINK_LIBRARIES(test_event dtutils)
ADD_EXECUTABLE(test_pool test/test_pool.c)
TARGET_LINK_LIBRARIES(test_pool dtutils)

if(BUILD_FOR_ANDROID)
    MESSAGE("Android Can Not Install")
else()
    INSTALL(TARGETS dtutils
        #RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
    INSTALL(DIRECTORY include/ DESTINATION /usr/local/include/${PROJECT_NAME})
endif()
