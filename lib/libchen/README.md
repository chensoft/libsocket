# libchen

[![Build Status](https://img.shields.io/travis/chensoft/libchen.svg?label=Linux%20Mac)](https://travis-ci.org/chensoft/libchen)
[![Build status](https://img.shields.io/appveyor/ci/chensoft/libchen.svg?label=Windows)](https://ci.appveyor.com/project/chensoft/libchen)
[![release](http://github-release-version.herokuapp.com/github/chensoft/libchen/release.svg?style=flat&refresh=0)](https://github.com/chensoft/libchen/releases/latest)
[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/hyperium/hyper/master/LICENSE)

libchen is a general purpose C++ toolkit, it contains many useful and handy methods for creating complex software. It's cross-platform and released under the MIT License.

## Platforms

* OS X (clang++ 3.5+)
* Linux (g++ 4.8.4+, clang++ 3.5+)
* Windows (vs 2015+)
* Android (ndk r10+)
* iOS (xcode 7+)

Notice that C++11's regex is incomplete on g++ 4.8.x, if you want to use it, you shall upgrade your compiler to the latest version.

## Features

* base - basic part
    * str: string format, print, equality test, prefix & suffix, join, replace, trim and more
    * utf8: check utf-8 string character count, retrieve sub string, check bom
    * any: provide a wrapper to hold any type
    * num: float equality test, integer swap, number to string, random number generator
    * map: check key existence, retrieve all keys, all values, flip map's key and value
    * vector: make unique, combine, intersect and difference
    * regex: simple wrapper for regex match, replace and group
* data - data parse
	* json: json file & string parser
	* ini: ini file & string parser
* mt - multithreading
	* semaphore: C++11 semaphore support
	* threadpool: threadpool support, use std::future
* sys - system operation
	* fs: dirname, basename, realpath, file exist and many filesystem related methods
	* sys: uuid generator
	* proc: create daemon easily, get process pid and kill process
* time - time and date
	* time: simple timing class
	* date: thread-safe gmt & local time
* tool - misc tools
	* log: simple logging system
	* cmd: modern command line parser

## Compile

### Method 1 - Via Source

You can directly add the source code to your project, the `include` folder contains the headers, the `src` folder contains the source code. Don't add `sample ` and `test` folders, they are the test code.

Please note that the `src/plat` folder contains some platform dependent code, if you use Unix-like os, just add the `src/plat/unix` folder, exclude others. If you use Windows, add the `src/plat/win` folder only.

### Method 2 - Via Library

If you want to compile it to a library, the easiest way is using CMake:

```
git clone https://github.com/chensoft/libchen.git
cd libchen
mkdir build
cd build

cmake ..
make
```

If no errors, a static library named `libchen.a` is generated in the `build` folder.

The default static library has debug info, if you want to build a release library, simply change the build type and compile again:

```
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## License

libchen is released under the MIT License. See the LICENSE file for more information.

## Contact

You can contact me by email: admin@chensoft.com.

—————————(○^～^○)我只是一条分割线(○^～^○)—————————

这个项目是我平时的一些代码积累，你也可以认为是在重复造轮子，这样说也没错。我认为造轮子能让我对某个功能有更深入的了解，而不仅仅是停留在会用的层面上，而且如果哪天我做的东西能被更多人使用，何乐而不为？

这里也没有什么高深莫测的代码，一切以实用为主，我尽可能的保证代码的准确和高效，但没法保证一定会比别人写的更快更好。当然，做的更好是我永远追求的目标。

关于库的稳定性，在库的小版本升级时，比如从1.0.0到1.1.0，大部分都会是内部实现的变动，API接口可能会有很少量的修改。但如果是大版本变更，比如从1.0.0到2.0.0，那很可能变动的东西会比较多。

本人有强迫症，如果我哪天看某个类名或函数名不舒服，我很可能会换个更简短的名字，如果因为这个对你造成困扰，请谅解。另外一方面，对于一个已经存在的功能，我不会轻易删除，所以这方面大可放心。

最后我非常欢迎能得到你的反馈，有什么问题，不管是抱怨还是建议，都可以发送到我邮箱：admin@chensoft.com。