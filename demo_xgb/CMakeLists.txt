cmake_minimum_required(VERSION 3.16)
project(demo_xgb VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找已安装的 MuJoCo
find_package(mujoco REQUIRED
  PATHS "/home/qiyuan/Softwares/Mujoco330/install/lib/cmake/mujoco"
)

# 查找线程库
find_package(Threads REQUIRED)

# 查找 GLFW（MuJoCo 构建时已下载的 GLFW）
find_package(glfw3 REQUIRED
  PATHS "/home/qiyuan/Softwares/Mujoco330/build"
)

add_executable(demo_xgb demo_xgb.cc)
target_link_libraries(demo_xgb
  mujoco::mujoco
  glfw
  Threads::Threads
)
