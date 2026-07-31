// XGB 四足机器人 MuJoCo 仿真 Demo
// 载入 xgb 模型并启动可视化窗口，支持鼠标交互和键盘控制

#include <cstdio>
#include <cstring>
#include <string>

#include <GLFW/glfw3.h>
#include <mujoco/mujoco.h>

// ==================== 全局变量 ====================
static mjModel* m = NULL;
static mjData* d = NULL;
static mjvCamera cam;
static mjvOption opt;
static mjvScene scn;
static mjrContext con;

static bool button_left = false;
static bool button_middle = false;
static bool button_right = false;
static double lastx = 0;
static double lasty = 0;

// ==================== 回调函数 ====================

// 键盘回调
void keyboard(GLFWwindow* window, int key, int scancode, int act, int mods) {
  if (act == GLFW_PRESS && key == GLFW_KEY_BACKSPACE) {
    mj_resetData(m, d);
    mj_forward(m, d);
  }
  if (act == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

// 鼠标按钮回调
void mouse_button(GLFWwindow* window, int button, int act, int mods) {
  button_left   = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)   == GLFW_PRESS);
  button_middle = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
  button_right  = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)  == GLFW_PRESS);
  glfwGetCursorPos(window, &lastx, &lasty);
}

// 鼠标移动回调
void mouse_move(GLFWwindow* window, double xpos, double ypos) {
  if (!button_left && !button_middle && !button_right) return;

  double dx = xpos - lastx;
  double dy = ypos - lasty;
  lastx = xpos;
  lasty = ypos;

  int width, height;
  glfwGetWindowSize(window, &width, &height);

  bool mod_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                    glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

  mjtMouse action;
  if (button_right) {
    action = mod_shift ? mjMOUSE_MOVE_H : mjMOUSE_MOVE_V;
  } else if (button_left) {
    action = mod_shift ? mjMOUSE_ROTATE_H : mjMOUSE_ROTATE_V;
  } else {
    action = mjMOUSE_ZOOM;
  }

  mjv_moveCamera(m, action, dx / height, dy / height, &scn, &cam);
}

// 滚轮回调
void scroll(GLFWwindow* window, double xoffset, double yoffset) {
  mjv_moveCamera(m, mjMOUSE_ZOOM, 0, -0.05 * yoffset, &scn, &cam);
}

// ==================== 主函数 ====================
int main(int argc, const char** argv) {
  // 默认模型路径：xgb scene.xml
  std::string model_path =
      "/home/qiyuan/Softwares/Matrix/src/robot_mujoco/zsibot_robots/xgb/scene.xml";

  // 也支持命令行参数传入
  if (argc == 2) {
    model_path = argv[1];
  }

  // 加载模型
  char error[1000] = "Could not load binary model";
  if (model_path.size() > 4 && model_path.substr(model_path.size() - 4) == ".mjb") {
    m = mj_loadModel(model_path.c_str(), 0);
  } else {
    m = mj_loadXML(model_path.c_str(), 0, error, 1000);
  }
  if (!m) {
    std::printf("ERROR: %s\n", error);
    return 1;
  }
  std::printf("[OK] 模型加载成功: %s\n", model_path.c_str());
  std::printf("     关节数量: %d\n", m->njnt);
  std::printf("     执行器数量: %d\n", m->nu);
  std::printf("     刚体数量: %d\n", m->nbody);

  // 创建仿真数据
  d = mj_makeData(m);

  // 初始化 GLFW
  if (!glfwInit()) {
    std::printf("ERROR: 无法初始化 GLFW\n");
    return 1;
  }

  // 创建窗口
  GLFWwindow* window = glfwCreateWindow(1200, 900, "XGB MuJoCo Demo", NULL, NULL);
  if (!window) {
    std::printf("ERROR: 无法创建 GLFW 窗口\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // 初始化可视化
  mjv_defaultCamera(&cam);
  mjv_defaultOption(&opt);
  mjv_defaultScene(&scn);
  mjr_defaultContext(&con);

  mjv_makeScene(m, &scn, 2000);
  mjr_makeContext(m, &con, mjFONTSCALE_150);

  // 设置初始相机位置（俯瞰机器人）
  cam.azimuth = 130;
  cam.elevation = -20;
  cam.distance = 1.5;
  cam.lookat[0] = 0;
  cam.lookat[1] = 0;
  cam.lookat[2] = 0.3;

  // 注册回调
  glfwSetKeyCallback(window, keyboard);
  glfwSetCursorPosCallback(window, mouse_move);
  glfwSetMouseButtonCallback(window, mouse_button);
  glfwSetScrollCallback(window, scroll);

  std::printf("\n[控制说明]\n");
  std::printf("  鼠标左键拖拽: 旋转视角\n");
  std::printf("  鼠标右键拖拽: 平移视角\n");
  std::printf("  滚轮:         缩放\n");
  std::printf("  Backspace:    重置仿真\n");
  std::printf("  ESC:          退出\n\n");

  // 主循环
  while (!glfwWindowShouldClose(window)) {
    // 推进仿真 1/60 秒
    mjtNum simstart = d->time;
    while (d->time - simstart < 1.0 / 60.0) {
      mj_step(m, d);
    }

    // 获取视口大小
    mjrRect viewport = {0, 0, 0, 0};
    glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

    // 更新场景并渲染
    mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
    mjr_render(viewport, &scn, &con);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // 清理
  mjv_freeScene(&scn);
  mjr_freeContext(&con);
  mj_deleteData(d);
  mj_deleteModel(m);

#if defined(__APPLE__) || defined(_WIN32)
  glfwTerminate();
#endif

  return 0;
}
