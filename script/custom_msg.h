#ifndef _CUSTOM_MSG_HH_
#define _CUSTOM_MSG_HH_

#include <string>

#include <boost/shared_ptr.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/msgs/msgs.hh>

#include "custom_msg.pb.h"

namespace gazebo
{
    typedef const boost::shared_ptr<custom_msg::msgs::PidCfgRequest const> PidCfgRequestPtr;
}

#endif // _CUSTOM_MSG_HH_