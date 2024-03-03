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
3. 编写少量代码即可，核心参考代码如下
    ```c++
    // 初始化
    h::DesktopBackgroundWindowManager::initialize();
    auto dbwm = h::DesktopBackgroundWindowManager::singleton();

    // 支持多屏幕，给每个屏幕设置壁纸，当然，你也可以修改，给每个屏幕设置不一样的壁纸
    auto screens = dbwm->screens();
    for(const auto &screen : screens){
        /*
         * 当前版本，建议使用下面的顺序来创建。
         * 因为调用 void DesktopBackgroundWindowManager::setScreenBackgroundWidget(QScreen *toScreen, QWidget *w);
         * 这个函数之后，框架会把w指向的QWidget的size设置成与其相应的屏幕大小。
         * 这个时候向w内填充其他控件，能更好地匹配size
        */

        // 第一步，创建一个QWidget，是任意你想显示的内容
        QLabel *label = new QLabel(nullptr);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // 第二步，将其设置为背景widget
        dbwm->setScreenBackgroundWidget(screen, label);

        // 第三步，初始化你的背景widget
        QMovie *movie = new QMovie(":/images/rainbowCat.gif");
        movie->setScaledSize(label->size());
        label->setMovie(movie);
        movie->start();
    }
    ```
    
### 效果1：Gif图片作为背景
![Gif图片作为背景](https://github.com/hanjianqiao/qt-desktop-background-window/assets/7146341/84910471-e385-403b-83f8-9001550b7f60)

## 更多
本项目使用MIT协议，欢迎使用、提建议、参与贡献。

