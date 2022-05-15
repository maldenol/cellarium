SET(TARGET cellarium)

FILE(GLOB_RECURSE SRC_FILES "${SRC_DIR}/${TARGET}/*.cpp")
ADD_EXECUTABLE(${TARGET} ${SRC_FILES})

ADD_DEPENDENCIES(dev ${TARGET})

SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_EXTENSIONS OFF)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_STANDARD 17)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_STANDARD_REQUIRED ON)

TARGET_COMPILE_OPTIONS(${TARGET} PUBLIC
  $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wall -Wextra -Wpedantic -O3>
)
TARGET_LINK_OPTIONS(${TARGET} PUBLIC
  $<$<LINK_LANG_AND_ID:CXX,GNU,Clang>:-lX11 -O3>
)

TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${INC_DIR})
FILE(GLOB_RECURSE LIB_FILES "${LIB_DIR}/*.a" "${LIB_DIR}/*.so" "${LIB_DIR}/*.lib")
TARGET_LINK_LIBRARIES(${TARGET} PRIVATE ${LIB_FILES})

FIND_PACKAGE(Qt5 COMPONENTS Core Quick REQUIRED)
TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE Qt)
TARGET_LINK_LIBRARIES(${TARGET} PRIVATE ${Qt5Core_LIBRARIES} ${Qt5Quick_LIBRARIES})