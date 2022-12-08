SET(TARGET cellarium)

FILE(GLOB_RECURSE SRC_FILES "${SRC_DIR}/${TARGET}/*.cpp")
FILE(GLOB_RECURSE DEAR_IMGUI_SRC_FILES "${SRC_DIR}/dear_imgui/*.cpp")
FILE(GLOB_RECURSE EXTRA_SRC_FILES "${SRC_DIR}/extra/*.cpp")
FILE(GLOB_RECURSE GLAD_SRC_FILES "${SRC_DIR}/glad/*.c")
FILE(GLOB_RECURSE LINKED_LIST_SRC_FILES "${SRC_DIR}/linked_list/*.cpp")
SET(SRC_FILES ${SRC_FILES} ${DEAR_IMGUI_SRC_FILES} ${EXTRA_SRC_FILES} ${GLAD_SRC_FILES} ${LINKED_LIST_SRC_FILES})

ADD_EXECUTABLE(${TARGET} ${SRC_FILES})

ADD_DEPENDENCIES(dev ${TARGET})

SET_TARGET_PROPERTIES(${TARGET} PROPERTIES C_EXTENSIONS OFF)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES C_STANDARD 17)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES C_STANDARD_REQUIRED ON)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_EXTENSIONS OFF)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_STANDARD 17)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_STANDARD_REQUIRED ON)

TARGET_COMPILE_OPTIONS(${TARGET} PUBLIC
  $<$<PLATFORM_ID:Linux>:
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang>:
      -Wall -Wextra -Wpedantic
      $<$<CONFIG:Debug>: -g -fsanitize=address,undefined>
      $<$<CONFIG:Release>: -O3>
    >
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:
      -Wall -Wextra -Wpedantic
      $<$<CONFIG:Debug>: -g -fsanitize=address,undefined>
      $<$<CONFIG:Release>: -O3>
    >
  >
  $<$<PLATFORM_ID:Windows>:
    $<$<COMPILE_LANG_AND_ID:C,MSVC>:
      /Wall
      $<$<CONFIG:Debug>: /DEBUG /fsanitize=address,undefined>
      $<$<CONFIG:Release>: /O2>
    >
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:
      /Wall
      $<$<CONFIG:Debug>: /DEBUG /fsanitize=address,undefined>
      $<$<CONFIG:Release>: /O2>
    >
  >
)
TARGET_LINK_OPTIONS(${TARGET} PUBLIC
  $<$<PLATFORM_ID:Linux>:
    $<$<LINK_LANG_AND_ID:C,GNU,Clang>:
      -lX11
      $<$<CONFIG:Debug>: -g -fsanitize=address,undefined>
      $<$<CONFIG:Release>: -O3>
    >
    $<$<LINK_LANG_AND_ID:CXX,GNU,Clang>:
      -lX11
      $<$<CONFIG:Debug>: -g -fsanitize=address,undefined>
      $<$<CONFIG:Release>: -O3>
    >
  >
  $<$<PLATFORM_ID:Windows>:
    $<$<LINK_LANG_AND_ID:C,MSVC>:
      $<$<CONFIG:Debug>: /DEBUG /fsanitize=address,undefined>
      $<$<CONFIG:Release>: /O2>
    >
    $<$<LINK_LANG_AND_ID:CXX,MSVC>:
      $<$<CONFIG:Debug>: /DEBUG /fsanitize=address,undefined>
      $<$<CONFIG:Release>: /O2>
    >
  >
)

TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${INC_DIR} ${SRC_DIR})
FILE(GLOB_RECURSE LIB_FILES "${LIB_DIR}/*.a" "${LIB_DIR}/*.so" "${LIB_DIR}/*.lib")
TARGET_LINK_LIBRARIES(${TARGET} PRIVATE ${LIB_FILES})

FIND_PACKAGE(Qt5 COMPONENTS Core REQUIRED)
TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE Qt)
TARGET_LINK_LIBRARIES(${TARGET} PRIVATE ${Qt5Core_LIBRARIES})
