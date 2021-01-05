# sample-multi-platform

## THIS REPO IS UNDER CONSTRUCTION

The goal of this repository is to demonstrate how a cmake project can create multiple executables for different platforms, and test the consistency of the code generated for each platform.

## Table of Contents

- [sample-multi-platform](#sample-multi-platform)
  - [THIS REPO IS UNDER CONSTRUCTION](#this-repo-is-under-construction)
  - [Table of Contents](#table-of-contents)
  - [Development Machine Requirements](#development-machine-requirements)
  - [Building the Repo](#building-the-repo)
  - [Introduction](#introduction)
  - [Scenario](#scenario)
    - [Hardware](#hardware)
  - [PAL [Platform Abstraction Layer]](#pal-platform-abstraction-layer)
    - [PAL Interface](#pal-interface)
    - [PAL Implementation](#pal-implementation)
    - [Creating the PAL Libraries](#creating-the-pal-libraries)
  - [Using the PAL](#using-the-pal)
    - [Creating the Device](#creating-the-device)
    - [The Contract](#the-contract)
  - [Conclusion](#conclusion)

## Development Machine Requirements

This repo is built using cmake. The minimum requirement is version 3.10. You can download and install the latest cmake version [here](https://cmake.org/). After install make sure that you added the cmake path on the Environment Variable _Path_. To test it, open a command prompt and type:

```bash
cmake --version
```

It should return the current cmake version.

## Building the Repo

1. Clone the repo into the directory of your choosing with the following command

    ```bash
    git clone https://github.com/mamokarz/sample-multi-platform.git
    ```

2. Go inside of the directory sample-multi-platform and create a new directory

    ```bash
    cd sample-multi-platform
    mkdir cmake-build
    cd cmake-build
    ```

3. From here you can build the cmake project.

    ```bash
    cmake ..
    ```

4. Now that you have the project built, you can build the code.

    ```bash
    cmake --build .
    ```

5. It generates a few executables, which you can directly execute. For example:

    - Linux

        ```bash
        ./device/device_pal_config_1
        ```

    - Windows

        ```bash
        device/Debug/device_pal_config_1.exe
        ```

    The result shall looks like

    ```bash
    This is the real pal_temperature_get for device A
    My current PAL version is 0.1.0
    My current temperature is 25.0c
    This is the real pal_temperature_get for device A
    This is the real pal_temperature_get for device A
    This is the real pal_temperature_get for device A
    This is the real pal_led_set_color
    LED: GREEN
    This is the real pal_fan_set_speed A
    Set speed to 250
    This is the real pal_temperature_get for device A
    This is the real pal_temperature_get for device A
    ...
    ```

## Introduction

In the IoT world, the ability to have code that can target multiple hardware, with different configurations and different OS [Operating System] is crucial. For example, a refrigerator may cost from hundreds to thousands of dollars, it may contain a 20 inches display, a small LCD display or no display at all, and it may use a full-blown CPU running a flavor of Linux or a small MCU running an RTOS. No matter these differences, some code may be reused on all of these devices, for example, the main business logic or the code that connects those devices to the cloud.

The goal of this sample is to show how a single cmake project can create multiple libraries with different configurations, test it against a set of compliance tests, and generate multiple executables, each of them targeting a different platform.

## Scenario

To demonstrate the usage of cmake targeting multiple platforms, let’s suppose the following scenario:

A system shall read a temperature sensor every second and change an LED color and a fan speed, depending on the current temperature. The LED shall be green if the temperature is between -10c and 80c, yellow if between -38c and -10c or 60c to 80c, or red if the temperature is lower than -38c or higher than 80c. If the temperature bypasses 120c or the device cannot read the temperature, the system shall turn off the fan, change the LED to red, and halt itself to avoid further damages. The fan speed shall be set as 10 times the temperature in celsius, so, it shall be capable to run on both direction, with speed between 1 RPM to 1200 RPM clockwise and 1 RPM to 400 RPM anticlockwise.

### Hardware

The system can be deployed using three different boards that uses a CPU that can run Linux or Windows. The code use an OS dependent API to sleep for 1 second, it will use `sleep()`, defined in _windows.h_, for Windows, and `nanosleep()`, defined in _pthread.h_, for Linux.

The main differences between these boards are the fan and the temperature sensor, the first board contains the temperature sensor type `A` and the fan `A`. The second board contains the temperature sensor type `B` and the fan `B`. The third board contains the temperature sensor type `B` and the fan `C`. The APIs for sensors and fans are different, so an adapter layer is required.

All boards may use both OSs, which means that each OS shall create 3 different executables, one for each board. So, in total, the cmake project shall be able to create 6 different executables.

## PAL [Platform Abstraction Layer]

Because software and hardware manufacturers normally define their own APIs (for example `sleep()` and `nanosleep()`), it becomes necessary to have an adapter layer that will unify all APIs in a single set that our business logic can understand.

On this example, we called the adapter layer PAL or Platform Abstraction Layer. There are some other similar names like HAL [Hardware Abstraction Layer] or BSP [Board Support Package] and while they are not exactly the same, there is a big overlap between them. So, for simplicity, we just call it PAL.

### PAL Interface

As mentioned, no matter the target platform, **the PAL shall always expose the same interface and the same behavior**. Doing that, the other parts of the system can be totally agnostic. In this sample, the interface was defined in `pal.h`, and exposes the following APIs.

```c
const char* pal_version_get_string(void);
const int pal_version_get_major(void);
const int pal_version_get_minor(void);
const int pal_version_get_patch(void);

pal_result pal_temperature_get(unsigned short* temperature);
float pal_temperature_to_celsius(unsigned short temperature);

pal_result pal_led_set_color(pal_led_color color);
pal_result pal_fan_set_speed(int speed_rpm);

void pal_halt(void);
void pal_sleep(unsigned int sleep_time_ms);
```

The first four APIs are related to the version which we will explain later. After that, two APIs, `pal_temperature_get()` and `pal_temperature_to_celsius()`, expose the temperature sensor on the board. The `pal_led_set_color()` exposes the LED color and the `pal_fan_set_speed` the fan speed on the board. Those are the board dependent APIs.

The `pal_halt()` exposes a way to completely stop the system. In our implementation, it is a simple `while(true){}` that will run forever. However, it shall be part of the PAL because it may not be the best way to halt in other systems. For instance, if a new device will run on battery, this busy loop is undesirable.

At the end, we have the `pal_sleep()` that is the OS dependent API.

### PAL Implementation

There are multiple `.c` files with the implementation, each of them related to one part of the implementation for a specific platform.

It is important to preserve uniqueness of identifiers in the _translation-units_ (very complicated way to say that we cannot have 2 functions with the same name in the same `.c`). However, there is no limitation about `.c` files that implement the same function. They are all fine since the linker can understand which one it should use when mounting the code.

#### Board dependencies

To better understand it, let's see the two implementations of the temperature sensor. Files _pal\_temperature\_a.c_ and _pal\_temperature\_b.c_ both implement the function `pal_temperature_get()`, and you can compile both.

From the repo root, create a directory called build and switch inside of it.

```bash
mkdir build
cd build
```

Compile both temperature sensor implementation

- Linux

    ```bash
    gcc -c ../pal/pal_temperature_a.c
    gcc -c ../pal/pal_temperature_b.c
    ```

- Windows

    ```bash
    cl /c ../pal/pal_temperature_a.c
    cl /c ../pal/pal_temperature_b.c
    ```

It will create one object file for each sensor. If you are sharing the same directories between your Linux and Windows machines, it is even fine to build for both platforms in the same "_build_" directory, creating 2 `.o`, object file for Linux, and 2 `.obj`, object file for Windows. In this case, it will result in something like:

```bash
./
../
a/
pal_temperature_a.o
pal_temperature_a.obj
pal_temperature_b.o
pal_temperature_b.obj
```

To examine the content of the object file, use in Linux the command **nm** (similar information can be extracted on Windows using _dumpbin.exe_)

```bash
nm pal_temperature_a.o
```

The result shall be something like

```bash
                 U _GLOBAL_OFFSET_TABLE_
0000000000000000 T pal_temperature_get
000000000000006a T pal_temperature_to_celsius
                 U puts
```

If you are not familiar with the `nm` command, the `T` represents the symbols of the code in the _.text_ area in the object file, or the symbols exported by the object file, and the `U` represents undefined symbols that are expected by the object.

It means that the object _pal\_temperature\_a.o_ exports the identifiers `pal_temperature_get`, `pal_temperature_to_celsius` and requests the implementation of `puts`. A similar result shall be obtained if you call `nm` for _pal\_temperature\_b.o_.

#### OS dependencies

For the parts of the PAL that are OS-dependent, the cmake shall be smart enough to decide which one to build. It is important because the `.c` will have dependencies of the OS libraries, and try to compile it. If the wrong platform is selected, it will result in a "No such file or directory" error.

To make it clear, look in the _pal\_thread\_pthread.c_. It includes _pthread.h_ that is not part of the Windows libraries; similarly, the _pal\_thread\_windows.c_ includes _windows.h_ that does not exist in Linux. So, building _pal\_thread\_pthread.c_ on a Linux machine

```bash
gcc -c ../pal/pal_thread_pthread.c
```

shall create "pal_thread_pthread.o". However, execute the similar command on Linux for _pal\_thread\_windows.c_

```bash
gcc -c ../pal/pal_thread_windows.c
```

shall result in error.

```bash
../pal/pal_thread_windows.c:1:10: fatal error: windows.h: No such file or directory
    1 | #include <windows.h>
      |          ^~~~~~~~~~~
compilation terminated.
```

Examining the object file created from the compilation of _pal\_thread\_pthread.c_.

```bash
nm pal_thread_pthread.o
```

shall result in,

```bash
                 U _GLOBAL_OFFSET_TABLE_
                 U __stack_chk_fail
                 U nanosleep
0000000000000000 T pal_sleep
```

As expected, _pal\_thread\_pthread.o_ exports `pal_sleep` and requests an implementation of `nanosleep`, that shall be provided by the pthread library.

#### Other Dependencies

The system may have other dependencies, for example the way that it halt in case of a critical error. It is better to keep it in the PAL layer, even if the current implementation work for all platforms, it will facilitate change if a new platform requires a different implementation in the future. Better safe than sorry.

### Creating the PAL Libraries

Up to now, we build our `.c` files directly using _gcc_ or _cl_, and we didn't link it to create a library or a executable. Now, let's move to cmake and create a project that will compile the `.c` files and link the object files creating a static library. CMake uses `CMakeLists.txt` to define the project. The PAL `CMakeLists.txt` shall create 3 libraries, one for each board, and it shall choose the right thread library to compile and link, depending on the OS.

CMake is a very powerful tool. For this sample, we will use only the ability to set variables, create projects, and do some glue logic.

#### Defining Libraries Using config.cmake

The set of `.c` files that will compose each library shall be enough to cover all APIs exposed by the header `pal.h`. In our case, it needs one adapter for the temperature sensor, fan, led, exception, and thread. The definition of these configurations are in the `config.cmake`. A `.cmake` file is a file that can be included in any `CMakeLists.txt` to export variables or functions. In this sample, the `config.cmake` exposes the list of configurations and the functions to create samples and tests.

Observe that we defined all possible configurations in the `config.cmake`, independent of the OS. As a convention, we define configurations `pal_win_x` for windows, and `pal_linux_x` for windows.

```cmake
list(APPEND pal_win_1
    pal_version.c
    ...
)

list(APPEND pal_win_2
    pal_version.c
    ...
)

...

list(APPEND pal_linux_1
    pal_version.c
    ...
)
```

To define which configurations are valid for each OS, we created a list called `pal_libraries` that contains all libraries that the cmake shall create and test.

CMake has a few build variables (a full documentation can be find at [https://cmake.org/documentation/](https://cmake.org/documentation/)). For the purpose of this sample, we will test if the build variable `WIN32` was defined or not (as you can presume, this variable only exists if you are running the cmake on a Windows machine).

```cmake
if(WIN32)
    list(APPEND pal_libraries 
        pal_win_1 
        pal_win_2
        ...
    )
else()
    list(APPEND pal_libraries 
        pal_linux_1 
        pal_linux_2
        ...
    )
endif()
```

So if `WIN32` is defined, `pal_libraries` will contain _pal_win_1_, _pal_win_2_, and _pal_win_3_, if not, this variable will contain _pal_linux_1_, _pal_linux_2_, and _pal_linux_3_. Other configurations may be necessary, for example, if you are using Clang as your compiler, you will need to make a more complex decision, because Clang uses pthread library on both Linux and Windows. If it is the case, the reliable way is to use the cmake variable `CMAKE_C_COMPILER_ID`.

#### Including `config.cmake`

For this sample, we decide to include the `config.cmake` in the root of the cmake project, to do that, we need to add in the `CMAKE_MODULE_PATH` a path to the directory where we stored our own `.cmake` files, and after that we just need to include the _config_ one.

```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(config)
```

#### CMake hierarchy

CMake works with hierarchy of directories, so, build a `CMakeLists.txt` will include all `CMakeLists.txt` included by **add_subdirectory**. The `CMakeLists.txt` in the root include the PAL, monitor, tests and samples.

```cmake
add_subdirectory(pal)
add_subdirectory(monitor)
add_subdirectory(tests)
add_subdirectory(device)
```

#### Building libraries

The `CMakeLists.txt` in the pal directory is responsible to build all the libraries in the `pal_libraries` list, it is done by interacting over the list and calling the cmake command **add_library**.

```cmake
foreach(pal IN LISTS pal_libraries)
    message(STATUS "creating library ${pal}[${${pal}}]")
    add_library(${pal}
        ${${pal}}
    target_compile_definitions(${pal} PUBLIC 
        "PAL_VERSION_MAJOR=${PAL_VERSION_MAJOR}"
        "PAL_VERSION_MINOR=${PAL_VERSION_MINOR}"
        "PAL_VERSION_PATCH=${PAL_VERSION_PATCH}")
    )
endforeach()
```

The **add_library** instruct cmake to create a new project for a library with the name defined by the first argument and including all the files defined by the subsequent arguments. Because `pal_libraries` is a list of lists, each iteration of the `foreach` will return one list, that contains the configuration name, defined by `${pal}`, with all files that compose it, defined by `${${pal}}`. The **target_compile_definitions** bypass the PAL version to the code.

To make the sample cleaner, let's isolate the Linux and Windows build directories. To do that, let's create a work directory inside the repo with a name related to the OS, for example "cmake-linux" and "cmake-win". Note that anything started with `cmake-` will be ignored by GIT in our repo (it is in _.gitignore_).

Go inside of the new "cmake-{OS}" and execute cmake for the root directory (the next steps are in a Linux machine, but a similar result shall be obtained from a Windows one).

```bash
cmake ..
```

As a result, cmake shall create a few files and directories, including the _make files_ that contain the project in the target compiler format. You can see that it creates the project for 3 libraries.

```bash
-- creating library pal_linux_1[pal_version.c;pal_temperature_a.c;pal_led.c;pal_fan_a.c;pal_exception.c;pal_thread_pthread.c]
-- creating library pal_linux_2[pal_version.c;pal_temperature_b.c;pal_led.c;pal_fan_b.c;pal_exception.c;pal_thread_pthread.c]
-- creating library pal_linux_3[pal_version.c;pal_temperature_b.c;pal_led.c;pal_fan_c.c;pal_exception.c;pal_thread_pthread.c]
```

Now we can build the project itself.

```bash
cmake --build .
```

As you can note in the cmake logs, cmake built each of the `.c` that each config library needed, and linked it together creating three static libraries,

```bash
Scanning dependencies of target pal_linux_3
[  2%] Building C object pal/CMakeFiles/pal_linux_3.dir/pal_version.c.o
[  5%] Building C object pal/CMakeFiles/pal_linux_3.dir/pal_temperature_b.c.o
[  8%] Building C object pal/CMakeFiles/pal_linux_3.dir/pal_led.c.o
[ 10%] Building C object pal/CMakeFiles/pal_linux_3.dir/pal_fan_c.c.o
[ 13%] Building C object pal/CMakeFiles/pal_linux_3.dir/pal_exception.c.o
[ 16%] Building C object pal/CMakeFiles/pal_linux_3.dir/pal_thread_pthread.c.o
[ 18%] Linking C static library libpal_linux_3.a
[ 18%] Built target pal_linux_3
Scanning dependencies of target pal_linux_2
[ 21%] Building C object pal/CMakeFiles/pal_linux_2.dir/pal_version.c.o
[ 24%] Building C object pal/CMakeFiles/pal_linux_2.dir/pal_temperature_b.c.o
[ 27%] Building C object pal/CMakeFiles/pal_linux_2.dir/pal_led.c.o
[ 29%] Building C object pal/CMakeFiles/pal_linux_2.dir/pal_fan_b.c.o
[ 32%] Building C object pal/CMakeFiles/pal_linux_2.dir/pal_exception.c.o
[ 35%] Building C object pal/CMakeFiles/pal_linux_2.dir/pal_thread_pthread.c.o
[ 37%] Linking C static library libpal_linux_2.a
[ 37%] Built target pal_linux_2
Scanning dependencies of target pal_linux_1
[ 40%] Building C object pal/CMakeFiles/pal_linux_1.dir/pal_version.c.o
[ 43%] Building C object pal/CMakeFiles/pal_linux_1.dir/pal_temperature_a.c.o
[ 45%] Building C object pal/CMakeFiles/pal_linux_1.dir/pal_led.c.o
[ 48%] Building C object pal/CMakeFiles/pal_linux_1.dir/pal_fan_a.c.o
[ 51%] Building C object pal/CMakeFiles/pal_linux_1.dir/pal_exception.c.o
[ 54%] Building C object pal/CMakeFiles/pal_linux_1.dir/pal_thread_pthread.c.o
[ 56%] Linking C static library libpal_linux_1.a
```

If you would like to investigate the cmake build process, you can turn on the building logs, by setting the variable `CMAKE_VERBOSE_MAKEFILE` in the root _CMakeList.txt_ to **on**.

```cmake
set(CMAKE_VERBOSE_MAKEFILE on)
```

With it, you will see that cmake is calling the compiler, for example **cc** on linux, to build each source code.

```bash
cc -o <source1>.a.o -c <source1>.c
cc -o <source2>.a.o -c <source2>.c
...
```

And, at the end, it links all together creating the library, for example, using **ar** command in linux.

```bash
ar qc lib<mylib>.a <source1>.c.o <source2>.c.o ...
```

CMake will do that for all 3 libraries, resulting in the following files and directories in the _cmake-linux/pal_

```bash
    ./
    ../
    CMakeFiles/
    Makefile
    cmake_install.cmake
    libpal_linux_1.a
    libpal_linux_2.a
    libpal_linux_3.a
```

Examining the 3 libraries with **nm** command.

<table>
    <tr>
        <th>
            libpal_linux_1.a
        </th>
        <th>
            libpal_linux_2.a and libpal_linux_3.a
        </th>
    </tr>
    <tr>
        <td>
            <pre lang="bash">
<br>pal_version.c.o:
0000000000000010 T pal_version_get_major
0000000000000020 T pal_version_get_minor
0000000000000030 T pal_version_get_patch
0000000000000000 T pal_version_get_string
<br>pal_temperature_a.c.o:
0000000000000000 T pal_temperature_get
0000000000000050 T pal_temperature_to_celsius
                 U printf <br>
pal_led.c.o:
0000000000000000 T pal_led_set_color
                 U printf <br>
pal_fan_a.c.o:
0000000000000000 T pal_fan_set_speed
                 U printf <br>
pal_exception.c.o:
0000000000000000 T pal_halt <br>
pal_thread_pthread.c.o:
                 U nanosleep
0000000000000000 T pal_sleep
            </pre>
        </td>
        <td>
            <pre lang="bash">
pal_version.c.o:
0000000000000010 T pal_version_get_major
0000000000000020 T pal_version_get_minor
0000000000000030 T pal_version_get_patch
0000000000000000 T pal_version_get_string
<br>pal_temperature_b.c.o:
0000000000000000 r .LCPI1_0
0000000000000000 T pal_temperature_get
0000000000000050 T pal_temperature_to_celsius
                 U printf <br>
pal_led.c.o:
0000000000000000 T pal_led_set_color
                 U printf <br>
pal_fan_a.c.o:
0000000000000000 T pal_fan_set_speed
                 U printf <br>
pal_exception.c.o:
0000000000000000 T pal_halt <br>
pal_thread_pthread.c.o:
                 U nanosleep
0000000000000000 T pal_sleep
            </pre>
        </td>
    </tr>
</table>

As expected, because the implementation are very similar, all libraries implemented the same symbols, and expected that some other library implement the missing symbols `printf` and `nanosleep`.

## Using the PAL

### Creating the Device

#### Structure of the device

The PAL libraries is used by the _monitor_ code that implement the business logic described in the scenario. However, it can be accessed by other parts of the system as well. The following diagram represent the _Device_ stack.

```
+-------------------------------------------------+
|                       main                      |
+-------------------------------------------------+
             |                    |
    +-----------------+           |
    |     monitor     |           |
    +-----------------+           |
             |                    |                     <==== PAL contract
+-------------------------------------------------+
|                       pal                       |
+-------------------------------------------------+
      |           |            |             |
+-----------+  +-----+  +-------------+  +--------+
| exception |  | fan |  | temperature |  | thread |
+-----------+  +-----+  +-------------+  +--------+
```

The `main()` function in the _Device_ implements a simple loop that calls the `temperature_monitor()` once a second. The `main()` function uses the PAL to create the 1 second interval, and the `temperature_monitor()` uses the PAL to read the sensor, change the fan and LED status and put the device on halt.

#### How the add_device() works

Each device that this sample will create shall be composed by one of the PAL libraries, the monitor library, that implements the business logic, and the main object, that contains the infinite loop that will run the system. The device executable is created by the function `add_device()` implemented in _cmake/config.cmake_, which is called by the `CMakeLists.txt` in the _device_ directory, once for each library pal library in pal_libraries list.

The `add_device()` will create a new executable named <i>device_{library_name}</i> with one object file created from _main.c_, by calling `add_executable()`.

```cmake
    add_executable(${TARGET_NAME} 
        main.c
    )
```

CMake will build all `.c` in the `add_executable`, converting it in a object file, for example, in Linux, the _main.c_ will be builded in _main.c.o_. Those `.c` may need some includes, `target_include_directories()` will point where the include files shall be found.

```cmake
    target_include_directories(${TARGET_NAME} 
        PUBLIC 
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/pal
            ${CMAKE_SOURCE_DIR}/monitor
    )
```

And cmake needs to know what are the libraries that the linker shall link together with object main.o, and what is the **sequency** to include this libraries, `add_device()` do that by calling `target_link_libraries()`.

```cmake
    target_link_libraries(${TARGET_NAME} 
        PUBLIC 
            monitor
            ${config_name}
    )
```

The combination of `add_executable()` and `target_include_directories()` commands, will provide 3 files for the linker as following (let's examine only the libpal_linux_1, the others one are similar to that).

<table>
    <tr>
        <th>
            main.c.o
        </th>
        <th>
            libmonitor.a
        </th>
        <th>
            libpal_linux_1.a
        </th>
    </tr>
    <tr>
        <td>
            <pre lang="bash">
T main
U pal_sleep
U pal_temperature_get
U pal_temperature_to_celsius
U pal_version_get_string
U printf
U temperature_monitor
<br><br><br><br><br><br><br><br>
<br><br><br><br><br><br><br><br>
            </pre>
        </td>
        <td>
            <pre lang="bash">
<i><b>monitor.c.o:</b></i>
r .LCPI0_0
r .LCPI0_1
r .LCPI0_2
r .LCPI0_3
r .LCPI0_4
r .LCPI0_5
r .LCPI0_6
U pal_fan_set_speed
U pal_halt
U pal_led_set_color
U pal_sleep
U pal_temperature_get
U pal_temperature_to_celsius
T temperature_monitor
<br><br><br><br><br><br><br><br><br>
            </pre>
        </td>
        <td>
            <pre lang="bash">
<i><b>pal_version.c.o:</b></i>
T pal_version_get_major
T pal_version_get_minor
T pal_version_get_patch
T pal_version_get_string
<br><i><b>pal_temperature_a.c.o:</b></i>
T pal_temperature_get
T pal_temperature_to_celsius
U printf
<br><i><b>pal_led.c.o:</b></i>
T pal_led_set_color
U printf
<br><i><b>pal_fan_a.c.o:</b></i>
T pal_fan_set_speed
U printf
<br><i><b>pal_exception.c.o:</b></i>
T pal_halt
<br><i><b>pal_thread_pthread.c.o:</b></i>
U nanosleep
T pal_sleep
            </pre>
        </td>
    </tr>
</table>

#### How linker will attach all together

One of the biggest difference between link a object file (.o) and a static library (.a) is the way that linker look for the symbols. As a general ruler, linker will add all symbols in a object file to the linker table and make the code available, no matter if some other code need it or not. At the end, if a function is not used, the linker will remove it from the binary to save some space (there are some exceptions, developers can ask the linker to keep a function even if nobody is using it).

For a static library, the linker will not do that, instead, it will try to find the symbols that are already in the link table without code, and see if the static library implements it. If the static library implements it, the linker will use the object file that contains that implementation and mark that symbol as implemented.

Let's see it step by step, we will concentrate only in the relevant parts of the linker:

First, linker will include the object files, in this sample, we only have main.c.o. The only code already implemented is the `main()` function.

```
Symbol                              Required by                             Implemented by

T main                              main.c.o                                main.c.o
U pal_sleep                         main.c.o
U pal_temperature_get               main.c.o
U pal_temperature_to_celsius        main.c.o
U pal_version_get_string            main.c.o
U printf                            main.c.o
U temperature_monitor               main.c.o
```

After that, the linker will start with the static libraries, using the order in the `target_link_libraries()`. So, it will start with the _libmonitor.a_. This library implements the function `temperature_monitor()` that is currently in the link table, but without code. So, the linker will bring the entire monitor.c.o to the table.

```
Symbol                              Required by                             Implemented by

T main                              main.c.o                                main.c.o
U pal_fan_set_speed                 monitor.c.o
U pal_halt                          monitor.c.o
U pal_led_set_color                 monitor.c.o
U pal_sleep                         main.c.o, monitor.c.o
U pal_temperature_get               main.c.o, monitor.c.o
U pal_temperature_to_celsius        main.c.o, monitor.c.o
U pal_version_get_string            main.c.o
U printf                            main.c.o
T temperature_monitor               main.c.o                                monitor.c.o
```

The next library is libpal_linux_1.a, because of pal_fan_set_speed, the linker will brings the pal_fan_a.c.o, because of the pal_halt, the pal_exception.c.o, and so on.

```
Symbol                              Required by                             Implemented by

T main                              main.c.o                                main.c.o
U nanosleep                         pal_thread_pthread.c.o                  @@GLIBC
T pal_fan_set_speed                 monitor.c.o                             pal_fan_a.c.o
T pal_halt                          monitor.c.o                             pal_exception.c.o
T pal_led_set_color                 monitor.c.o                             pal_led.c.o
T pal_sleep                         main.c.o, monitor.c.o                   pal_thread_pthread.c.o
T pal_temperature_get               main.c.o, monitor.c.o                   pal_temperature_a.c.o
T pal_temperature_to_celsius        main.c.o, monitor.c.o                   pal_temperature_a.c.o
T pal_version_get_major                                                     pal_version.c.o
T pal_version_get_minor                                                     pal_version.c.o
T pal_version_get_patch                                                     pal_version.c.o
T pal_version_get_string            main.c.o                                pal_version.c.o
U printf                            main.c.o, pal_fan_a.c.o,                @@GLIBC
                                    pal_led.c.o, pal_temperature_a.c.o
T temperature_monitor               main.c.o                                monitor.c.o
```

Both missing symbols now belong to the standard C library and will be dynamically linked by the OS (_GLIBC_ on linux), we will not cover it in this sample.

As the last steep, linker may remove all unused symbols.

#### Linker order

The above information shows that the order of the libraries is essential in the linker process. To make it clear, lets change the library order and see what happen, you can try that by changing the target_link_libraries() order.

```cmake
    target_link_libraries(${TARGET_NAME} 
        PUBLIC 
            ${config_name}
            monitor
    )
```

After grab all the object files, the first library that the linker will add will be the libpal_linux_1.a, bringing the objects for the pal_xxx functions required by the `main()` function.

```
Symbol                              Required by                             Implemented by

T main                              main.c.o                                main.c.o
U nanosleep                         pal_thread_pthread.c.o
T pal_sleep                         main.c.o                                pal_thread_pthread.c.o
T pal_temperature_get               main.c.o                                pal_temperature_a.c.o
T pal_temperature_to_celsius        main.c.o                                pal_temperature_a.c.o
T pal_version_get_major                                                     pal_version.c.o
T pal_version_get_minor                                                     pal_version.c.o
T pal_version_get_patch                                                     pal_version.c.o
T pal_version_get_string            main.c.o                                pal_version.c.o
U printf                            main.c.o, pal_temperature_a.c.o
U temperature_monitor               main.c.o
```

Now, linker will bring the `libmonitor.a`.

```
Symbol                              Required by                             Implemented by

T main                              main.c.o                                main.c.o
U nanosleep                         pal_thread_pthread.c.o                  @@GLIBC
U pal_fan_set_speed                 monitor.c.o
U pal_halt                          monitor.c.o
U pal_led_set_color                 monitor.c.o
T pal_sleep                         main.c.o, monitor.c.o                   pal_thread_pthread.c.o
T pal_temperature_get               main.c.o, monitor.c.o                   pal_temperature_a.c.o
T pal_temperature_to_celsius        main.c.o, monitor.c.o                   pal_temperature_a.c.o
T pal_version_get_major                                                     pal_version.c.o
T pal_version_get_minor                                                     pal_version.c.o
T pal_version_get_patch                                                     pal_version.c.o
T pal_version_get_string            main.c.o                                pal_version.c.o
U printf                            main.c.o, pal_temperature_a.c.o         @@GLIBC
T temperature_monitor               main.c.o                                monitor.c.o
```

As a result, at the end of the link process, the functions `pal_fan_set_speed()`, `pal_halt()`, `pal_led_set_color()` are not implemented by any object, and the linker will throw an error.

```bash
/usr/bin/ld: ../monitor/libmonitor.a(monitor.c.o): in function `temperature_monitor':
monitor.c:(.text+0x61): undefined reference to `pal_halt'
/usr/bin/ld: monitor.c:(.text+0xd7): undefined reference to `pal_led_set_color'
/usr/bin/ld: monitor.c:(.text+0xdf): undefined reference to `pal_halt'
/usr/bin/ld: monitor.c:(.text+0x124): undefined reference to `pal_led_set_color'
/usr/bin/ld: monitor.c:(.text+0x169): undefined reference to `pal_led_set_color'
/usr/bin/ld: monitor.c:(.text+0x178): undefined reference to `pal_led_set_color'
/usr/bin/ld: monitor.c:(.text+0x198): undefined reference to `pal_fan_set_speed'
/usr/bin/ld: monitor.c:(.text+0x1a7): undefined reference to `pal_led_set_color'
/usr/bin/ld: monitor.c:(.text+0x1af): undefined reference to `pal_halt'
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```

> **_Note:_** Some linkers resolve it by revisiting the static libraries multiple times. But there is no guarantees about it.

### The Contract

Because the `pal.h` exposes a single interface for all PAL libraries, there is no need for multiple implementations of the _device_ or the _monitor_. But we should always make sure that the **PAL contract** is respected by both sides, all PAL implementations, and all codes that use it.

#### Versioning

The most important part of any contract is the clauses that make clear the responsibilities of for both sides. In our case, we call it as **requirements**. Write software requirements is not fun, it is a tedious process that many developers avoid as much as possible, however, it is the only way to have a good contract that will guide all the tests for the interface. The requirements shall describe what the code **shall** and **shall not** do.

Another important point about the requirements is the evolution of it. A good contract **shall not** change along the time. Change an interface requirement is knowing as **breaking change** and cause damage to codes that use it. On the other hand, freeze the requirements, will freeze the software evolution.

To solve this dilemma, a good contract shall have version. A good semantic for a version splits it in 3 parts.

1. _major_: New version that can be incompatible with the previews one and may contain **breaking changes**.
2. _minor_: It represents a significant increment in the API, but without breaking changes.
3. _patch_: Small fix in the code.

This sample uses this semantic, with a single version for all PAL libraries. The version is defined in the `config.cmake` and passed down to the PAL code using the `target_compile_definitions` in the PAL `CMakeLists.txt`.

```cmake
    target_compile_definitions(${pal} PUBLIC 
        "PAL_VERSION_MAJOR=${PAL_VERSION_MAJOR}"
        "PAL_VERSION_MINOR=${PAL_VERSION_MINOR}"
        "PAL_VERSION_PATCH=${PAL_VERSION_PATCH}")
```

Because version is constant in the PAL, developers may be tented to expose it in the `pal.h`, something like this:

```c
const char* PAL_VERSION_STR = STR(PAL_VERSION_MAJOR) "." \
                              STR(PAL_VERSION_MINOR) "." \
                              STR(PAL_VERSION_PATCH);
```

However, the `pal.h` may be included in the files that will use the library, and it will result in `PAL_VERSION_STR` as part of the _.text_ area of the user of the library not the _.text_ area of the library itself, which is very undesirable.

To make sure that this sample avoid this bad behavior, the PAL `CMakeLists.txt` uses `target_compile_definitions` instead of `add_definitions`. In this case, if a developer try to use the version constant directly, for instance the `PAL_VERSION_MAJOR`, the compiler will throw an error because this symbol does not exists outside of the PAL library.

As a result, in the sample, we have the `pal_version.c` that will expose the version information to the system, keeping this information in the _.text_ area of the library. With it, users may test the PAL version to make sure that it is compatible with their current code.

#### Requirements

Test the version can help to understand and keep track of possible breaking changes, so, it is essential that the users of an library keep tests that will test their code against the interface requirements.

Some developers like to create a requirement document, others, like myself, put the requirements as comments, together with the tests that check that requirement. No matter your stile, it is always a good idea to define all the requirements, and negotiate them with the other developers before start any implementation. **If you are discussing requirements in the code PR (Pull Request), you are wasting time and money**.

To demonstrate how to test the interface, we defined some requirements, it is a subset, far to cover the entire interface, but will help us to understand the interface test concept.

```c
/** pal_temperature_get shall return the current temperature and PAL_OK. */
/** pal_temperature_get shall return PAL_ERROR_ARG if the temperature pointer is NULL. */
/** pal_fan_set_speed shall return PAL_OK for speeds between 1200 and -800. */
/** pal_fan_set_speed shall return PAL_ERROR_ARG if the speed is not supported. */
/** pal_halt shall completely stop the system. */
```

> **_Note:_** It is important to test all requirements, however, because it is related to the hardware, there will be cases that test a requirement is supper hard of even impossible, for example, the requirement related to "pal_halt".

The code that uses the interface shall have its own set of requirements, once again, here is a subset of it.

```c
/** temperature_monitor shall turn the LED green if the temperature is between -10.0c and 60.0c. */
/** temperature_monitor shall turn the LED yellow if the temperature is between -38.0c and -10.0c or between 80.0c and 60.0c. */
/** temperature_monitor shall turn the LED red if the temperature is bellow -38.0c or above 80.0c. */
/** temperature_monitor shall set the fan speed to 10 times the reported temperature. */
/** temperature_monitor shall set LED to red, turn fan off, and halt the system if the temperature if above 120.0c or if the temperature sensor failed to read the temperature. */
```

> **_Note:_** The interface requirements shall guide the user's requirements, for example, every code that call `pal_temperature_get` shall test the success case, where it returns `PAL_OK`, but the fail case as well, where it returns `PAL_ERROR_ARG`.

#### User test

In this sample, _tests/temperature_monitor_ut.c_ is a set of unit tests that check the "temperature_monitor" and other device-related requirements. Some of these requirements can be tested in the device as is. However, most requirements, specially those requiring unit test, are better checked if the test has full control of the dependencies, in this case, the user's test shall have control on the results of the PAL.

PAL is related to the hardware, and control the hardware result using the hardware itself is hard and unreliable. The solution for that is **mock** the interface. To make things simple, this sample will mock the interface using the linker.

> **_Note:_** It is highly recommended that you use a mock framework like [umock-c](https://github.com/Azure/umock-c) or [cmocka](https://cmocka.org).

Let's understand why we need to mock the interface. To do this, let's focus on the last _temperature_monitor_ requirement in the list above.

```c
/** temperature_monitor shall set LED to red, turn fan off, and halt the system if the temperature is above 120.0c or if the temperature sensor failed to read the temperature. */
```

As result of this requirement, the system shall goes to a full stop state. Some systems will allow developers to test the full stop using a system error that will result in a call to a halt function that the BSP will implement as an infinite loop. These systems allows developers to overwrite this halt functions to have something that the test can check instead of halting the device. The issue with this approach is those halt functions are platform-dependent, and will require one test implementation for each platform, which we what to avoid. So, mock the `pal_halt()` will create a way to check if the halt was called without really stop the system.

Another issue, is how to trigger these requirements. It can be triggered by a high temperature on the sensor or a fault in the temperature reader. Activate these situations using the hardware will require changes in the circuit and addition of other devices, such as a heater. Nobody wants it!

So, mock the `pal_temperature_get()` will allow the test to define the correct behavior to trigger the test.

But, the trick here is how to do that. We saw that we can define in the liker each PAL library the application shall use. However, in the unit tests, we want to replace only part of the library. For example, there is no reason to replace the `pal_sleep()` function.

The good news is, because PAL is a static library, linker will help us in this task. Let's remember 2 rules about how linker uses static libraries.

1. Linker will only add symbols from the static library if this symbol is in the linker table waiting for an implementation.
2. If at least one symbol is required from an object in the static library, linker will bring all symbols of that object to the table.

The first rule will help to mock the pal functions, if the test contains an implementation for all needed functions from a particular object in the static library, the linker will ignore that object. However, the second rule tells that we must make sure that we are not using any function in that particular object in the library, if we use any symbol from the object, linker will bring the entire object, resulting in linker error of multiple implementations of the same symbol. So, when define the test, developers shall decide each objects will be used from the static library and each ones will be mocked.

So, targeting the sample, to do the unit test for `temperature_monitor`, we decided to mock functions `pal_led_set_color()`, `pal_temperature_get()`, `pal_temperature_to_celsius()`, `pal_fan_set_speed()`, and `pal_halt()` and use the version functions and `pal_sleep()` from the pal library as is. So, if you look at the _temperature\_monitor\_ut.c_, at the beginning, there is a session with the mock implementation of the pal, it is below the comment:

```c
/**
 * Session of mock functions that will replace the PAL ones.
 */
```

You can build and run the _temperature_monitor_ut_ to see that those tests are calling the mock functions instead of the real one. For example

```bash
Execute test temperature_monitor_with_too_hight_temperature_halt
This is the mock pal_temperature_get
This is the mock pal_temperature_get
This is the mock pal_temperature_get
This is the mock pal_led_set_color
This is the mock pal_fan_set_speed
This is the mock pal_halt... HALT!
Succeeded

```
To test the second ruler, you can comment the `pal_temperature_to_celsius()`

```c
// float pal_temperature_to_celsius(unsigned short temperature) {
//     return ((float)(temperature - 80) / 2.0f);
// }
```

and try to build the solution again. As a result, the linker will complain about the double definition of `pal_temperature_get()`.

```bash
...
[ 83%] Linking C executable temperature_monitor_ut
/usr/bin/ld: ../pal/libpal_linux_1.a(pal_temperature_a.c.o): in function `pal_temperature_get':
pal_temperature_a.c:(.text+0x0): multiple definition of `pal_temperature_get'; CMakeFiles/temperature_monitor_ut.dir/temperature_monitor_ut.c.o:temperature_monitor_ut.c:(.text+0x40): first defined here
clang: error: linker command failed with exit code 1 (use -v to see invocation)
make[2]: *** [tests/CMakeFiles/temperature_monitor_ut.dir/build.make:105: tests/temperature_monitor_ut] Error 1
make[1]: *** [CMakeFiles/Makefile2:380: tests/CMakeFiles/temperature_monitor_ut.dir/all] Error 2
make: *** [Makefile:103: all] Error 2
```

> **_Note:_** The trick about this issue is the function that cause the problem is not the one reported in the error. So, to fix the double definition, we need to investigate the other functions in the object, besides the one reported in the error.

#### Compliance Test

Now that we know that our code works with the PAL, following the defined contract, we have to ensure that all PAL libraries, independent of the platform, have the same behavior. This is done by the **compliance test**.

The cmake files created for this sample will always create a library for each PAL listed in _pal\_libraries_, and the function `add_compliance_test()` will test each library against the compliance test _pal\_compliance\_ut.c_. So, after build the sample, besides the _temperature\_monitor\_ut_, there will be 3 compliance tests in the _tests_ directory.

```bash
./
../
CMakeFiles/
Makefile*
cmake_install.cmake
pal_linux_1_compliance_ut
pal_linux_2_compliance_ut
pal_linux_3_compliance_ut
temperature_monitor_ut
```

These tests will run the PAL APIs against the PAL contract, executing the _pal_linux_1_compliance_ut_ the result will shown that there are no errors, which means that the _pal_linux_1.a_ is in compliance with the PAL contract and can be used in the system.

```bash
Starting unit tests:
Execute test pal_temperature_get_success
This is the real pal_temperature_get for device A
Succeeded

Execute test pal_temperature_get_with_null_pointer_fail
This is the real pal_temperature_get for device A
Succeeded

Execute test pal_fan_set_speed_hi_success
This is the real pal_fan_set_speed A
Set speed to 1200
Succeeded

Execute test pal_fan_set_speed_low_success
This is the real pal_fan_set_speed A
Set speed to -800
Succeeded

End unit tests with 0 errors
```

However, it is not true for _pal_linux_3_compliance_ut_, in fact, the fan **C** does not runs in anticlockwise. You can see that the last test returns an error.

```bash
 Starting unit tests:
Execute test pal_temperature_get_success
This is the real pal_temperature_get for device B
Succeeded

Execute test pal_temperature_get_with_null_pointer_fail
This is the real pal_temperature_get for device B
Succeeded

Execute test pal_fan_set_speed_hi_success
This is the real pal_fan_set_speed C
Set speed to 1200
Succeeded

Execute test pal_fan_set_speed_low_success
This is the real pal_fan_set_speed C
pal_fan_set_speed return failed. Expected:PAL_OK Actual:2

End unit tests with 1 errors

```
The interesting part about it, is all 3 libraries expose the correct signature, and in a simple test, using only clockwise speeds on the fan, they will all succeed. But, if you deploy the device in the field using the 3rd library, soon or latter, it will fail.

So, a few important rulers about the compliance test:

1. Always create libraries for settings and include the `.a` or `.lib` to the project, never include a `.c` file from the PAL directly to the project.
2. If you need a different configuration, create a library for that.
3. If you find a bug in the library that is not covered by the compliance test, add a test to it. It will help fix the same bug in all other settings.

## Conclusion

We show with this sample that it is possible to create and test multiple platform adapters for a device and build it all using a simple set of cmake files. This sample runs properly in Linux and Windows, using different implementations for possible hardware on the device.

A compliance test made sure that all versions of the PAL that will be deployed works properly, and shown that the 3rd configuration, despite the fact that it works fine in a simple test, shall not be used.
