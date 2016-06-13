cmake_minimum_required ( VERSION 3.0 )
project ( DemonEdge )

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

find_package(Snappy REQUIRED)
include_directories(${SNAPPY_INCLUDE_DIR})


MESSAGE(STATUS "Generating Protobuf Files")
FILE(GLOB ProtoFiles ${CMAKE_SOURCE_DIR}/proto/*.proto)
execute_process(COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${ProtoFiles}
    -I${PROTOBUF_INCLUDE_DIR}
    --cpp_out=${CMAKE_SOURCE_DIR}/src/demonedge
    --proto_path=${CMAKE_SOURCE_DIR}/proto
)

SET(CMAKE_CXX_FLAGS "--std=c++11")

FILE(GLOB PROTO_SOURCES ${CMAKE_SOURCE_DIR}/src/demonedge/*.pb.cc)
FILE(GLOB CORE_SOURCES ${CMAKE_SOURCE_DIR}/src/demonedge/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src/demonedge)

add_library(dedge-core OBJECT ${CORE_SOURCES} ${PROTO_SOURCES})
add_library(dedge-core-static STATIC $<TARGET_OBJECTS:dedge-core>)

add_executable ( first
    example/first.cpp
)
target_link_libraries(first dedge-core-static ${SNAPPY_LIBRARIES} ${PROTOBUF_LIBRARIES})