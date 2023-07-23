
![Scion2D_Logo](https://github.com/dwjclark11/Scion2D/assets/63356975/f47edca1-4a4f-4948-8d40-09bbb6bc6e3f)

## Introduction
The goal for **Scion2D** is to make a simple 2D game engine where I can teach what I am learning through my [youtube channel](https://www.youtube.com/playlist?list=PL3HUvSWOJR7XRDwVVQqqWO-zyyscb8L-v). 
This is for educational purposes and to constantly learn more about programming and gaming everyday. There are definitely better 
engines out there; however, I find it more fun to try to build your own, learn from others, and grow as a developer.

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
* GLAD - This will be build as a static library
* SOIL - Soil is the texture loading library we will be using - Built as a static library
* ImGui -
* SCION_EDITOR - 
* SCION_RENDERING -
* SCION_UTILITIES -
* SCION_WINDOW -
* SCION_PHYSICS -
* SCION_CORE -
* SCION_LOGGER -
* SCION_SOUNDS
* SCION_FILESYSTEM

Readme is still under construction
