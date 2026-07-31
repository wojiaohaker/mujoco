# MuJoCo 3.3.0 深度项目分析

## 一、项目概述

MuJoCo（Multi-Joint dynamics with Contact）是 Google DeepMind 开源的高性能物理引擎，专注于接触动力学仿真。版本 3.3.0，采用 Apache 2.0 许可证。

**核心能力：**
- 刚体/柔性体多体动力学仿真
- 精确接触力计算（凸优化求解器）
- 正/逆动力学、解析导数
- 肌腱/肌肉/执行器建模
- 传感器仿真
- 实时渲染与交互

**技术栈：** C11（引擎核心）+ C++17（插件/线程/用户层），CMake ≥ 3.16 构建，OpenGL 渲染。

---

## 二、源码架构

```
src/
├── engine/     # 核心仿真引擎（~45个文件，纯C为主）
├── user/       # 用户API层（模型加载/编译，C++）
├── xml/        # MJCF/URDF XML解析（C++）
├── render/     # OpenGL渲染（GL2/GL3后端）
├── thread/     # 线程池（C++）
├── ui/         # 原生UI控件
└── cc/         # 公共C++工具

plugin/         # 可加载插件（actuator/sensor/elasticity/sdf）
simulate/       # 交互式GUI模拟器
sample/         # 示例程序
python/         # Python绑定
mjx/            # JAX加速后端（MJX）
```

---

## 三、仿真管线（核心流程）

### 3.1 主步进函数 `mj_step()`

```
mj_step(m, d)
  ├── 状态检查（qpos/qvel有效性）
  ├── mj_forward(m, d)          // 计算所有动力学量
  └── 积分器选择：
      ├── Euler（半隐式，默认）
      ├── RK4（四阶Runge-Kutta）
      ├── Implicit（全隐式）
      └── ImplicitFast（快速隐式）
```

### 3.2 前向动力学 `mj_forward()` → `mj_forwardSkip()`

```
mj_forwardSkip(m, d, skipstage)
  ├── mj_fwdPosition()       // 位置相关计算
  │   ├── mj_kinematics()    // 前向运动学（体位置/姿态）
  │   ├── mj_comPos()        // 质心位置
  │   ├── mj_camlight()      // 相机/光源
  │   ├── mj_flex()          // 柔性体顶点
  │   ├── mj_tendon()        // 肌腱几何
  │   ├── mj_transmission()  // 传动
  │   ├── mj_crb()           // 复合刚体惯性
  │   ├── mj_factorM()       // 质量矩阵分解
  │   ├── mj_collision()     // 碰撞检测（可并行）
  │   ├── mj_makeConstraint()// 约束构建
  │   ├── mj_island()        // 约束岛检测
  │   └── mj_projectConstraint() // 约束投影
  │
  ├── mj_fwdVelocity()       // 速度相关计算
  │   ├── 柔性体/肌腱/执行器速度
  │   ├── mj_comVel()        // 质心速度
  │   ├── mj_passive()       // 被动力（弹簧/阻尼）
  │   ├── mj_referenceConstraint() // 参考约束
  │   └── mj_rne()           // 递推Newton-Euler（科氏力/重力）
  │
  ├── mj_fwdActuation()      // 执行器动力学
  │   ├── 执行器状态积分（integrator/filter/muscle/plugin）
  │   ├── gain/bias 计算
  │   └── 力矩映射 qfrc_actuator
  │
  ├── mj_fwdAcceleration()   // 加速度
  │   └── qacc = M⁻¹(qfrc_applied + qfrc_actuator + qfrc_passive - qfrc_bias)
  │
  └── mj_fwdConstraint()     // 约束求解
      ├── warmstart
      ├── 岛并行CG / 全局PGS/CG/Newton
      └── noslip 后处理
```

### 3.3 两阶段步进

```c
mj_step1(m, d);   // 执行 mj_forward()，不积分
// 用户在此设置 d->ctrl, d->qfrc_applied 等
mj_step2(m, d);   // 执行积分
```

### 3.4 约束求解器

| 求解器 | 特点 | 适用场景 |
|--------|------|----------|
| PGS | 逐坐标投影Gauss-Seidel | 简单场景、低精度需求 |
| CG | 共轭梯度（默认） | 通用场景 |
| Newton | 牛顿法（最精确） | 高精度需求 |

支持**约束岛**（Island）检测：通过 flood-fill 算法将独立约束子系统分离，多线程并行求解。

### 3.5 碰撞检测

碰撞函数表 `mjCOLLISIONFUNC[9][9]` 覆盖 9 种几何体两两组合：
- **原始体碰撞**：Plane-Sphere、Plane-Capsule、Sphere-Sphere 等解析解
- **GJK/EPA**：凸体通用碰撞（`engine_collision_gjk.c`）
- **libccd MPR**：备选凸体碰撞（`engine_collision_convex.c`）
- **SDF**：有符号距离场碰撞（`engine_collision_sdf.c`）
- **BVH 宽相**：层次包围盒加速

---

## 四、模块详解

### 4.1 引擎核心 `src/engine/`

| 文件 | 职责 |
|------|------|
| `engine_forward.c` | **主仿真管线**（mj_step/mj_forward/积分器） |
| `engine_solver.c` | 约束求解器（PGS/CG/Newton） |
| `engine_core_smooth.c` | 前向运动学、复合刚体、质量矩阵 |
| `engine_core_constraint.c` | 约束构建、arena内存、AVX优化 |
| `engine_collision_driver.c` | 碰撞调度（函数表） |
| `engine_collision_gjk.c` | GJK+EPA算法 |
| `engine_collision_primitive.c` | 原始体碰撞 |
| `engine_collision_convex.c` | 凸体碰撞（libccd） |
| `engine_collision_sdf.c` | SDF碰撞 |
| `engine_island.c` | 约束岛检测（flood fill） |
| `engine_passive.c` | 被动力（弹簧/阻尼/柔性体） |
| `engine_sensor.c` | 传感器计算（pos/vel/acc三阶段） |
| `engine_derivative.c` | 解析导数（空间代数） |
| `engine_derivative_fd.c` | 有限差分导数 |
| `engine_inverse.c` | 逆动力学 |
| `engine_io.c` | 模型/数据I/O（分配/保存/加载） |
| `engine_callback.c` | 全局回调指针 |
| `engine_plugin.cc` | 插件注册/加载（dlopen） |
| `engine_util_blas.c` | 线性代数（AVX优化） |
| `engine_util_sparse.c` | 稀疏矩阵运算 |
| `engine_util_spatial.c` | 四元数/空间代数 |
| `engine_util_solve.c` | Cholesky/LU分解 |
| `engine_util_misc.c` | 肌腱包裹/肌肉模型 |
| `engine_util_errmem.c` | 错误处理/对齐内存分配 |
| `engine_ray.c` | 射线检测 |
| `engine_vis_*.c` | 可视化（初始化/交互/状态/渲染） |
| `engine_setconst.c` | 模型常量设置 |
| `engine_name.c` | 名称↔ID查找 |
| `engine_print.c` | 模型/数据打印 |
| `engine_crossplatform.cc` | 跨平台（Rosetta检测/ASAN） |
| `engine_support.c` | 版本常量(mjVERSION=330)/字符串表 |

### 4.2 用户层 `src/user/`

- `user_api.cc`：用户API实现，全局模型缓存（500MB）
- `user_model.cc`：mjModel 编译
- `user_objects.cc`：对象管理
- `user_mesh.cc`：网格加载（OBJ格式）
- `user_flexcomp.cc`：柔性体复合体
- `user_composite.cc`：复合体
- `user_cache.cc`：资源缓存
- `user_resource.cc`：资源管理
- `user_vfs.c`：虚拟文件系统

### 4.3 XML解析 `src/xml/`

- `xml_native_reader.cc`：MJCF XML → mjSpec
- `xml_native_writer.cc`：mjSpec → MJCF XML
- `xml_urdf.cc`：URDF 支持
- `xml_base.cc`：XML基础设施
- `xml_util.cc`：数值格式化/解析工具

### 4.4 渲染 `src/render/`

- `render_context.c`：OpenGL上下文管理、位图字体
- `render_gl2.c` / `render_gl3.c`：GL2/GL3渲染后端
- `render_util.c`：渲染工具
- `render_debug.c`：调试渲染
- `glad/`：OpenGL函数加载器

### 4.5 线程 `src/thread/`

- `thread_pool.cc`：线程池（队列大小640，worker_id线程局部变量）
- `thread_task.cc`：任务调度
- `thread_queue.cc`：无锁队列

用于：惯性计算并行、碰撞检测并行、约束岛并行求解。

### 4.6 插件系统 `plugin/`

四种能力（可组合）：`ACTUATOR` | `SENSOR` | `PASSIVE` | `SDF`

| 插件 | 输出 | 功能 |
|------|------|------|
| `actuator/` | `libactuator.so` | PID控制器 |
| `sensor/` | `libsensor.so` | 自定义传感器、触摸网格 |
| `elasticity/` | `libelasticity.so` | 线缆、弹性体、壳体 |
| `sdf/` | `libsdf.so` | 螺栓、碗、齿轮、螺母、环面 |

**插件接口** (`mjplugin.h`)：
```c
struct mjPlugin_ {
  const char* name;
  int capabilityflags;  // mjPLUGIN_ACTUATOR | mjPLUGIN_SENSOR | ...
  // 生命周期回调
  int (*init)(const mjModel*, mjData*, int);
  void (*destroy)(mjData*);
  void (*reset)(const mjModel*, mjData*, int);
  // 能力回调
  void (*actuator_act_dot)(...);
  void (*sensor)(...);
  void (*passive)(...);
  void (*sdf_distance)(...);
  // ...
};
```

加载方式：`mjp_loadPluginLibrary(path)` 或 simulate 自动扫描 `MUJOCO_PLUGIN_DIR`。

---

## 五、构建系统

### 5.1 基本构建

```bash
# 配置
cmake -S . -B build

# 编译（所有核心）
cmake --build build -j$(nproc)

# 输出位置
# build/lib/libmujoco.so     - 核心共享库
# build/bin/simulate         - GUI模拟器
# build/bin/basic            - 基础示例
# build/bin/testspeed        - 性能测试
# build/bin/compile          - 模型转换
# build/bin/record           - 录制
```

### 5.2 构建选项

| 选项 | 默认 | 说明 |
|------|------|------|
| `MUJOCO_BUILD_EXAMPLES` | ON | 构建示例程序 |
| `MUJOCO_BUILD_SIMULATE` | ON | 构建GUI模拟器 |
| `MUJOCO_BUILD_TESTS` | ON | 构建测试 |
| `MUJOCO_TEST_PYTHON_UTIL` | OFF | Python测试工具 |
| `MUJOCO_SIMULATE_USE_SYSTEM_GLFW` | OFF | 使用系统GLFW |
| `MUJOCO_SIMULATE_USE_SYSTEM_MUJOCO` | OFF | 使用系统MuJoCo |
| `MUJOCO_HARDEN` | OFF | 安全加固（栈保护等） |
| `MUJOCO_ENABLE_AVX` | ON | AVX指令集优化 |
| `MUJOCO_ENABLE_AVX_INTRINSICS` | ON | AVX内联函数 |
| `MUJOCO_ENABLE_LTO` | ON(非Debug) | 链接时优化 |

### 5.3 编译标准与优化

- C标准：C11，C++标准：C++17
- 默认构建类型：Release
- 符号可见性：隐藏（`-fvisibility=hidden`）
- 警告：`-Werror`（警告即错误）
- OpenGL：GLVND
- 所有第三方依赖编译为**静态库**链接进 `libmujoco.so`

### 5.4 第三方依赖（FetchContent自动下载）

| 依赖 | 用途 |
|------|------|
| libccd | 凸体碰撞检测（MPR算法） |
| qhull | 凸包计算 |
| tinyxml2 | XML解析 |
| tinyobjloader | OBJ网格加载 |
| lodepng | PNG图像 |
| SdfLib | SDF计算 |
| MarchingCubeCpp | 等值面提取 |
| GLFW | 窗口管理（simulate/sample） |
| GoogleTest v1.16 | 测试框架 |
| Abseil | 测试辅助 |
| Google Benchmark | 性能基准 |
| Eigen3 | 测试中矩阵验证 |

### 5.5 仅构建核心库（最小构建）

```bash
cmake -S . -B build \
  -DMUJOCO_BUILD_EXAMPLES=OFF \
  -DMUJOCO_BUILD_SIMULATE=OFF \
  -DMUJOCO_BUILD_TESTS=OFF
cmake --build build -j$(nproc)
```

### 5.6 安装

```bash
cmake --install build --prefix /usr/local
# 安装：lib/libmujoco.so, include/mujoco/*.h, lib/cmake/mujoco/
```

---

## 六、运行指南

### 6.1 交互式模拟器（simulate）

```bash
./build/bin/simulate                    # 启动GUI，通过菜单加载模型
./build/bin/simulate model/humanoid/humanoid.xml  # 直接加载模型
```

功能：实时仿真、暂停/步进、参数调节、传感器可视化、性能profiler。

### 6.2 示例程序

```bash
# 基础渲染循环（GLFW窗口，60fps）
./build/bin/basic model/humanoid/humanoid.xml

# 性能测试（多线程rollout）
./build/bin/testspeed model/humanoid/humanoid.xml
./build/bin/testspeed --nthread=4 model/humanoid/humanoid.xml

# 模型格式转换
./build/bin/compile model.xml model.mjb    # XML → 二进制
./build/bin/compile model.xml model.txt    # XML → 文本

# 离屏录制（支持EGL/OSMesa/GLFW）
./build/bin/record model.xml output.mp4 60
```

### 6.3 无头仿真（Headless）

```bash
# 使用EGL（NVIDIA GPU）
MUJOCO_GL=egl ./build/bin/testspeed model.xml

# 使用OSMesa（软件渲染）
MUJOCO_GL=osmesa ./build/bin/record model.xml out.mp4 60
```

---

## 七、开发指南

### 7.1 基本API使用

```c
#include <mujoco/mujoco.h>

// 加载模型
char error[1000];
mjModel* m = mj_loadXML("model.xml", NULL, error, sizeof(error));
mjData* d = mj_makeData(m);

// 仿真循环
while (!done) {
  mj_step(m, d);
  // 读取状态：d->qpos, d->qvel, d->sensordata ...
}

// 清理
mj_deleteData(d);
mj_deleteModel(m);
```

### 7.2 回调机制

全局回调指针（`engine_callback.c`）：

```c
mjcb_passive      // 自定义被动力
mjcb_control      // 控制回调（每步调用）
mjcb_contactfilter// 接触过滤
mjcb_sensor       // 自定义传感器
mjcb_time         // 时间回调
mjcb_act_dyn      // 执行器动力学
mjcb_act_gain     // 执行器增益
mjcb_act_bias     // 执行器偏置
```

### 7.3 插件开发流程

1. 实现 `mjPlugin` 结构体（定义能力回调）
2. 导出 `mjp_plugin_init` 函数注册插件
3. 编译为共享库（`.so`）
4. 加载：`mjp_loadPluginLibrary("libmyplugin.so")` 或放入 `MUJOCO_PLUGIN_DIR`
5. XML中引用：`<plugin plugin="my_plugin">`

参考实现：`plugin/actuator/pid.cc`（最简示例）

### 7.4 外部项目集成（CMake）

```cmake
find_package(mujoco REQUIRED)
target_link_libraries(myapp PRIVATE mujoco::mujoco)
```

### 7.5 关键数据结构

- **mjModel**（`mjmodel.h`）：模型常量（拓扑、几何、物理参数），只读
- **mjData**（`mjdata.h`）：仿真状态（qpos/qvel/qacc/ctrl/力/接触），每线程一份
- **mjSpec**（`mjspec.h`）：模型规格（编辑态，可修改后编译为mjModel）
- **mjvScene**（`mjvisualize.h`）：可视化场景描述
- **mjrContext**（`mjrender.h`）：渲染上下文

### 7.6 线程安全

- `mjModel` 只读，可多线程共享
- `mjData` 非线程安全，每线程需独立实例
- 多线程rollout模式：每线程一个 `mjData`，共享 `mjModel`
- 内部线程池仅用于单步内的并行计算（碰撞/岛求解）

---

## 八、测试体系

### 8.1 框架

- GoogleTest v1.16 + Google Benchmark
- 测试宏：`mujoco_test` (CMake函数)
- Fixture：`test/fixture.h`（MujocoTest基类）

### 8.2 测试目录

```
test/
├── engine/      # 引擎单元测试（31个文件）
├── user/        # 用户API测试
├── xml/         # XML解析测试
├── plugin/      # 插件测试
├── sample/      # 示例程序测试
├── benchmark/   # 性能基准
└── thread/      # 线程测试
```

### 8.3 运行测试

```bash
cmake -S . -B build -DMUJOCO_BUILD_TESTS=ON
cmake --build build -j$(nproc)
ctest --test-dir build              # 全部测试
ctest --test-dir build -R engine    # 仅引擎测试
```

---

## 九、Python绑定

### 9.1 安装

```bash
cd python
pip install -e .
# 或从源码构建wheel
pip wheel . --no-build-isolation
```

依赖：numpy, glfw, pyopengl, absl-py, etils

### 9.2 基本使用

```python
import mujoco
import numpy as np

model = mujoco.MjModel.from_xml_path("model.xml")
data = mujoco.MjData(model)

while data.time < 10:
    mujoco.mj_step(model, data)
    print(data.qpos)
```

### 9.3 MJX（JAX加速）

```bash
cd mjx
pip install -e .
```

MJX 将 MuJoCo 仿真管线移植到 JAX，支持 GPU/TPU 批量并行仿真，适用于强化学习训练。

---

## 十、性能优化要点

1. **AVX优化**：稀疏矩阵点积（`dotSparseX3`）、约束投影使用AVX内联
2. **约束岛并行**：独立子系统多线程CG求解
3. **线程池**：碰撞检测和惯性计算并行（`nthread` 参数控制）
4. **Arena内存**：约束数据使用arena分配器，避免频繁malloc
5. **LTO**：Release模式默认开启链接时优化
6. **Warmstart**：约束求解器使用前一步结果作为初始猜测
7. **质量矩阵分解缓存**：`mj_factorM()` 结果在 `skipstage` 机制下复用

---

## 十一、开发注意事项

1. **C11兼容**：引擎核心为纯C，不使用C++特性
2. **符号隐藏**：所有内部函数默认隐藏，仅 `mj`/`mju`/`mjv`/`mjr` 前缀公共API导出
3. **内存对齐**：使用 `aligned_alloc`（C11）/ `_aligned_malloc`（Windows）
4. **错误处理**：`mju_error()` 默认打印并abort，可通过 `mju_user_error` 回调覆盖
5. **模型不可变**：`mjModel` 创建后不应修改（除 `opt` 部分参数）
6. **平台限制**：macOS Apple Silicon 不支持 Rosetta 2 翻译运行
7. **插件目录**：simulate 启动时自动扫描 `MUJOCO_PLUGIN_DIR` 环境变量指定的目录
8. **VFS**：虚拟文件系统用于从内存加载资源（纹理/网格），避免磁盘I/O
9. **全局缓存**：用户层维护 500MB 模型缓存（`user_api.cc`）
10. **版本常量**：`mjVERSION = 330`（`engine_support.c`），用于二进制兼容性检查
