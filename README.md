# 选题

## 项目需求

本项目旨在复刻游戏[星露谷物语](https://xinglugu.huijiwiki.com/wiki/星露谷物语)的迷你游戏《草原之王的冒险之旅》（[Journey of the Prairie King](http://stardewvalleywiki.com/Journey_of_the_Prairie_King)）。

**Journey of the Prairie King** 是一款自上至下的射击游戏，类似于 ["双人手柄"](https://en.wikipedia.org/wiki/Shoot_'em_up#Types|) 射击游戏，例如 [Smash TV](https://en.wikipedia.org/wiki/Smash_TV).你可以使用WASD移动你的角色并且使用↑←↓→射击。 玩家可以通过同时按住两个按键来实现斜线移动或开火 （e.g.同时按住 W 和 A 向左上方移动）。

你出发的时候拥有四点生命值（在屏幕左上角显示为 "x3" ），并且如果你的角色接触到了敌人或者弹射物，他将瞬间被杀死。失去你的最后一点生命值（当数值改变为 "x0" 以及 "x-1"）会导致游戏的终结。在每一个级别中，当成群结队的敌人从屏幕边缘出现向你移动时，屏幕上方的计时条也会开始倒数。 当计时条的倒数彻底数完后，敌人将停止生成，所有敌人被击败后，当前关卡结束，你将被允许进入下一个地区。

敌人被杀死时有概率掉落硬币和额外生命值以及[力量提升道具](https://xinglugu.huijiwiki.com/wiki/草原之王的冒险之旅#Powerups)，它们可以使你在一段时间内拥有一种升级效果。拾取到的道具显示在屏幕左上角的一个槽里，并且你可以按空格在任何时候激活。如果你已有一个道具在道具栏中，那么拾取到的新道具将会被直接消耗。升级效果只会持续一小段时间。

每个阶段都会以一场头领战结束, 每个头领都拥有大量的生命值（HP）并且它们会向你发射子弹.

## 技术开发规划

1. **核心技术栈：**
   - **开发语言：** C++ (标准版本：C++17)
   - **应用框架：** Qt 框架 (版本：Qt6)
   - **构建系统：** CMake
   - **架构模式：** MVVM (Model-View-ViewModel) - 本项目将重点放在 View 和 ViewModel 层的交互，核心业务逻辑直接集成于 ViewModel 中。
2. **开发阶段：**
   - **中期目标 (完成核心玩法循环)：**
     - 实现玩家角色基础控制：移动 (WASD)、射击 (↑←↓→)。
     - 实现核心战斗机制：敌人生成、基础 AI、碰撞检测、生命值系统。
     - 实现道具系统：道具拾取与使用逻辑。
     - 实现游戏音乐和音效的播放
     - 构建基础游戏场景视图。
     - 完成核心游戏循环：游戏开始、进行中、失败/胜利状态。
   - **期末目标 (完善游戏内容与系统)：**
     - 实现多地图系统：复刻原版游戏的多个可游玩场景及地图内障碍生成。
     - 实现商店系统：经济系统、升级界面与功能。
     - 完善怪物AI：使用A*算法作为怪物的寻路逻辑。
     - 进行游戏性平衡调整与基础性能优化。
     - 实现最终 Boss 战：为Boss开发独立的AI。

## 工具选择

1. **开发环境：**

   - **IDE：** Visual Studio Code (搭配 C++ 扩展包)
   - **理由：** VSCode 提供强大的 C++ 支持、调试和 Qt 集成能力。跨平台兼容性好。
2. **版本控制与协作：**

   - **系统：** Git
   - **托管平台：** GitHub (项目仓库：https://github.com/Nek0Charm/Journey-of-the-Prairie-King/)
   - **理由：** Git 是行业标准分布式版本控制系统；GitHub 提供完善的代码托管、协作 (Issue, Pull Request)、项目管理 (Projects, Wiki) 功能。
3. **构建与依赖管理：**

   - **构建系统：** CMake
   - **依赖管理：** vcpkg (优先) 或 手动管理第三方库
   - **理由：** CMake 提供跨平台、灵活的构建配置。vcpkg 简化 Qt 及其他 C++ 库的获取、编译和集成；手动管理作为备选方案。
4. **资源管理：**
   - **图形资源来源：** 主要引用自 Spriter's Resource 的 Stardew Valley 素材库 (https://www.spriters-resource.com/pc_computer/stardewvalley/sheet/82481/)。
   - **音效资源来源：** 主要引用自 Spriter's Resource 的 Stardew Valley 素材库 (https://www.sounds-resource.com/pc_computer/stardewvalley/)
   - **音乐资源来源：** 主要引用自 Youtube-Lewie G(播放列表：https://www.youtube.com/playlist?list=PLKDOdCjxOjzIFucHobwJpSK4-vAVXST90)

