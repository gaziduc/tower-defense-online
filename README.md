# tower-defense-online

## How to build to project

```shell
git clone https://github.com/gaziduc/tower-defense-online
```
First of all, you need the 'conan' dependency manager (you can get it at https://conan.io/downloads) to get the 'miniupnpc' library. Don't forget to add it to the PATH environment variable.

Download a copy of the SDL2, SDL2_image, SDL2_mixer, SDL2_net and SDL2_ttf development libraries at:

```
https://github.com/libsdl-org/SDL/releases
https://github.com/libsdl-org/SDL_image/releases
https://github.com/libsdl-org/SDL_mixer/releases
https://github.com/libsdl-org/SDL_net/releases
https://github.com/libsdl-org/SDL_ttf/releases
```

Unzip it and put them in the root folder, next to resources/ and src/

If necessary, modify the libraries paths in CMakeLists.txt, according to the libs version you downloaded...

```cmake
set(CMAKE_PREFIX_PATH
        ${CMAKE_SOURCE_DIR}/SDL2-2.30.3/cmake
        ${CMAKE_SOURCE_DIR}/SDL2_image-2.8.2/cmake
        ${CMAKE_SOURCE_DIR}/SDL2_mixer-2.8.0/cmake
        ${CMAKE_SOURCE_DIR}/SDL2_net-2.2.0/cmake
        ${CMAKE_SOURCE_DIR}/SDL2_ttf-2.22.0/cmake
)
```
After compiling tower-defense-online, copy the resources/ folder into cmake-build-debug or cmake-build-release.

Also, for each library, copy the .dll file(s) which are in the bin/ folder, into cmake-build-debug/ or cmake-build-release/ depending on your target.

Finally, for the SDL2_image library, you have to download the library, not just the development library. Then copy the following .dll files from the VisualC/external/optional/x64 (or VisualC/external/optional/x86 if you want 32 bits) folder:
```
libwebp-7.dll
libwebpdemux-2.dll
```

into the cmake-build-debug/ or cmake-build-release/ folder.

## Release binaries

Don't forget to add the .dll files that are in the mingw folder, e.g.: C:\Program Files\JetBrains\CLion 2024.1\bin\mingw\bin, to the release folder.