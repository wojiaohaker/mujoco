cmake_minimum_required(VERSION 3.16)
project(mujoco_sim VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== 依赖 =====

# MuJoCo (从本地安装)
find_package(mujoco REQUIRED
  PATHS "/home/qiyuan/Softwares/Mujoco330/install/lib/cmake/mujoco"
)

# Protobuf
find_package(Protobuf REQUIRED)

# Threads
find_package(Threads REQUIRED)

# GLFW (可视化窗口)
find_package(glfw3 REQUIRED
  PATHS "/home/qiyuan/Softwares/Mujoco330/build"
)

# eCAL (系统安装, 无cmake配置, 手动链接)
# 头文件: /usr/include/ecal/
# 库: /lib/libecal_core.so, /lib/libecal_core_pb.so

# ROS 2 (可选, 发布 /odom/mujoco_odom)
# 编译前需 source /opt/ros/humble/setup.bash
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)

# ===== 目标 =====

add_executable(mujoco_sim
  main.cpp
  mujoco_sim.cpp
)

target_include_directories(mujoco_sim PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  /usr/include                # robot_sdk.pb.h
  /usr/include/ecal           # eCAL headers (通过 ecal/ecal.h 引用)
)

target_link_libraries(mujoco_sim PRIVATE
  mujoco::mujoco
  protobuf::libprotobuf
  ecal_core
  ecal_core_pb
  robot_sdk                   # /lib/librobot_sdk.so (protobuf 消息定义)
  glfw
  Threads::Threads
  rclcpp::rclcpp              # ROS 2
  ${nav_msgs_TARGETS}         # nav_msgs::msg::Odometry
)

# ===== 安装 =====
install(TARGETS mujoco_sim DESTINATION bin)
