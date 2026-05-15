#include <gazebo/gazebo.hh> // Include the main Gazebo header file

namespace gazebo
{
	// Define a new class WelcomeWorldPlugin tat inherits from the WorldPlugin class
	class WelcomeWorldPlugin : public WorldPlugin
	{
	public:
		// Constructor for WelcomeWorldPlugin
		WelcomeWorldPlugin() : WorldPlugin()
		{
			printf("Welcome to Filipe’s World!!\n");
		}

	public:
		// Load function is called by Gazebo when loading the plugin
		void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf)
		{
		}
	};
	// Register this plugin with Gazebo,
	// making it discoverable at runtime
	GZ_REGISTER_WORLD_PLUGIN(WelcomeWorldPlugin)
}
