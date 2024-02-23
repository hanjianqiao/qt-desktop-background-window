# qt-desktop-background-window 快速创建桌面背景应用

> 运行使用示例请移步[qt-desktop-background-window-sample](https://github.com/hanjianqiao/qt-desktop-background-window-sample)

## 使用方式

1. 在项目主路径下执行命令
   ```bash
   git submodule add https://github.com/hanjianqiao/qt-desktop-background-window.git submodules/qt-desktop-background-window
   ```
2. 修改CMakeLists.txt，添加下面指令
   ```
   # 假设你的目标名称为：yourTarget
   add_subdirectory(submodules/qt-desktop-background-window)
   target_include_directories(yourTarget PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/submodules")
   target_link_libraries(yourTarget PRIVATE qt-desktop-background-window)
   ```
3. 继承`h::FramelessWindow`，进行少量配置即可，参考代码如下
    ```c++
    #include <QApplication>
    #include <QStyle>
    #include "samplewindow.h"

    #include <qt-desktop-background-window/desktopbackgroundwindow.h>
    
    SampleWindow::SampleWindow()
        : FramelessWindow{}
    {
      // 初始化
      h::DesktopBackgroundWindowManager::initialize();
      auto dbwm = h::DesktopBackgroundWindowManager::singleton();
  
      // 支持多屏幕，给每个屏幕设置壁纸
      auto screens = dbwm->screens();
      for(const auto &screen : screens){
          QLabel *label = new QLabel(nullptr);
          label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
          // 将label设置为背景widget
          dbwm->setScreenBackgroundWidget(screen, label);
  
          // 1、 加载静态图片
          // label->setPixmap(QPixmap(":/images/robot.jpg").scaled(label->size(), Qt::IgnoreAspectRatio));
  
          // 2、 加载动图
          QMovie *movie = new QMovie(":/images/rainbowCat.gif");
          movie->setScaledSize(label->size());
          label->setMovie(movie);
          movie->start();
      }
    }
    ```
    
### 效果1：Gif图片作为背景
![Gif图片作为背景](https://github.com/hanjianqiao/qt-desktop-background-window/assets/7146341/84910471-e385-403b-83f8-9001550b7f60)

## 更多
本项目使用MIT协议，欢迎使用、提建议、参与贡献。

