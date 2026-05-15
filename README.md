# Gazebo Robotics Lab

A personal simulation asset library and testbed for robotics development
in Gazebo. Contains a growing collection of robot models, building and
environment models, and world files that compose those assets into
scenario-specific simulation environments.

Work here supports ongoing projects in controls, motion planning, and
autonomous navigation. The lab started as part of the
[Udacity Robotics Software Engineer Nanodegree](https://www.udacity.com/course/robotics-software-engineer--nd209)
and is being extended continuously beyond those assignments.

---

## Repository Structure
.
├── model/
│   ├── <building_name>/      # Architectural environments
│   │   ├── model.config
│   │   └── model.sdf
│   └── <robot_name>/         # Robot models
│       ├── model.config
│       └── model.sdf
├── world/
│   └── <scenario>.world      # World files composing models into scenes
├── script/
│   └── <plugin>.cpp          # Gazebo C++ plugins
├── CMakeLists.txt
├── CMakePresets.json
└── README.md

Models are resolved via `GAZEBO_MODEL_PATH` (see setup below), so any
world file in `world/` can reference any model in `model/` regardless
of which project it was originally built for.

---

## Branches

| Branch | Stack | Notes |
|--------|-------|-------|
| `main` | Gazebo Classic 11 · ROS1 Noetic | Course-track submissions |
| `ros2-humble` | Ignition Fortress (Gazebo Sim 6) · ROS2 Humble | (Planned) Modern port, Jetson Orin Nano |

The ROS2 branch documents API differences from ROS1 Noetic explicitly
in its own README sections. Upgrade path to ROS2 Jazzy + Gazebo Harmonic
is planned when JetPack 7 (Ubuntu 24.04) becomes available for the
Jetson Orin Nano.

---

## Current Contents

### Models

| Name | Type | Description |
|------|------|-------------|
| `building1` | Building | Single-floor structure with interior partitions; used in `world2.world` |
| `hoverboard` | Robot | Differential-drive chassis — revolute left and right wheel joints; controlled via `libhoverboard.so` |
| `gripper_base` | Robot | 6-DOF gripper arm — prismatic riser, revolute finger and fingertip joints; controlled via `libgripper.so` |

### Worlds

| Name | Description |
|------|-------------|
| `world2.world` | Udacity assignment world — `building1`, two `hoverboard` instances, `gripper_base`, and Mine Cart Engine from Gazebo Fuel |

### Plugins

| Name | Type | Description |
|------|------|-------------|
| `welcome` | World Plugin | Prints "Welcome to Filipe's World!!" on load |
| `hoverboard` | Model Plugin | Sets wheel joint velocities via SDF parameters at load time |
| `gripper` | Model Plugin | PID joint position control; accepts commands over a Gazebo Transport topic |

### Standalone command tools

Built as executables (not Gazebo plugins). Require a running Gazebo instance with the matching model loaded and `GAZEBO_PLUGIN_PATH` set. Run from the `build/` directory or add it to `PATH`.

**`vel_cmd`** — send a one-shot velocity command to a hoverboard instance:
```
vel_cmd <model_name> <linear_vel> <angular_vel>
```
```bash
./vel_cmd hoverboard0 1.0 0.0   # drive straight
./vel_cmd hoverboard0 0.0 1.0   # spin in place
```

**`gripper_cmd`** — send a one-shot position command to a gripper instance:
```
gripper_cmd <model_name> <riser_pos> <finger_pos> <tip_pos>
```
```bash
./gripper_cmd gripper 0.2 -0.3 -0.1   # raise riser, open fingers
```
Positions are in radians for revolute joints and metres for the prismatic riser joint. Left/right joints receive the same target (symmetric control).

**`gripper_pid_cfg`** — update PID gains on a running gripper instance:
```
gripper_pid_cfg <model_name> <joint_type> <Kp> <Ki> <Kd>
```
```bash
./gripper_pid_cfg gripper finger 4.0 2.5 1.0   # tune finger joints
./gripper_pid_cfg gripper tip    4.0 2.5 1.0   # tune tip joints
./gripper_pid_cfg gripper riser  4.0 2.5 1.0   # tune riser joint
./gripper_pid_cfg gripper all    4.0 2.5 1.0   # apply to all joints
```
Any `<joint_type>` value other than `finger`, `tip`, or `riser` applies the gains to all joints.

---

## Prerequisites

| Dependency | Notes |
|------------|-------|
| Gazebo Classic 11 | Tested on Ubuntu 20.04 (ROS1 Noetic). Earlier versions may work but are untested. |
| CMake ≥ 3.22 | Required by `CMakePresets.json` |
| `libprotobuf-dev` + `protobuf-compiler` | Required for the `gripper` plugin and the standalone command tools. Not needed for the `welcome` or `hoverboard` plugins. |

Install on Ubuntu 20.04:

```bash
sudo apt install gazebo11 libgazebo11-dev cmake libprotobuf-dev protobuf-compiler
```

---

## Setup

### 1. Clone and build plugins

```bash
git clone https://github.com/fvital/gazebo-robotics-lab.git
cd gazebo-robotics-lab
cmake --preset default
cmake --build --preset default
```

### 2. Add models and plugins to Gazebo's search paths

Source the included setup script (works from any directory):

```bash
source /absolute/path/to/gazebo-robotics-lab/setup.bash
```

To make this persistent, add that line — with the absolute path — to your `~/.bashrc`.

All plugin shared libraries — including `libcustom_msg.so` — are placed directly in `build/` by the CMake preset.

### 3. Launch a world

```bash
gazebo world/world2.world
```

---
