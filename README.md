# Bluez5 Util

使用gio用C++编写的bluez5工具，用于在[joycon-tool](https://github.com/mumumusuc/joycon-toolkit)项目中提供Linux平台的蓝牙操作支持。

## 依赖&编译

1. Debian/Ubuntu :

    ```
    # install gio(?)
    > sudo apt-get install libglib2.0-dev

    # check libgio-2.0
    > pkg-config --libs gio-2.0
    
    -lgio-2.0 -lgobject-2.0 -lglib-2.0
    ```

2. 使用CMake编译    

    *默认使用clang++9，因为flutter在编译Linux项目时使用的是clang++*

    ```
    @ bluez5_util/
    > mkdir build && cd build
    > cmake ..
    > make
    ```
