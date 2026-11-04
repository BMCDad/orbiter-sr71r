# Legacy code

Nothing in this folder is compiled. It is kept for reference, mainly for the deferred autopilot work.

- The `.cpp` files here target an older `bc_orbiter` API. Some share a name with a current header in
  `SR71R/systems/` (`HUD.cpp` and `HUD.h`, for example), but they are not its implementation: the
  current classes are implemented in their headers.
- Includes in this folder were not updated when the source tree was reorganized. Anything brought back
  needs porting to the current API, new include paths, and an entry in `CMakeLists.txt`.
- `tools/` holds the old Visual Studio packaging script and Orbiter config files. CMake now handles
  building and deploying.
