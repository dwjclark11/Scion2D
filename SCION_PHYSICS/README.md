
# SCION PHYSICS 

Contains all of the necessary physics code and wrappers for the engine.
This library utilizes Box2d version #2.41. The latest version of box2d is
version 3.1 which uses a C API that differs to what we already have in place.

Eventually we will update to use the latest version of box2d; however, for now,
if you are using VCPKG and have the 3.1 version installed, you will need to
remove that install and add an ```overlay-port``` to install the correct version.

----

### VCPKG Overlay-port Instructions
There are two files that will need to be changed inside of your vcpkg ```ports``` folder.
You can find those files here: ```[your_storage_path]\Scion2D\thirdparty\vcpkg\port_files\box2d```.

* Open vcpkg in the cmdline and remove your current box2d, if installed.

```vcpkg remove box2d```

* From there you need to go to ```[your_path_to_vcpkg_]\vcpkg\ports\box2d``` and replace the files there
with the files stated above.

* Once the files have been changed, go back to the cmdline and use the following cmd:

```vcpkg install box2d --overlay-ports=[your_path_to_vcpkg]\vcpkg\ports\box2d```

This should install the correct version of box2d that we are currently using in the engine.

You can also verify that the correct version was installed by listing all of the packages you have installed
and check the version.

Use this cmd for that in the cmdline:

```vcpkg list```

