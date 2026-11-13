![new_logo](https://github.com/user-attachments/assets/1c6b585d-6b34-4f38-a9e3-84af16954a2c)


# 🎮 Scion2D – A Simple 2D Game Engine for Learning and Teaching

Scion2D is a lightweight, educational 2D game engine built with the goal of **learning by doing** 🧠💡. It's part of my journey to understand game development from the ground up and share everything I learn through my [YouTube channel]([https://www.youtube.com/](https://www.youtube.com/playlist?list=PL3HUvSWOJR7XRDwVVQqqWO-zyyscb8L-v)) 📺✨.

> This engine isn’t meant to compete with big names like Unity or Godot - and that’s okay!  
> It’s about exploring, building, breaking things, and **growing as a developer** 🚀.

Whether you're here to learn alongside me, peek at the source, or just curious about making your own engine, welcome! 👋

---

🛠 **Why Scion2D?**

- ✅ Made with ❤️ for education
- 🎓 Teach what I learn, and learn what I teach
- 🎮 Build real 2D games from scratch
- 💬 Collaborate, share ideas, and grow together

---

## 📖 Documentation
> **⚠️ Active Development Notice:** This engine and editor are currently under active development. The documentation is still a work in progress and will not be up to date.
[Scion2D Docs](https://dwjclark11.github.io/Scion2D_Docs/)

## 🚀 Features
-  **Cross-Platform**: Builds on Windows and Linux. Currently Mac is not supported.
-  **Modern C++23**: Use of modern c++ standards and functionality.
-  **Integrated Tilemap Editor**: Ability to create tilemaps, create game objects, and more.

---- 
### Hub
<img width="1366" height="768" alt="scion_hub" src="https://github.com/user-attachments/assets/62db81fa-80c2-4413-8f6e-877cb58c1c51" />


----
#### Scion Editor
<img width="1920" height="1032" alt="NewSceneEditorUI" src="https://github.com/user-attachments/assets/374c627f-4e75-4c6e-acee-2caf358ea294" />

----

# Build
----
Requires [CMake 3.26+](https://cmake.org/), a C++23 capable compiler, [Git](https://git-scm.com/) and [vcpkg](https://github.com/microsoft/vcpkg).

Dependencies are declared in the `vcpkg.json` manifest at the root of the repository. vcpkg runs in manifest mode, so the packages are downloaded and built automatically the first time you configure the project. You do **not** need to run `vcpkg install` or `vcpkg integrate install`, and nothing has to be installed globally except the Linux system packages listed below.

## Get VCPKG
```
git clone https://github.com/microsoft/vcpkg
```
- Windows
	```
	./vcpkg/bootstrap-vcpkg.bat -disableMetrics
	```
- Linux
	```
	./vcpkg/bootstrap-vcpkg.sh -disableMetrics
	```
	vcpkg needs `curl zip unzip tar` on Linux:
	```
	sudo apt install curl zip unzip tar
	```

### Set the environment variables
The root `CMakeLists.txt` picks up `$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake` as the toolchain file automatically, so `VCPKG_ROOT` is the only variable that is required.
```
VCPKG_ROOT=[path_to_vcpkg]
```
Optionally set the default triplet:
```
VCPKG_DEFAULT_TRIPLET=x64-windows   (Windows)
VCPKG_DEFAULT_TRIPLET=x64-linux     (Linux)
```

- Windows

	Add `VCPKG_ROOT` through the System Environment Variables dialog, or from a terminal:
	```
	setx VCPKG_ROOT "<path_to_vcpkg_installation_folder>"
	```

- Linux

	Add the following lines at the end of your `~/.bashrc`:
	```
	export VCPKG_ROOT=<path_to_vcpkg_installation_folder>
	export PATH=$VCPKG_ROOT:$PATH
	```
	Apply changes:
	```
	source ~/.bashrc
	```

Restart your terminal or IDE after changing environment variables.

## Linux system packages
These are the only things that still need to be installed by hand, because they are system libraries the vcpkg ports (mainly SDL3) rely on.
- Linux[debian based]
	```
	sudo apt install python3-jinja2 autoconf automake libtool pkg-config libibus-1.0-dev
	```
	* if[Xorg]
		 ```
		sudo apt install libx11-dev libxft-dev libxext-dev
		```
	* if[Wayland]
		```
		sudo apt install libwayland-dev libxkbcommon-dev libegl1-mesa-dev
		```
	* Optional but good practice
		```
		sudo apt install build-essential
		```

Windows does not need any extra system packages.

## Clone and build the repository
```
git clone https://github.com/dwjclark11/Scion2D.git
cd Scion2D
cmake -S . -B build
cmake --build build
```
The first configure takes a while because vcpkg builds every dependency listed in `vcpkg.json`. The built packages are cached, so later configures are fast.

In-source builds are not allowed, always use a separate build directory such as `build`.

### Visual Studio
Open the repository folder directly (File > Open > Folder). Visual Studio reads `CMakeSettings.json` and the vcpkg manifest, and restores the packages on its own as long as `VCPKG_ROOT` is set.

### Adding a dependency
Add the package name to the `dependencies` list in `vcpkg.json` and re-run the CMake configure step. Do not run `vcpkg install <package>`.

## Use the built-in OpenGL debugger
Note: this requires a graphics adapter with OpenGL version >= 4.3 capabilities.

In Scion2D/CMakeLists.txt:
* Set the variable ```SCION_OPENGL_DEBUG_CALLBACK``` to ```ON```.
* Optionally, set the variable ```SCION_OPENGL_DEBUG_FORWARD_COMPATIBILITY``` to ```ON``` in order to enable warnings about deprecated OpenGL functions.

Activate the debugger in your code as soon as you have a valid OpenGL context made current:
* ```Scion::Rendering::OpenGLDebugger::init()```.
* Optionally, you can opt out a list of warning wy doing the following:
	```
	std::vector<unsigned int> ignore{ 1281, 131169, 131185, 131204, 31218 };
	Scion::Rendering::OpenGLDebugger::init( ignore );
	```

* To allow the debugger to break, call ```Scion::Rendering::OpenGLDebugger::breakOnError( true/false )``` and/or ```Scion::Rendering::OpenGLDebugger::breakOnWarning( true/false )```.
* To ignore a specific warning, call ```Scion::Rendering::OpenGLDebugger::push( Id )```.
* To reinstate, call ```Scion::Rendering::OpenGLDebugger::pop( Id )```.
* To set the severity level, call ```Scion::Rendering::OpenGLDebugger::setSeverityLevel( Scion::Rendering::OpenGLDebuggerSeverity::Disable/Notification/Low/Medium/High )```.

## Force the discrete GPU on Optimus laptops
In Scion2D/CMakeLists.txt:
* Set the variable ```SCION_OPENGL_FORCE_DISCRETE_GPU``` to ```ON```.
Note: Ids can differ between platforms or drivers.
Note for linux users: this is currently working only on nVidia GPUs.

## 🎯 Supported Platforms

| Platform | Compiler   | Status             |
| -------- | ---------- | ------------------ |
| Windows  | MSVC 2019+ | ✅ Fully Supported |
| Windows  | MinGW/GCC  | 🔧 To be tested    |
| Linux    | GCC 10+    | ✅ Fully Supported |
| Linux    | Clang 12+  | 🔧 To be tested    |

* Linux GCC build has warnings but builds successfully.

## 📝 License

This project is licensed under the [MIT License](LICENSE) - see the LICENSE file for details.

## 🙏 Acknowledgments
This project would not be possible without the help of all the contributors, the motivation to keep working forward through the wonderful comments and supporters from my YouTube Channel. 
Also from all the wonderful open source projects that I have been able to use in the creation of the engine.

### Open Source Dependencies
Check out these amazing open source projects that we are using in the engine. Make sure to give them all a star! for all of their amazing work.

-   **[EnTT](https://github.com/skypjack/entt)** - Fast and reliable Entity Component System.
-   **[SDL3](https://github.com/libsdl-org/SDL)** -  a cross-platform library that provides an abstraction layer for computer multimedia hardware components.
-   **[SDL3_mixer](https://github.com/libsdl-org/SDL_mixer)** - An audio mixer that supports various file formats for Simple Directmedia Layer.
-   **[Dear ImGui](https://github.com/ocornut/imgui)** - Immediate mode GUI for C++.
-   **[Box2D](https://github.com/erincatto/box2d)** - 2D physics engine for games.
-   **[GLM](https://github.com/g-truc/glm)** - Mathematics library for graphics software.
-   **[RapidJSON](https://github.com/Tencent/rapidjson)** - Fast JSON parser and generator.
-   **[sol2](https://github.com/ThePhD/sol2)** - C++ Lua binding library.
-   **[stb](https://github.com/nothings/stb)** - Single-file public domain libraries.
-   **[Lua](https://www.lua.org/)** - Powerful, efficient scripting language.
-   **[tinyfiledialogs](https://github.com/native-toolkit/libtinyfiledialogs)** - Cross-Platform Native dialog library for WINDOWS MAC OSX GTK+ QT CONSOLE **[Official](https://sourceforge.net/projects/tinyfiledialogs/)**.
-   **[Libzippp](https://github.com/ctabin/libzippp)** - C++ wrapper around the libzip library.
-   **[tinyxml2](https://github.com/leethomason/tinyxml2)** - TinyXML2 is a simple, small, efficient, C++ XML parser that can be easily integrated into other programs.
-   **[FMT](https://github.com/fmtlib/fmt)** - A modern formatting library
-   **[Tracy Profiler](https://github.com/wolfpld/tracy)** - A real time, nanosecond resolution, remote telemetry, hybrid frame and sampling profiler for games and other applications.

_Thank you to all the contributors and maintainers of these projects!_ ❤️
