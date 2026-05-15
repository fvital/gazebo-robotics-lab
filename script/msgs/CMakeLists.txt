# Protobuf setup

find_package(Protobuf REQUIRED)
set(PROTOBUF_IMPORT_DIRS)
foreach(ITR ${GAZEBO_INCLUDE_DIRS})
  if(ITR MATCHES ".*gazebo-[0-9.]+$")
    set(PROTOBUF_IMPORT_DIRS "${ITR}/gazebo/msgs/proto")
  endif()
endforeach()
set (msgs
  custom_msg.proto
  ${PROTOBUF_IMPORT_DIRS}/vector3d.proto
)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${msgs})
add_library(custom_msg SHARED ${PROTO_SRCS})
target_link_libraries(custom_msg ${PROTOBUF_LIBRARY})

target_include_directories(custom_msg PUBLIC ${CMAKE_CURRENT_BINARY_DIR})