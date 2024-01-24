
![logo](https://github.com/dwjclark11/Scion2D/assets/63356975/cc26ef0c-b190-4af9-9ac1-cac8cd9f2ed5)

## Introduction
The goal for **Scion2D** is to make a simple 2D game engine where I can teach what I am learning through my [youtube channel](https://www.youtube.com/playlist?list=PL3HUvSWOJR7XRDwVVQqqWO-zyyscb8L-v). 
This is for educational purposes and to constantly learn more about programming and gaming everyday. There are definitely better 
engines out there; however, I find it more fun to try to build your own, learn from others, and grow as a developer.

## Build
* Under construction
* Eventually I would like to do a cmake or premake setup for the build; however, if you are building from the ```.sln``` make sure that you set the ```start-up project``` to the ```SCION_EDITOR```. **Right click on project -> Set as Startup Project.**
* Also, in the main.lua file for the editor, comment out the loaded assets and files that don't exist. They will just error out.
* In the ```Application.cpp```, change the loading of the fonts to a .ttf file at a given location for your engine.
```cpp
		// Temp Load pixel font // Change this to the font you use and its path
		if (!assetManager->AddFont("pixel", "./assets/fonts/pixel.ttf"))
		{
			SCION_ERROR("Failed to load pixel font!");
			return false;
		}
```
* I will start working on the cmake/premake soon. Thanks.


## Dependencies
**SDL2**
  * We are using SDL2 for the windowing and input functionality for our engine. SDL is easy to use and has many features.
  
**OpenGL**
  * We will be doing all of our graphics with OpenGL, maybe change to vulkan as my skills with graphics increase.

**Glad**
  * We will be using Glad as our OpenGL Loader library.
  
**ENTT**
  * For our Entity Component System, we will be using the wonderful ENTT library.
  * We will also be making use of ENTT's meta reflection functionality.
  
**Box2D**
  * For our physics, we will be creating a nice wrapper around Box2D.
  
**Lua**
  * The scripting language of choice for us will be Lua.
  * Lua is easy to use and understand, I also have more experience with setting lua up and using it.
  
**Sol**
  * We will be using the wonderful Sol3 library to bind our lua functions, user types, etc.
  * Sol is very easy to use and works great.


The Engine will be broken down into multiple projects. The reason for this is that we will not need the same things in the editor 
that we will need in the Game Engine. They both will have some shared libraries; however, editor specific functionality will not be 
used inside an actual exported game. 

The projects will be as follows:
* ***GLAD***
  * This will be build as a static library
* ***SOIL***
  * Soil is the texture loading library we will be using - Built as a static library
* ***ImGui***
  * Dear ImGui will be used in the editor to allow us to have docking, file management, and it is really easy to use.
* ***SCION_EDITOR***
  * The editor will have a built in tilemap editor, scene hierarchy for game objects/entities, and allow us to export the game.
* ***SCION_RENDERING***
  * Contains all the necessary graphics classes - Textures, shaders, framebuffer, batch renderer, camera, and more.
* ***SCION_UTILITIES***
  * Utility classes and functions that will be useful everywhere. 
* ***SCION_WINDOW***
  * Windowing uses SDL to create the window, also to handle inputs - Keyboard, mouse, and gamepad.
* ***SCION_PHYSICS***
  * Box2D will be used for our physics. This will contain wrapper classes around box2D bodies, world, etc.
* ***SCION_CORE***
  * The core will be what binds everything together. ECS wrapper classes, Asset management, Scripting, will all be in the core.
* ***SCION_LOGGER***
  * The logger to be used throughout the engine to log errors, information, and warnings.
* ***SCION_SOUNDS***
  * This will have wrapper classes around SDL Mixer for Sound FX and Music
* ***SCION_FILESYSTEM***
  * TODO:  

Readme is still under construction
