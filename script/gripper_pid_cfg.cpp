#include <string>

#include <gazebo/gazebo_config.h>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>

// Gazebo's API has changed between major releases. These changes are
// accounted for with #if..#endif blocks in this file.
#if GAZEBO_MAJOR_VERSION < 6
#include <gazebo/gazebo.hh>
#else
#include <gazebo/gazebo_client.hh>
#endif

#include "custom_msg.pb.h"
#include "custom_msg.h"

/////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  // Load gazebo as a client
#if GAZEBO_MAJOR_VERSION < 6
  gazebo::setupClient(_argc, _argv);
#else
  gazebo::client::setup(_argc, _argv);
#endif

  // Create our node for communication
  gazebo::transport::NodePtr node(new gazebo::transport::Node());
  node->Init();

  // Publish to the gripper pid cfg topic
  gazebo::transport::PublisherPtr pub =
      node->Advertise<custom_msg::msgs::PidCfgRequest>("~/" + std::string(_argv[1]) + "/gripper_pid_cfg");

  // Wait for a subscriber to connect to this publisher
  pub->WaitForConnection();

  // Create a PidCfgRequest message
  custom_msg::msgs::PidCfgRequest msg;

  // Set the target joint type and new pid parameters

  msg.set_joint_id(std::string(_argv[2]));
  msg.mutable_pid_params()->set_x(std::atof(_argv[3]));
  msg.mutable_pid_params()->set_y(std::atof(_argv[4]));
  msg.mutable_pid_params()->set_z(std::atof(_argv[5]));

  // Send the message
  pub->Publish(msg);

  // Make sure to shut everything down.
#if GAZEBO_MAJOR_VERSION < 6
  gazebo::shutdown();
#else
  gazebo::client::shutdown();
#endif
}