
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
./vcpkg/bootstrap-vcpkg.bat
```
#### Make sure the following environment variables are set:
```
VCPKG_ROOT=[path_to_vcpkg]
VCPKG_DEFAULT_TRIPLET=x64-windows
```

#### Install dependencies 
```
./vcpkg install glm entt sdl2[alsa] sdl2-mixer box2d lua sol2
```
  * Linux `apt-get install python-jinja2`

#### Clone the repository 
```
git clone https://github.com/dwjclark11/Scion2D.git`
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
