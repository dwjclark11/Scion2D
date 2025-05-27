
![new_logo](https://github.com/user-attachments/assets/1c6b585d-6b34-4f38-a9e3-84af16954a2c)


# Introduction
The goal for **Scion2D** is to make a simple 2D game engine where I can teach what I am learning through my [youtube channel](https://www.youtube.com/playlist?list=PL3HUvSWOJR7XRDwVVQqqWO-zyyscb8L-v). 

This is for educational purposes and to constantly learn more about programming and gaming everyday. 

There are definitely better engines out there; however, I find it more fun to try to build your own, learn from others, and grow as a developer.

---- 

## Check the Docs
The documentation is still a work in progress, a lot of stuff has changed and it needs to be updated; however, you can check it out here:

[Scion2D Docs](https://dwjclark11.github.io/Scion2D_Docs/)

---- 
### Hub
![scionhub](https://github.com/user-attachments/assets/d9be3935-fa0f-470a-ad03-d1c31a241565)

----
#### Scion Editor
![neweditorpic](https://github.com/user-attachments/assets/147c74eb-82e7-42e5-a065-4b86b6cb1f88)

----

# Build
----
Requires [CMake 3.26](https://cmake.org/) and [vcpkg](https://github.com/microsoft/vcpkg)
## Get VCPKG:
```ps
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.bat -disableMetrics
```
### Make sure the following environment variables are set:
```
VCPKG_ROOT=[path_to_vcpkg]
VCPKG_DEFAULT_TRIPLET=x64-windows
```

- Windows

	Add the following line in your Path environnment variable:
	```
	<path_to_vcpkg_installation_folder>
	```

	Open a terminal and type the following:
	```
	vcpkg integrate install
	vcpkg integrate powershell
	```

- Linux

	Edit your profile's bashrc file:
	```
	nano ~/.bashrc
	```
	Add the following lines at the end:
	```
	export PATH=<path_to_vcpkg_installation_folder>:$PATH
	export VCPKG_ROOT=<path_to_vcpkg_installation_folder>
	export VCPKG_DEFAULT_TRIPLET=x64-linux
	```
	Apply changes:
	```
	source ~/.bashrc
	```

	Open a terminal and type the following:
	```
	vcpkg integrate install
	vcpkg integrate bash
	```
	
## Install dependencies 
- Windows
	```
	vcpkg install fmt glm entt glad soil2 sdl2 sdl2-mixer lua sol2 stb tinyfiledialogs rapidjson imgui[docking-experimental,opengl3-binding]
	```
- Linux[debian based]
	```
	sudo apt install python-jinja2 autoconf automake libtool pkg-config libibus-1.0-dev
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
	```
	vcpkg install fmt glm entt glad soil2 sdl2[alsa] sdl2-mixer lua sol2 stb tinyfiledialogs rapidjson imgui[docking-experimental,opengl3-binding]
	```
- ImGui SDL2-Binding
  * It seems like the ```ImGui[sdl2-binding]``` no longer exists in vcpkg. I have added the necessary files under the [thirdparty](https://github.com/dwjclark11/Scion2D/tree/master/thirdparty/imgui_backends) folder.
  * These should already be setup in cmake.
- Box2d Install
  * There has been a huge change in the latest box2d that is a breaking change to our codebase.
  * We are using Box2D 2.41, the latest 3.1, uses a C-API that is done in a completely different way.
  * Eventually we are going to have to update the code to support the latest; however, we currently have a work around.
  * Please see the ```Readme.md``` file here [SCION_PHYSICS](https://github.com/dwjclark11/Scion2D/tree/master/SCION_PHYSICS) for the workaround steps.
    
## Clone the repository 
```
git clone https://github.com/dwjclark11/Scion2D.git
cd Scion2D
cmake -S . -B build
```
 
* Also, in the main.lua file for the editor, comment out the loaded assets and files that don't exist. They will just error out.

## Use the built-in OpenGL debugger
Note: this requires a graphics adapter with OpenGL version >= 4.3 capabilities.

In Scion2D/CMakeLists.txt:
* Set the variable ```SCION_OPENGL_DEBUG_CALLBACK``` to ```ON```.
* Optionnaly, set the variable ```SCION_OPENGL_DEBUG_FORWARD_COMPATIBILITY``` to ```ON``` in order to enable warnings about deprecated OpenGL functions.

Activate the debugger in your code as soon as you have a valid OpenGL context made current:
* ```SCION_RENDERING::OpenGLDebugger::init()```.
* Optionnaly, you can opt out a list of warning wy doing the following:
	```
	std::vector<unsigned int> ignore{ 1281, 131169, 131185, 131204, 31218 };
	SCION_RENDERING::OpenGLDebugger::init( ignore );
	```

* To allow the debugger to break, call ```SCION_RENDERING::OpenGLDebugger::breakOnError( true/false )``` and/or ```SCION_RENDERING::OpenGLDebugger::breakOnWarning( true/false )```.
* To ignore a specific warning, call ```SCION_RENDERING::OpenGLDebugger::push( Id )```.
* To reinstate, call ```SCION_RENDERING::OpenGLDebugger::pop( Id )```.
* To set the severity level, call ```SCION_RENDERING::OpenGLDebugger::setSeverityLevel( SCION_RENDERING::OpenGLDebuggerSeverity::Disable/Notification/Low/Medium/High )```.

## Force the discrete GPU on Optimus laptops
In Scion2D/CMakeLists.txt:
* Set the variable ```SCION_OPENGL_FORCE_DISCRETE_GPU``` to ```ON```.

Note: Ids can differ between platforms or drivers.
Note for linux users: this is currently working only on nVidia GPUs.

#### Under Construction
