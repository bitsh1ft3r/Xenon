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

int main(int argc, char *argv[]) {

  // Load configuration.
  const auto user_dir = Base::FS::GetUserPath(Base::FS::PathType::UserDir);
  Config::loadConfig(user_dir / "xenon_config.toml");

  /*********Jasper motherboard CPU fuses*********/
  eFuses jasperCpuFuses;
  jasperCpuFuses.fuseLine00 = 0xc0ffffffffffffff;
  jasperCpuFuses.fuseLine01 = 0x0f0f0f0f0f0f0ff0;
  jasperCpuFuses.fuseLine02 = 0x0000000000000000;
  jasperCpuFuses.fuseLine03 = 0x2EBCD846F1A7711C;
  jasperCpuFuses.fuseLine04 = 0x2EBCD846F1A7711C;
  jasperCpuFuses.fuseLine05 = 0x8F06C4C7E3EC4961;
  jasperCpuFuses.fuseLine06 = 0x8F06C4C7E3EC4961;
  jasperCpuFuses.fuseLine07 = 0x0000000000000000;
  jasperCpuFuses.fuseLine08 = 0x0000000000000000;
  jasperCpuFuses.fuseLine09 = 0x0000000000000000;
  jasperCpuFuses.fuseLine10 = 0x0000000000000000;
  jasperCpuFuses.fuseLine11 = 0x0000000000000000;

  /*********Xenon motherboard CPU fuses*********/
  eFuses xenonCpuFuses;
  xenonCpuFuses.fuseLine00 = 0xc0ffffffffffffff;
  xenonCpuFuses.fuseLine01 = 0x0f0f0f0f0f0f0ff0;
  xenonCpuFuses.fuseLine02 = 0x0000000000000000;
  xenonCpuFuses.fuseLine03 = 0xF98C9725B2052FE2;
  xenonCpuFuses.fuseLine04 = 0xF98C9725B2052FE2;
  xenonCpuFuses.fuseLine05 = 0x08EE3C57932DCACA;
  xenonCpuFuses.fuseLine06 = 0x08EE3C57932DCACA;
  xenonCpuFuses.fuseLine07 = 0x0000000000000000;
  xenonCpuFuses.fuseLine08 = 0x0000000000000000;
  xenonCpuFuses.fuseLine09 = 0x0000000000000000;
  xenonCpuFuses.fuseLine10 = 0x0000000000000000;
  xenonCpuFuses.fuseLine11 = 0x0000000000000000;

  /*******Xenon XDK motherboard CPU fuses*******/
  eFuses xedkCpuFuses;
  xedkCpuFuses.fuseLine00 = 0xc0ffffffffffffff;
  xedkCpuFuses.fuseLine01 = 0x0f0f0f0f0f0f0ff0;
  xedkCpuFuses.fuseLine02 = 0x0000000000000000;
  xedkCpuFuses.fuseLine03 = 0x8CBA33C6B70BF641;
  xedkCpuFuses.fuseLine04 = 0x8CBA33C6B70BF641;
  xedkCpuFuses.fuseLine05 = 0x2AC5A81E6B41BFE6;
  xedkCpuFuses.fuseLine06 = 0x2AC5A81E6B41BFE6;
  xedkCpuFuses.fuseLine07 = 0x0000000000000000;
  xedkCpuFuses.fuseLine08 = 0x0000000000000000;
  xedkCpuFuses.fuseLine09 = 0x0000000000000000;
  xedkCpuFuses.fuseLine10 = 0x0000000000000000;
  xedkCpuFuses.fuseLine11 = 0x0000000000000000;

  RootBus RootBus;       // RootBus Object
  HostBridge hostBridge; // HostBridge Object
  PCIBridge pciBridge;   // PCIBridge Object

  // SMCCore State for setting diffrent SMC settings.
  Xe::PCIDev::SMC::SMC_CORE_STATE smcCoreState;

  // Memset the SMCCore State.
  memset(&smcCoreState, 0, sizeof(Xe::PCIDev::SMC::SMC_CORE_STATE));

  // Initialize several settings from the struct.
  smcCoreState.currentCOMPort = const_cast<wchar_t *>(L"\\\\.\\COM2");
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
  SFCX sfcx("C://Xbox/xenon_xdk.bin", &pciBridge);
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
  nandDevice.Load("C://Xbox/xenon_xdk.bin");

  // Load 1BL here from given path.
  Xenon xenonCPU(&RootBus, "C://Xbox/1bl.bin", xedkCpuFuses);

  /**************Registers the IIC**************/
  pciBridge.RegisterIIC(xenonCPU.GetIICPointer());

  // CPU Start routine and entry point.
  xenonCPU.Start(0x20000000100);

  return 0;
}
