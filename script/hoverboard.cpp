#ifndef _HOVERBOARD_PLUGIN_HH_
#define _HOVERBOARD_PLUGIN_HH_

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>

namespace gazebo
{
  class HoverboardPlugin : public ModelPlugin
  {
  public:
    virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
      // Just output a message
      std::cerr << "\nThe Hoverboard plugin is attached to model[" << _model->GetName() << "]\n";

      // Safety check
      if (_model->GetJointCount() < 2)
      {
        std::cerr << "Invalid joint count, Hoverboard plugin not loaded\n";
        return;
      }

      // Store the model pointer for convenience.
      model = _model;

      r_joint = model->GetJoint("right_wheel_joint");
      l_joint = model->GetJoint("left_wheel_joint");

      // Setup a P-controllers, with a gain of 0.1.
      r_pid = common::PID(0.1, 0, 0);
      l_pid = common::PID(0.1, 0, 0);

      // Apply the P-controller to the joint.
      model->GetJointController()->SetVelocityPID(
          r_joint->GetScopedName(), r_pid);

      model->GetJointController()->SetVelocityPID(
          l_joint->GetScopedName(), l_pid);

      // Default to zero velocity
      double v = 0;
      double w = 0;

      // Check that the velocity elements exist, then read the values.
      // Update defaults to use values defined in the sdf file.
      if (_sdf->HasElement("lin_velocity"))
        v = _sdf->Get<double>("lin_velocity");
      if (_sdf->HasElement("ang_velocity"))
        w = _sdf->Get<double>("ang_velocity");

      // Set the joint's target velocity.
      SetVelocity(v, w);

      // Create the node

      node = transport::NodePtr(new transport::Node());
#if GAZEBO_MAJOR_VERSION < 8
      node->Init(model->GetWorld()->GetName());
#else
      node->Init(model->GetWorld()->Name());
#endif

      // Create a topic name
      std::string topicName = "~/" + model->GetName() + "/vel_cmd";

      // Subscribe to the topic, and register a callback
      sub = node->Subscribe(topicName, &HoverboardPlugin::OnMsg, this);
    }

    /// \brief Set the velocities of the hoverboard
    /// \param[in] _lin_vel New target linear velocity
    /// \param[in] _ang_vel New target angular velocity
    void SetVelocity(const double &_lin_vel, const double &_ang_vel)
    {

      model->GetJointController()->SetVelocityTarget(
          r_joint->GetScopedName(), (2 * _lin_vel + _ang_vel * L) / (2 * R));

      model->GetJointController()->SetVelocityTarget(
          l_joint->GetScopedName(), (2 * _lin_vel - _ang_vel * L) / (2 * R));
    }

  private:
    /// \brief Pointer to the model.
    physics::ModelPtr model;
    /// \brief Pointers to the joints.
    physics::JointPtr r_joint;
    physics::JointPtr l_joint;

    /// \brief A PID controller for the joints.
    common::PID r_pid;
    common::PID l_pid;

    double R = 0.2;
    double L = 1;

    /// \brief A node used for transport
    transport::NodePtr node;

    /// \brief A subscriber to a named topic.
    transport::SubscriberPtr sub;

    /// \brief Handle incoming message
    /// \param[in] _msg Repurpose a vector3 message.
    /// This function will only use the x and y components.
    void OnMsg(ConstVector3dPtr &_msg)
    {
      SetVelocity(_msg->x(), _msg->y());
    }
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(HoverboardPlugin)
}

#endif // _HOVERBOARD_PLUGIN_HH_