# Copyright 2025 Xenon Emulator Project

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

project(Xenon)

# This function should be passed a list of all files in a target. It will automatically generate file groups
# following the directory hierarchy, so that the layout of the files in IDEs matches the one in the filesystem.
function(create_target_directory_groups target_name)
    # Place any files that aren't in the source list in a separate group so that they don't get in the way.
    source_group("Other Files" REGULAR_EXPRESSION ".")
    get_target_property(target_sources "${target_name}" SOURCES)
    foreach(file_name IN LISTS target_sources)
        get_filename_component(dir_name "${file_name}" PATH)
        # Group names use '\' as a separator even though the entire rest of CMake uses '/'...
        string(REPLACE "/" "\\" group_name "${dir_name}")
        source_group("${group_name}" FILES "${file_name}")
    endforeach()
endfunction()

find_package(SDL3 3.1.10 CONFIG)

add_subdirectory(third_party)
include_directories(Xenon)

set(Base
    Xenon/Base/SystemDevice.h
    Xenon/Base/Types.h
    Xenon/Base/Version.h
)

set(NAND
    Xenon/Core/NAND/NAND.cpp
    Xenon/Core/NAND/NAND.h
)

set(RAM
    Xenon/Core/RAM/RAM.cpp
    Xenon/Core/RAM/RAM.h
)

set(RootBus
    Xenon/Core/RootBus/RootBus.cpp
    Xenon/Core/RootBus/RootBus.h
    Xenon/Core/RootBus/HostBridge/HostBridge.cpp
    Xenon/Core/RootBus/HostBridge/HostBridge.h
    Xenon/Core/RootBus/HostBridge/PCIe.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/AUDIOCTRLLR/AudioController.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/AUDIOCTRLLR/AudioController.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/EHCI0/EHCI0.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/EHCI0/EHCI0.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/EHCI1/EHCI1.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/EHCI1/EHCI1.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/ETHERNET/Ethernet.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/ETHERNET/Ethernet.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/HDD/HDD.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/HDD/HDD.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/ODD/ODD.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/ODD/ODD.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/OHCI0/OHCI0.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/OHCI0/OHCI0.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/OHCI1/OHCI1.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/OHCI1/OHCI1.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/PCIBridge.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/PCIBridge.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/PCIBridgeConfig.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/PCIDevice.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/SFCX/SFCX.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/SFCX/SFCX.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/SMC/SMC.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/SMC/SMC.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/SMC/HANA_State.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/SMC/SMC_Config.h
    Xenon/Core/RootBus/HostBridge/PCIBridge/XMA/XMA.cpp
    Xenon/Core/RootBus/HostBridge/PCIBridge/XMA/XMA.h
)

set(XCPU
    Xenon/Core/XCPU/Xenon.cpp
    Xenon/Core/XCPU/Xenon.h
    Xenon/Core/XCPU/XenonReservations.cpp
    Xenon/Core/XCPU/XenonReservations.h
    Xenon/Core/XCPU/eFuse.h
    Xenon/Core/XCPU/IIC/IIC.cpp
    Xenon/Core/XCPU/IIC/IIC.h
    Xenon/Core/XCPU/Interpreter/PPC_ALU.cpp
    Xenon/Core/XCPU/Interpreter/Interpreter_Helpers.cpp
    Xenon/Core/XCPU/Interpreter/PPC_MMU.cpp
    Xenon/Core/XCPU/Interpreter/PPC_Instruction.cpp
    Xenon/Core/XCPU/Interpreter/PPC_Instruction.h
    Xenon/Core/XCPU/Interpreter/PPC_LS.cpp
    Xenon/Core/XCPU/Interpreter/PPC_BC.cpp
    Xenon/Core/XCPU/Interpreter/PPC_System.cpp
    Xenon/Core/XCPU/Interpreter/PPCInterpreter.cpp
    Xenon/Core/XCPU/Interpreter/PPCInterpreter.h
    Xenon/Core/XCPU/Interpreter/PPCInternal.h
    Xenon/Core/XCPU/PostBus/PostBus.cpp
    Xenon/Core/XCPU/PostBus/PostBus.h
    Xenon/Core/XCPU/PPU/PPU.cpp
    Xenon/Core/XCPU/PPU/PPU.h
    Xenon/Core/XCPU/PPU/PowerPC.h
)

set(XGPU
    Xenon/Core/XGPU/XGPU.cpp
    Xenon/Core/XGPU/XenosRegisters.h
    Xenon/Core/XGPU/XGPU.h
    Xenon/Core/XGPU/XGPUConfig.h
)

set(Core
    ${NAND}
    ${RAM}
    ${RootBus}
    ${XCPU}
    ${XGPU}
)

add_executable(Xenon
    ${Base}
    ${Core}
    Xenon/Xe_Main.cpp
)

create_target_directory_groups(Xenon)

target_include_directories(Xenon PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Xenon PRIVATE SDL3::SDL3)

if (WIN32)
    target_sources(Xenon PRIVATE Xenon/Xenon.rc)
endif()