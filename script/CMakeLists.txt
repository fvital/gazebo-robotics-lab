

# Find and link to Gazebo libraries
include_directories(
  ${GAZEBO_INCLUDE_DIRS}
  )
link_directories(${GAZEBO_LIBRARY_DIRS})


# Add the plugin library and link it to Gazebo
add_library(welcome SHARED welcome.cpp)
target_link_libraries(welcome ${GAZEBO_LIBRARIES})


add_library(hoverboard SHARED hoverboard.cpp)
target_link_libraries(hoverboard ${GAZEBO_LIBRARIES})

if(BUILD_EXTENDED)
  add_subdirectory(msgs)

  add_library(gripper SHARED gripper.cpp)
  target_link_libraries(gripper ${GAZEBO_LIBRARIES} ${PROTOBUF_LIBRARIES} custom_msg)

  add_executable(vel_cmd vel_cmd.cpp)
  add_executable(gripper_cmd gripper_cmd.cpp)
  add_executable(gripper_pid_cfg gripper_pid_cfg.cpp)

  if (${gazebo_VERSION_MAJOR} LESS 6)
    include(FindBoost)
    find_package(Boost ${MIN_BOOST_VERSION} REQUIRED system filesystem regex)
    target_link_libraries(vel_cmd ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES})
    target_link_libraries(gripper_cmd ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} custom_msg)
    target_link_libraries(gripper_pid_cfg ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} custom_msg)
  else()
    target_link_libraries(vel_cmd ${GAZEBO_LIBRARIES})
    target_link_libraries(gripper_cmd ${GAZEBO_LIBRARIES} custom_msg)
    target_link_libraries(gripper_pid_cfg ${GAZEBO_LIBRARIES} ${PROTOBUF_LIBRARIES} custom_msg)
  endif()

endif()