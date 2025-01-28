// Copyright 2025 Xenon Emulator Project

#include "Base/Config.h"
#include "Base/Path_util.h"

#include "Core/NAND/NAND.h"
#include "Core/RAM/RAM.h"
#include "Core/RootBus/HostBridge/HostBridge.h"
#include "Core/RootBus/HostBridge/PCIBridge/AUDIOCTRLLR/AudioController.h"
#include "Core/RootBus/HostBridge/PCIBridge/EHCI0/EHCI0.h"
#include "Core/RootBus/HostBridge/PCIBridge/EHCI1/EHCI1.h"
#include "Core/RootBus/HostBridge/PCIBridge/ETHERNET/Ethernet.h"
#include "Core/RootBus/HostBridge/PCIBridge/HDD/HDD.h"
#include "Core/RootBus/HostBridge/PCIBridge/ODD/ODD.h"
#include "Core/RootBus/HostBridge/PCIBridge/OHCI0/OHCI0.h"
#include "Core/RootBus/HostBridge/PCIBridge/OHCI1/OHCI1.h"
#include "Core/RootBus/HostBridge/PCIBridge/PCIBridge.h"
#include "Core/RootBus/HostBridge/PCIBridge/SFCX/SFCX.h"
#include "Core/RootBus/HostBridge/PCIBridge/SMC/SMC.h"
#include "Core/RootBus/HostBridge/PCIBridge/XMA/XMA.h"
#include "Core/RootBus/RootBus.h"
#include "Core/XCPU/Xenon.h"
#include "Core/XGPU/XGPU.h"

eFuses getFuses(const std::filesystem::path& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return { 0x9999999999999999 };
  }
  std::vector<std::string> fusesets;
  std::string fuseset;
  while (std::getline(file, fuseset)) {
    if (size_t pos = fuseset.find(": "); pos != std::string::npos) {
      fuseset = fuseset.substr(pos + 2);
    }
    fusesets.push_back(fuseset);
  }

  eFuses cpuFuses;
  cpuFuses.fuseLine00 = strtoull(fusesets[0].data(), 0, 16);
  cpuFuses.fuseLine01 = strtoull(fusesets[1].data(), 0, 16);
  cpuFuses.fuseLine02 = strtoull(fusesets[2].data(), 0, 16);
  cpuFuses.fuseLine03 = strtoull(fusesets[3].data(), 0, 16);
  cpuFuses.fuseLine04 = strtoull(fusesets[4].data(), 0, 16);
  cpuFuses.fuseLine05 = strtoull(fusesets[5].data(), 0, 16);
  cpuFuses.fuseLine06 = strtoull(fusesets[6].data(), 0, 16);
  cpuFuses.fuseLine07 = strtoull(fusesets[7].data(), 0, 16);
  cpuFuses.fuseLine08 = strtoull(fusesets[8].data(), 0, 16);
  cpuFuses.fuseLine09 = strtoull(fusesets[9].data(), 0, 16);
  cpuFuses.fuseLine10 = strtoull(fusesets[10].data(), 0, 16);
  cpuFuses.fuseLine11 = strtoull(fusesets[11].data(), 0, 16);
  
  return cpuFuses;
}

int main(int argc, char *argv[]) {

  // Load configuration.
  const auto user_dir = Base::FS::GetUserPath(Base::FS::PathType::UserDir);
  Config::loadConfig(user_dir / "xenon_config.toml");

  RootBus RootBus;       // RootBus Object
  HostBridge hostBridge; // HostBridge Object
  PCIBridge pciBridge;   // PCIBridge Object

  // SMCCore State for setting diffrent SMC settings.
  Xe::PCIDev::SMC::SMC_CORE_STATE smcCoreState;

  // Memset the SMCCore State.
  memset(&smcCoreState, 0, sizeof(Xe::PCIDev::SMC::SMC_CORE_STATE));

  // Initialize several settings from the struct.
  smcCoreState.currentCOMPort = "\\\\.\\COM2";
  smcCoreState.currAVPackType = Xe::PCIDev::SMC::SMC_AVPACK_TYPE::HDMI_NO_AUDIO;
  smcCoreState.currPowerOnReas =
      (Xe::PCIDev::SMC::SMC_PWR_REAS)Config::smcPowerOnType();
  smcCoreState.currTrayState = Xe::PCIDev::SMC::SMC_TRAY_STATE::SMC_TRAY_CLOSE;

  // SMCCore Object.
  Xe::PCIDev::SMC::SMCCore smcCore(&pciBridge, &smcCoreState);

  Xe::PCIDev::ETHERNET::ETHERNET ethernet;            // Ethernet Object
  Xe::PCIDev::AUDIOCTRLR::AUDIOCTRLR audioController; // Audio Controller Object
  Xe::PCIDev::OHCI0::OHCI0 ohci0;                     // OHCI0 Object
  Xe::PCIDev::OHCI1::OHCI1 ohci1;                     // OHCI1 Object
  Xe::PCIDev::EHCI0::EHCI0 ehci0;                     // EHCI0 Object
  Xe::PCIDev::EHCI1::EHCI1 ehci1;                     // EHCI1 Object

  // Create the Secure Flash Controller for Xbox Device, and load the Nand dump
  // for emulation.
  SFCX sfcx("C:/Xbox/nand.bin", &pciBridge);
  RAM ram;
  XMA xma;
  ODD odd(&pciBridge, &ram);
  HDD hdd(&pciBridge);
  NAND nandDevice;

  Xe::Xenos::XGPU xenos(&ram);

  // Initialize all devices
  RootBus.Init();

  ohci0.Initialize("OHCI0", OHCI0_DEV_SIZE);
  ohci1.Initialize("OHCI1", OHCI1_DEV_SIZE);
  ehci0.Initialize("EHCI0", OHCI0_DEV_SIZE);
  ehci1.Initialize("EHCI1", OHCI1_DEV_SIZE);
  audioController.Initialize("AUDIOCTRLR", AUDIO_CTRLR_DEV_SIZE);
  ethernet.Initialize("ETHERNET", ETHERNET_DEV_SIZE);
  sfcx.Initialize("SFCX", SFCX_DEV_SIZE);
  xma.Initialize("XMA", XMA_DEV_SIZE);
  odd.Initialize("CDROM", ODD_DEV_SIZE);
  hdd.Initialize("HDD", HDD_DEV_SIZE);
  smcCore.Initialize("SMC", SMC_DEV_SIZE);

  /*******Add PCI devices*******/
  pciBridge.addPCIDevice(&ohci0);
  pciBridge.addPCIDevice(&ohci1);
  pciBridge.addPCIDevice(&ehci0);
  pciBridge.addPCIDevice(&ehci1);
  pciBridge.addPCIDevice(&audioController);
  pciBridge.addPCIDevice(&ethernet);
  pciBridge.addPCIDevice(&sfcx);
  pciBridge.addPCIDevice(&xma);
  pciBridge.addPCIDevice(&odd);
  pciBridge.addPCIDevice(&hdd);
  pciBridge.addPCIDevice(&smcCore);

  // Register the Xenos GPU and the PCIBridge
  hostBridge.RegisterXGPU(&xenos);
  hostBridge.RegisterPCIBridge(&pciBridge);

  /*****************Initializes the NAND and the RAM*****************/
  nandDevice.Initialize("NAND", NAND_START_ADDR, NAND_END_ADDR, true);
  ram.Initialize("RAM", RAM_START_ADDR, RAM_START_ADDR + RAM_SIZE, false);

  /**Adds the HostBridge, NAND and RAM**/
  RootBus.AddHostBridge(&hostBridge);
  RootBus.AddDevice(&nandDevice);
  RootBus.AddDevice(&ram);

  // NAND Load Path.
  nandDevice.Load("C:/Xbox/nand.bin");

  // Load 1BL here from given path.
  eFuses cpuFuses = getFuses(Config::fusesPath());
  if (cpuFuses.fuseLine00 == 0x9999999999999999)
  {
    std::cerr << "No valid fuses.txt!" << std::endl;
  }
  Xenon xenonCPU(&RootBus, "C:/Xbox/1bl.bin", cpuFuses);

  /**************Registers the IIC**************/
  pciBridge.RegisterIIC(xenonCPU.GetIICPointer());

  // CPU Start routine and entry point.
  xenonCPU.Start(0x20000000100);

  return 0;
}
