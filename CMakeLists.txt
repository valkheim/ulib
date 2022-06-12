if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE sources_asm "${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm")
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_library(ulib STATIC ${sources} ${sources_asm} ${headers})
target_include_directories(ulib
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_subdirectory(test)