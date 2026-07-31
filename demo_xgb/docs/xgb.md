
Demo 已成功构建并运行！窗口已打开，xgb 四足机器人模型加载成功。

**创建的文件：**

| 文件 | 说明 |
|------|------|
| [demo_xgb.cc](file:///home/qiyuan/Softwares/Mujoco330/demo_xgb/demo_xgb.cc) | C++ 源代码，加载 xgb 模型并渲染 |
| [CMakeLists.txt](file:///home/qiyuan/Softwares/Mujoco330/demo_xgb/CMakeLists.txt) | CMake 构建配置 |
| [run.sh](file:///home/qiyuan/Softwares/Mujoco330/demo_xgb/run.sh) | 一键构建+运行脚本 |

**运行结果：**
```
[OK] 模型加载成功
     关节数量: 13
     执行器数量: 12
     刚体数量: 14
```

**操作方式：**
- 鼠标左键拖拽 → 旋转视角
- 鼠标右键拖拽 → 平移视角
- 滚轮 → 缩放
- Backspace → 重置仿真
- ESC → 退出

**后续运行：**
```bash
bash /home/qiyuan/Softwares/Mujoco330/demo_xgb/run.sh
```

可以！MuJoCo 3.3.0 有以下几种加载 XML 模型的方式：

---

### 方式一：直接拖拽（最简单）

**可以！** 打开 `simulate` 后直接把 `scene.xml` 拖进窗口即可：

```bash
/home/qiyuan/Softwares/Mujoco330/install/bin/simulate
```

然后把文件拖进去：
```
/home/qiyuan/Softwares/Matrix/src/robot_mujoco/zsibot_robots/xgb/scene.xml
```

---

### 方式二：命令行直接启动

```bash
/home/qiyuan/Softwares/Mujoco330/install/bin/simulate \
  /home/qiyuan/Softwares/Matrix/src/robot_mujoco/zsibot_robots/xgb/scene.xml
```



```
./build/bin/simulate /home/qiyuan/Softwares/Matrix/src/robot_mujoco/zsibot_robots/xgb/scene.xml
```



---

### 方式三：Python API（需安装 mujoco）

```bash
pip install mujoco
```

```python
import mujoco
import mujoco.viewer

model = mujoco.MjModel.from_xml_path(
    "/home/qiyuan/Softwares/Matrix/src/robot_mujoco/zsibot_robots/xgb/scene.xml"
)
data = mujoco.MjData(model)

with mujoco.viewer.launch_passive(model, data) as viewer:
    while viewer.is_running():
        mujoco.mj_step(model, data)
        viewer.sync()
```

---

### 方式四：C++ 程序（已创建的 demo）

```bash
bash /home/qiyuan/Softwares/Mujoco330/demo_xgb/run.sh
```

---

### 总结

| 方式 | 命令/操作 | 适用场景 |
|------|----------|---------|
| **拖拽** | 打开 simulate → 拖入 xml | 快速预览 |
| **命令行** | `simulate scene.xml` | 快速启动 |
| **Python** | `from_xml_path()` | 编程控制 |
| **C++** | `mj_loadXML()` | 高性能仿真 |

**推荐**：日常调试用 `simulate` 拖拽或命令行最快，编程控制用 Python。