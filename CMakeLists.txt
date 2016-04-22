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
TARGET_LINK_LIBRARIES(dtutils pthread)

# target - test
ADD_EXECUTABLE(test_list test/test_list.c)
TARGET_LINK_LIBRARIES(test_list dtutils)
ADD_EXECUTABLE(test_event test/test_event.c)
TARGET_LINK_LIBRARIES(test_event dtutils)

INSTALL(TARGETS dtutils
    #RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
INSTALL(DIRECTORY include/ DESTINATION /usr/local/include/${PROJECT_NAME})
