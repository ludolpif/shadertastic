# Building shadertastic

## Manual build for ubuntu 22.04

As root:
```sh
# Build tools (from .github/scripts/.Aptfile)
apt install cmake ccache git jq ninja-build pkg-config
# OBS build deps (from .github/scripts/utils.zsh/setup_linux)
apt install gcc g++
add-apt-repository --yes ppa:obsproject/obs-studio
apt update
apt install build-essential libgles2-mesa-dev obs-studio
apt install qt6-base-dev:amd64 libqt6svg6-dev:amd64 qt6-base-private-dev:amd64
# This plugin build deps
apt install libzip-dev #TODO
```
As user :
```sh
git clone -b v<branch-or-tag> --recurse-submodules <this repo url> shadertastic
cd shadertastic/
cmake --preset linux-x86_64
cmake #TODO check the .build.zsh calls
```

## Informations learned on the road

- OBS Studio uses GitHub CI actions for MacOS, Windows and Ubuntu builds
    - this is made with `.yaml` configuration and `.zsh` scripts that basically triggers `cmake` commands
    - plugins can / should use same infrastructure
    - with github CI, `obsproject/obs-plugintemplate` or variants from famous OBS plugin makers have used to 
        - do a full `git clone` of obs-studio project
        - do a full `git fetch --tags` (which costs 50 seconds because 15 years of project history)
        - build OBS before build plugin (earch time ! with or without a working ccache file restore before build)
    - OBS 30 was a first release with a big cleanup and refreshed build system (cmake side and Github CI side)
        - old git `CI/` folder and shell scripts are gone
        - now, it's only with `.github` folder and CMake "scripts" (entrypoint is `CMakeLists.txt`)
    - recent plugin-template will 
        - download obs source from publish zip file, without git history
        - build plugin against OBS headers and sources, without building OBS itself

- Github CI configuration: if you are new to that, follow this to have a basic understanding of is going on
    - `.github/workflow/(push|pr-pull).yaml` are entry points
    - they contains a `build-project` jobs that uses `.github/workflow/build-project.yaml`
    - `build-project.yaml` defines jobs named `macos-build`, `ubuntu-build`, `windows-build`
    - they start containers on the targeted OS
    - containers will run `.github/actions/build-plugin` (then `package-plugin` action) with correct `inputs`
    - `.github/actions/build-plugin.action.yaml` will exec
        - zsh scripts on MacOS or Linux `.github/scripts/build-linux` (symlink to `.github/scripts/.build.zsh`)
        - or PowerShell script on windows `.github/scripts/Build-Windows.ps1`
    - `.build.zsh` accepts a ``--debug`` argument to show issued CMake commands, and action set it if `$RUNNER_DEBUG` is set
    - this `$RUNNER_DEBUG` is set by github if the checkbox "Enable debug logging" is checked on "Re-run all jobs" button/popup from a workflow summary
    - For Github workflow evaluation debug mode (the YaML part not the zsh part), you can set in repo Settings/General/Secrets and variables/Actions, tab "Variables", section "Repository variables" `ACTIONS_RUNNER_DEBUG=true` (URL like: https://github.com/<project>/<repo>/settings/variables/actions)
    - see: https://docs.github.com/en/actions/monitoring-and-troubleshooting-workflows/troubleshooting-workflows/enabling-debug-logging#enabling-runner-diagnostic-logging
    - this debug is not shown on the web interface, you have to on the action run summary (and not workflow summary), use the setting icon to click "Download log archive" or open `running-diagnostic-log` folder in the zip.

- CMake has phases a bit like `./configure`, `make`, `make install`
    - but it is more configure, generate (input files for native build system)
    - and it gives wrappers and helpers around the build and packaging actions
- CMake project configuration is usually done manually with cmake-gui 
    - (it needs the path of the root of this repository local copy (created by `git clone`)
    - user configurable options are defined with a default value in various `CMakeLists.txt` or included `.cmake` files
    - choosen configured values for a particular build are known as CMake "Cache variables" as they are persistent if you restart the same build from the same environnement again
    - CMake cache variables are not only user options, they are also the result of envrionnement detection and auto-enabling features
    - `CMakeCache.txt` is the file CMake uses to keep cache variables values. It is not commited in git repository (on purpose)
    - all variables aren't cache variables as https://cmake.org/cmake/help/v3.0/command/set.html says:

In CMake there are two types of variables: normal variables and cache variables. Normal variables are meant for the internal use of the script (just like variables in most programming languages); they are not persisted across CMake runs. Cache variables (unless set with INTERNAL) are mostly intended for configuration settings where the first CMake run determines a suitable default value, which the user can then override, by editing the cache with tools such as ccmake or cmake-gui. Cache variables are stored in the CMake cache file, and are persisted across CMake runs.

- CMake since 3.19 (2022) have `CMakePresets.json`
    - allows to set CMake cache values (user option ones) from various presets for various typical build environnements
    - it is sweat to make un unattended configuration phase for all CI environement
    - basically github actions will call `cmake --preset XXX-ci-x64` where XXX is one of windows, linux, macos


## Build configuration bootstrap from 2024-09

- Started from https://github.com/obsproject/obs-plugintemplate commit e3688b7 (was latest)
- Filled in plugin basic infos in buildspec.json
- Added `if: false` on macos-build build in `.github/workflows/build-project.yaml` 
- Added `if: true` on windows-build and linux-build to let disable them while scaffolding
- Renamed from `src/plugin-main.c` to `src/plugin-main.cpp`, filled in the start of the file (comments)
- Edited root `CMakeLists.txt` to define `GLOB sources_CPP` and use in `target_sources()`
- Edited `CMakePresets.json` to compare build times with and without `ENABLE_FRONTEND_API` and `ENABLE_QT`
    - on ubuntu enabling one of them (or the two) cost around 3.5 minutes in `apt install <many-deps>`
    - will try to make them optionnal for shadertastic (at the cost of missing Tools/Shadertastic QT menu+dialog)
    - set `CMAKE_COMPILE_WARNING_AS_ERROR` to false for windows-ci-x64
        - rationale: MSVC find non-problematic warnings in `flexc++` generated code for upcoming `shadership` tool
        - editing the generated code seems not a good choice even if it's for `#pramga` something

