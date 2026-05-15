#ifndef _GRIPPER_PLUGIN_HH_
#define _GRIPPER_PLUGIN_HH_

#include <string>

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>

#include "custom_msg.h"

namespace gazebo
{

  class GripperPlugin : public ModelPlugin
  {

  private:
    /// \brief Pointer to the model.
    physics::ModelPtr model;
    /// \brief Pointers to the joints.
    physics::JointPtr riser_joint;
    physics::JointPtr right_finger_joint;
    physics::JointPtr left_finger_joint;
    physics::JointPtr right_tip_joint;
    physics::JointPtr left_tip_joint;

    /// \brief PID controllers for the joints.
    common::PID riser_pid;
    common::PID rf_pid;
    common::PID lf_pid;
    common::PID rt_pid;
    common::PID lt_pid;

    /// \brief A node used for transport
    transport::NodePtr node;

    /// \brief A subscriber to a named topic.
    transport::SubscriberPtr sub_cmd;
    transport::SubscriberPtr sub_pid_cfg;

    /// \brief Handle incoming position command message
    /// \param[in] _msg Repurpose a vector3 message.
    void OnCmdMsg(ConstVector3dPtr &_msg)
    {
      SetPosition(_msg->x(), _msg->y(), _msg->z());
    }

    /// \brief Handle incoming pid configuration message
    void OnPidCfgMsg(PidCfgRequestPtr &_msg)
    {
      SetPID(_msg->joint_id(), _msg->pid_params().x(), _msg->pid_params().y(), _msg->pid_params().z());
    }

  public:
      virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
      // Just output a message for now
      std::cerr << "\nThe Gripper plugin is attached to model[" << _model->GetName() << "]\n";

      // Safety check
      if (_model->GetJointCount() < 5)
      {
        std::cerr << "Invalid joint count, Gripper plugin not loaded\n";
        return;
      }

      // Store the model pointer for convenience.
      model = _model;

      right_finger_joint = model->GetJoint("right_finger_joint");
      left_finger_joint = model->GetJoint("left_finger_joint");
      right_tip_joint = model->GetJoint("right_tip_joint");
      left_tip_joint = model->GetJoint("left_tip_joint");
      riser_joint = model->GetJoint("riser_joint");

      // Setup a PID-controllers.

      double K = 8;
      double a1 = 0.5;
      double a2 = 5;

      rf_pid = common::PID(K * (a1 + a2), K * (a1 * a2), K, 1000, -1000);
      lf_pid = common::PID(K * (a1 + a2), K * (a1 * a2), K, 1000, -1000);
      rt_pid = common::PID(K * (a1 + a2), K * (a1 * a2), K, 1000, -1000);
      lt_pid = common::PID(K * (a1 + a2), K * (a1 * a2), K, 1000, -1000);
      riser_pid = common::PID(K * (a1 + a2), K * (a1 * a2), K, 1000, -1000);

      // Apply the default PID-controllers to the joints.
      model->GetJointController()->SetPositionPID(
          right_finger_joint->GetScopedName(), rf_pid);

      model->GetJointController()->SetPositionPID(
          left_finger_joint->GetScopedName(), lf_pid);

      model->GetJointController()->SetPositionPID(
          right_tip_joint->GetScopedName(), rt_pid);
      model->GetJointController()->SetPositionPID(
          left_tip_joint->GetScopedName(), lt_pid);

      model->GetJointController()->SetPositionPID(
          riser_joint->GetScopedName(), riser_pid);

      // Default to zero position
      double finger_position = 0;
      double tip_position = 0;
      double riser_position = 0;

      // Check that the position elements exist, then read the values.
      // Update defaults to use values defined in the sdf file.
      if (_sdf->HasElement("riser_position"))
        riser_position = _sdf->Get<double>("riser_position");
      if (_sdf->HasElement("finger_position"))
        finger_position = _sdf->Get<double>("finger_position");
      if (_sdf->HasElement("tip_position"))
        tip_position = _sdf->Get<double>("tip_position");

      // Set the joint's target positions.
      SetPosition(riser_position, finger_position, tip_position);

      // Create the node

      node = transport::NodePtr(new transport::Node());
#if GAZEBO_MAJOR_VERSION < 8
      node->Init(model->GetWorld()->GetName());
#else
      node->Init(model->GetWorld()->Name());
#endif

      // Create a topic name
      std::string cmdTopicName = "~/" + model->GetName() + "/gripper_cmd";
      std::string pidCfgTopicName = "~/" + model->GetName() + "/gripper_pid_cfg";

      // Subscribe to the topic, and register a callback
      sub_cmd = node->Subscribe(cmdTopicName, &GripperPlugin::OnCmdMsg, this);
      sub_pid_cfg = node->Subscribe(pidCfgTopicName, &GripperPlugin::OnPidCfgMsg, this);
    }

    /// \brief Set the positions of the gripper. Assuming tips' and finger's positions are symmetrical.
    /// \param[in] _riser_position New target riser position
    /// \param[in] _finger_position New target finger position
    /// \param[in] _tip_position New target tip position
    void SetPosition(const double &_riser_position, const double &_finger_position, const double &_tip_position)
    {

      model->GetJointController()->SetPositionTarget(
          riser_joint->GetScopedName(), _riser_position);

      model->GetJointController()->SetPositionTarget(
          right_finger_joint->GetScopedName(), _finger_position);
      model->GetJointController()->SetPositionTarget(
          left_finger_joint->GetScopedName(), _finger_position);

      model->GetJointController()->SetPositionTarget(
          right_tip_joint->GetScopedName(), _tip_position);
      model->GetJointController()->SetPositionTarget(
          left_tip_joint->GetScopedName(), _tip_position);
    }

    void SetPID(
        const std::string &_joint_type,
        const double _Kp,
        const double _Ki,
        const double _Kd)
    {
      auto applyPID = [&](common::PID &pid, physics::JointPtr joint)
      {
        pid.SetPGain(_Kp);
        pid.SetIGain(_Ki);
        pid.SetDGain(_Kd);
        model->GetJointController()->SetPositionPID(joint->GetScopedName(), pid);
      };

      if (_joint_type == "finger")
      {
        applyPID(rf_pid, right_finger_joint);
        applyPID(lf_pid, left_finger_joint);
      }
      else if (_joint_type == "tip")
      {
        applyPID(rt_pid, right_tip_joint);
        applyPID(lt_pid, left_tip_joint);
      }
      else if (_joint_type == "riser")
      {
        applyPID(riser_pid, riser_joint);
      }
      else
      {
        applyPID(riser_pid, riser_joint);
        applyPID(rf_pid, right_finger_joint);
        applyPID(lf_pid, left_finger_joint);
        applyPID(rt_pid, right_tip_joint);
        applyPID(lt_pid, left_tip_joint);
      }
    }
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(GripperPlugin)
}

#endif // _GRIPPER_PLUGIN_HH_