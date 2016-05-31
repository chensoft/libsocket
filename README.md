# libchen

[![Build Status](https://travis-ci.org/chensoft/libchen.svg?branch=master)](https://travis-ci.org/chensoft/libchen)
[![release](http://github-release-version.herokuapp.com/github/chensoft/libchen/release.svg?style=flat)](https://github.com/chensoft/libchen/releases/latest)
[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/hyperium/hyper/master/LICENSE)

libchen is a micro C++ foundation framework, it contains many useful and handy methods for creating complex software. It's open-source and released under the MIT License. If you have any questions or advices, feel free to contact me via [email](mailto:admin@chensoft.com).

## Quickstart

libchen is heavily depends on C++11, make sure your compiler have fully support for C++11, see the following `Requirements` part if you have any doubt.

The easiest way to build libchen is using CMake:

```
git clone https://github.com/chensoft/libchen.git
cd libchen
mkdir build
cd build

cmake ..
make
```

If no errors, a static library named `libchen.a` is generated in the build folder, good luck.

The default static library has debug info, if you want to build a release library, simply change the build type and compile again:

```
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Requirements

The library is known to work under the following environments:

* GCC 5.x (it has fully support for C++11)
* Clang (bundled with Xcode 7.x)

You may need to install Xcode command line tools if you use OS X:

```
xcode-select --install
```

My personal development environment is:

```
// OS
* OS X EI Capitan 10.11.5
* Ubuntu 16.04 LTS

// IDE
* CLion 2016.1
* Xcode 7.3.1
```

## Features

* string, map, vector algorithms
* utf-8 string support
* json string and file parser
* ini configuration parser
* semaphore, threadpool support
* filesystem handy methods
* thread-safe logging system
* a modern command line parser

## License

libchen is released under the MIT License. See the LICENSE file for more information.

## Contact

You can contact me by email: admin@chensoft.com.

## TODO

* Add support for Windows
* Write documentation

—————————(○^～^○)我只是一条分割线(○^～^○)—————————

这个项目是我平时的一些代码积累，你也可以认为是在重复造轮子，这样说也没错。我认为造轮子能让我对某个功能有更深入的了解，而不仅仅是停留在会用的层面上，而且如果哪天我做的东西能被更多人使用，何乐而不为？

这里也没有什么高深莫测的代码，一切以实用为主，我尽可能的保证代码的准确和高效，但没法保证一定会比别人写的更快更好。当然，做的更好是我永远追求的目标。

关于库的稳定性，在库的小版本升级时，比如从1.0.0到1.1.0，大部分都会是内部实现的变动，API接口可能会有很少量的修改。但如果是大版本变更，比如从1.0.0到2.0.0，那很可能变动的东西会比较多。

本人有强迫症，如果我哪天看某个类名或函数名不舒服，我很可能会换个更简短的名字，如果因为这个对你造成困扰，请谅解。另外一方面，对于一个已经存在的功能，我不会轻易删除，所以这方面大可放心。

最后我非常欢迎能得到你的反馈，有什么问题，不管是抱怨还是建议，都可以发送到我邮箱：admin@chensoft.com。