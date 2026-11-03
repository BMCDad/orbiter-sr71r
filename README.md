# orbiter-sr71r
Orbiter add-on SR71r

## Development setup (VS Code)

You need:

- [VS Code](https://code.visualstudio.com/) with the **C/C++** and **CMake Tools** extensions (VS Code
  offers to install both when you open the folder).
- The MSVC compiler from [Build Tools for Visual Studio 2026](https://visualstudio.microsoft.com/downloads/),
  with the **Desktop development with C++** workload.
- CMake 3.26 or later (CMake Tools can use the copy that comes with the Build Tools).
- An Orbiter 2024 install that includes `Orbitersdk/`. Make sure Orbiter runs correctly from that
  folder before you start.

### 1. Tell the build where Orbiter is

Create a file named `local.cmake` in the repo root with the path to your Orbiter folder (the one that
contains `Orbitersdk\`). Use forward slashes:

```cmake
set(ORBITER_DIR "C:/Orbiter")
```

`local.cmake` is not committed, so each clone of the repo can point at a different Orbiter install.
When CMake configures, it also generates `.vscode/launch.json` (not committed) from
`.vscode/launch.json.in` with that path filled in. To change the debug settings, edit the template,
not the generated file.

### 2. Build

Open the repo folder in VS Code. If CMake Tools asks for a configure preset, pick **Orbiter (32-bit MSVC)**.
In the CMake side bar (or the status bar), the build preset is **Debug** or **Release**.

Build with **Ctrl+Shift+B**. The build output goes to `build/`. Each build also copies `SR71R.dll` (and
its `.pdb`) into `Modules\` in your Orbiter folder, and copies the contents of `Orbiter/` (meshes,
textures, config, scenarios) into the Orbiter folder.

### 3. Debug

Press **F5** (**Debug SR71R in Orbiter**). This builds the module, then starts Orbiter under the
debugger. Pick a scenario that uses the SR71R, and your breakpoints bind once the vessel loads.

The debugger launches `Modules\Server\Orbiter.exe`, not `orbiter_ng.exe`. `orbiter_ng.exe` only starts
the server executable as a child process, which the debugger does not follow. To debug an Orbiter
that's already running, use **Attach to Orbiter** instead.

## Building from the command line

With `local.cmake` in place:

```
cmake --preset orbiter
cmake --build --preset release
```

## Troubleshooting

- **`ORBITER_DIR (...) is not an Orbiter install with the SDK`**: `local.cmake` is missing, or its path
  doesn't point at a folder that contains `Orbitersdk\`. Fix the file and build again; CMake
  reconfigures on its own when `local.cmake` changes.
- **F5 says `launch.json` is missing or points at the wrong folder**: the file is generated when CMake
  configures. Run **CMake: Configure** from the command palette (Ctrl+Shift+P).
