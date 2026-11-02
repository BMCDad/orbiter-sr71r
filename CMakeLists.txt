cmake_minimum_required(VERSION 3.26)
project(SR71R LANGUAGES CXX RC)

if(NOT MSVC)
  message(FATAL_ERROR "SR71R must be built with MSVC")
endif()
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR "Orbiter modules are 32-bit: configure with -A Win32 (or use the presets)")
endif()

set(ORBITER_DIR "" CACHE PATH "Orbiter install root (contains Orbitersdk/ and Modules/)")
if(NOT EXISTS "${ORBITER_DIR}/Orbitersdk/include/OrbiterAPI.h")
  message(FATAL_ERROR "ORBITER_DIR ('${ORBITER_DIR}') is not an Orbiter install with the SDK. "
                      "Set it in a CMakeUserPresets.json configure preset that inherits 'x86' (see README.md).")
endif()

option(SR71R_DEPLOY "Copy the module and the Orbiter/ data folder into ORBITER_DIR after building" ON)

find_library(ORBITER_LIB orbiter PATHS "${ORBITER_DIR}/Orbitersdk/lib" NO_DEFAULT_PATH REQUIRED)
find_library(ORBITERSDK_LIB Orbitersdk PATHS "${ORBITER_DIR}/Orbitersdk/lib" NO_DEFAULT_PATH REQUIRED)

# Sources are listed explicitly: SR71R/ contains older .cpp files that are not part of the build.
set(SR71R_SOURCES
  SR71R/LiftCoeff.cpp
  SR71R/SR71r.cpp
  SR71R/SR71Vessel.cpp
)

set(SR71R_RESOURCES
  SR71R/SR71R.rc
)

set(SR71R_HEADERS
  SR71R/LiftCoeff.h
  SR71R/MfdMets.h
  SR71R/resource.h
  SR71R/ShipMets.h
  SR71R/SR71Barrel.h
  SR71R/SR71Button.h
  SR71R/SR71Gauge.h
  SR71R/SR71Light.h
  SR71R/SR71MFDs.h
  SR71R/SR71MouseClick.h
  SR71R/SR71r2DMain_mesh.h
  SR71R/SR71r2DRight_mesh.h
  SR71R/SR71r_common.h
  SR71R/SR71r_mesh.h
  SR71R/SR71rVC_mesh.h
  SR71R/SR71Toggle.h
  SR71R/SR71Transform.h
  SR71R/SR71Vessel.h
  SR71R/SR71WrapGauge.h
  SR71R/StdAfx.h
)

set(SR71R_COMPONENTS
  SR71R/AirBrake.h
  SR71R/Airspeed.h
  SR71R/Altimeter.h
  SR71R/APU.h
  SR71R/Avionics.h
  SR71R/Canopy.h
  SR71R/CargoBay.h
  SR71R/ClockTimer.h
  SR71R/FuelCell.h
  SR71R/HoverEngines.h
  SR71R/HSI.h
  SR71R/HUD.h
  SR71R/HydrogenTank.h
  SR71R/LandingGear.h
  SR71R/Lights.h
  SR71R/MFDs.h
  SR71R/NavModes.h
  SR71R/OxygenTank.h
  SR71R/PowerSystem.h
  SR71R/PropulsionController.h
  SR71R/RCSSystem.h
  SR71R/RetroEngines.h
  SR71R/SurfaceController.h
)

set(SR71R_INTERFACES
  SR71R/IAvionics.h
  SR71R/IConsumable.h
  SR71R/IFuelCellSystem.h
  SR71R/IFuelSystem.h
  SR71R/IHydraulicProvider.h
  SR71R/IHydrogenProvider.h
  SR71R/ILiquidOxygenProvider.h
  SR71R/IPowerProvider.h
)

set(BC_ORBITER_HEADERS
  bc_orbiter/AnimatedValue.h
  bc_orbiter/AnimationGroup.h
  bc_orbiter/AnimationUpdate.h
  bc_orbiter/IUIControl.h
  bc_orbiter/MeshTools.h
  bc_orbiter/PanelMouseEvent.h
  bc_orbiter/PanelTextureTranslate.h
  bc_orbiter/Tools.h
  bc_orbiter/Types.h
  bc_orbiter/VCMouseEvent.h
  bc_orbiter/VCTextureTranslate.h
  bc_orbiter/Vessel.h
)

add_library(SR71R SHARED
  ${SR71R_SOURCES} ${SR71R_RESOURCES}
  ${SR71R_HEADERS} ${SR71R_COMPONENTS} ${SR71R_INTERFACES} ${BC_ORBITER_HEADERS}
)

# IDE folders, matching the old SR71R.vcxproj.filters.
source_group("Source Files" FILES ${SR71R_SOURCES})
source_group("Resource Files" FILES ${SR71R_RESOURCES})
source_group("Header Files" FILES ${SR71R_HEADERS})
source_group("Components" FILES ${SR71R_COMPONENTS})
source_group("Interfaces" FILES ${SR71R_INTERFACES})
source_group("bc_orbiter" FILES ${BC_ORBITER_HEADERS})

target_compile_features(SR71R PRIVATE cxx_std_20)
# No _DEBUG in Debug: the Orbiter SDK libs are built against the release CRT (/MD), and _DEBUG
# would pull in the debug STL (msvcprtd) and fail the RuntimeLibrary mismatch check.
target_compile_definitions(SR71R PRIVATE
  WIN32 _WINDOWS _USRDLL SR71R_EXPORTS UNICODE _UNICODE
  $<$<CONFIG:Release>:NDEBUG>
)
target_compile_options(SR71R PRIVATE
  /W3 /permissive /wd26812
  $<$<CONFIG:Release>:/Oi /Gy>
)
# Only the SDK: source includes are relative to the including file, so the repo root must not
# be on the path (it holds a stale copy of SR71r_mesh.h).
target_include_directories(SR71R PRIVATE "${ORBITER_DIR}/Orbitersdk/include")
target_precompile_headers(SR71R PRIVATE SR71R/StdAfx.h)

target_link_libraries(SR71R PRIVATE ${ORBITER_LIB} ${ORBITERSDK_LIB})
target_link_options(SR71R PRIVATE
  /DEBUG
  $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
)

# Both configurations use the release DLL CRT to match Orbitersdk.lib (MD_DynamicRelease).
set_target_properties(SR71R PROPERTIES
  MSVC_RUNTIME_LIBRARY MultiThreadedDLL
  MSVC_DEBUG_INFORMATION_FORMAT ProgramDatabase
  INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
)

if(SR71R_DEPLOY)
  add_custom_command(TARGET SR71R POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ORBITER_DIR}/Modules"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SR71R> $<TARGET_PDB_FILE:SR71R> "${ORBITER_DIR}/Modules/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/Orbiter" "${ORBITER_DIR}"
    COMMENT "Deploying SR71R to ${ORBITER_DIR}"
    VERBATIM
  )
endif()
