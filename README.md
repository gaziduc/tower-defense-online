# tower-defense-online

## How to build to project

```
git clone https://github.com/gaziduc/tower-defense-online
```

Download a copy of the SDL2 and SDL2_image development libraries at:

```
https://github.com/libsdl-org/SDL/releases
https://github.com/libsdl-org/SDL_image/releases
```

Unzip it and put them in the root folder, next to resources/ and src/

If necessary, modify the libraries paths in CMakeLists.txt, according to the libs version you downloaded, and the architecture...

```cmake
set(SDL2_PATH "SDL2-2.30.2/x86_64-w64-mingw32")
set(SDL2_IMAGE_PATH "SDL2_image-2.8.2/x86_64-w64-mingw32")
```
After compiling, for each library, copy the .dll file(s) which are in the bin/ folder, into cmake-build-debug or cmake-build-release depending on your target.