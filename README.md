# orbiter-sr71r
Orbiter add-on SR71r

## Building

Requires Visual Studio (2022 or later, C++ workload) and CMake 3.26+. The module is 32-bit.

The location of your Orbiter install (the folder containing `Orbitersdk/`) is set in a
`CMakeUserPresets.json` file in the repo root. This file is per-machine and not committed; create it
with your own path:

```json
{
  "version": 6,
  "configurePresets": [
    { "name": "local", "inherits": "x86", "cacheVariables": { "ORBITER_DIR": "D:/Orbiter2024" } }
  ],
  "buildPresets": [
    { "name": "local-debug", "configurePreset": "local", "configuration": "Debug" },
    { "name": "local-release", "configurePreset": "local", "configuration": "Release" }
  ]
}
```

Then configure and build:

```
cmake --preset local
cmake --build --preset local-release
```

In Visual Studio or VS Code, open the folder and pick the `local` preset.

By default the build copies `SR71R.dll` into `ORBITER_DIR/Modules` and the contents of `Orbiter/`
(meshes, textures, config, scenarios) into `ORBITER_DIR`. Add `"SR71R_DEPLOY": "OFF"` to the
preset's `cacheVariables` to skip that.
