#!/usr/bin/env bash
# Adds this repo's model and plugin directories to Gazebo's search paths.
# Source from anywhere using the absolute path:
#   source /path/to/gazebo-robotics-lab/setup.bash
# Or add that line to ~/.bashrc for a persistent setup.

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:${REPO_DIR}/model
export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:${REPO_DIR}/build