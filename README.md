
![logo](https://github.com/dwjclark11/Scion2D/assets/63356975/cc26ef0c-b190-4af9-9ac1-cac8cd9f2ed5)

## Introduction
The goal for **Scion2D** is to make a simple 2D game engine where I can teach what I am learning through my [youtube channel](https://www.youtube.com/playlist?list=PL3HUvSWOJR7XRDwVVQqqWO-zyyscb8L-v). 
This is for educational purposes and to constantly learn more about programming and gaming everyday. There are definitely better 
engines out there; however, I find it more fun to try to build your own, learn from others, and grow as a developer.

![scion2d](https://github.com/dwjclark11/Scion2D/assets/63356975/ba9c466d-780d-4217-bf9e-5ed0dc4981d9)

## Check the Docs
The documentation is still a work in progress; however, you can check it out here:
[Scion2D Docs](https://dwjclark11.github.io/Scion2D_Docs/)

# Build
----
Requires [CMake 3.26](https://cmake.org/) and [vcpkg](https://github.com/microsoft/vcpkg)
#### Get VCPKG:
```ps
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.bat --disableMetrics
```
#### Make sure the following environment variables are set:
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
    
#### Install dependencies 
- Windows
    ```
    vcpkg install fmt glm entt glad soil2 sdl2 sdl2-mixer box2d lua sol2 stb imgui[docking-experimental,opengl3-binding,sdl2-binding]
    ```
- Linux[debian based]
    ```
    vcpkg install fmt glm entt glad soil2 sdl2[alsa] sdl2-mixer box2d lua sol2 stb imgui[docking-experimental,opengl3-binding,sdl2-binding]
    ```
    ```
    sudo apt install python-jinja2 autoconf automake libtool pkg-config libibus-1.0-dev`
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
        sudo apt install build-essentials
        ```

#### Clone the repository 
```
git clone https://github.com/dwjclark11/Scion2D.git
cd Scion2D
cmake -S . -B build
```
 
* Also, in the main.lua file for the editor, comment out the loaded assets and files that don't exist. They will just error out.

#### Add a default font
* The engine needs a default font. Use the code below and add the font of your choice.
* We will be adding default font to the engine shortly that will be automatically loaded.
* In the ```Application.cpp```, change the loading of the fonts to a .ttf file at a given location for your engine.

```cpp
// Temp Load pixel font // Change this to the font you use and its path
if (!assetManager->AddFont("pixel", "./assets/fonts/pixel.ttf"))
{
  SCION_ERROR("Failed to load pixel font!");
  return false;
}
``` 
#### Under Construction
