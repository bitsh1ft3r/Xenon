<h1 align="center">
  <br>
  <a href="https://discord.gg/TDvbdXRRQ6"><img src="https://github.com/bitsh1ft3r/Xenon/blob/main/Xenon/Assets/images/Xenon.png" width="250"></a>
  <br>
  <b>Xbox 360 Low Level Emulator Research Project</b>
  <br>
</h1>

## Join our Discord
 <a href="https://discord.gg/TDvbdXRRQ6">
        <img src="https://img.shields.io/discord/1289565618957193217?color=5865F2&label=Xenon Discord&logo=Discord&logoColor=white">
 </a>

## Build Status
[![Build](https://github.com/bitsh1ft3r/Xenon/actions/workflows/build.yml/badge.svg)](https://github.com/bitsh1ft3r/Xenon/actions/workflows/build.yml)

## Current State

***Kernel*** (`xboxkrnl.exe`)
- Proper boot (skipping `HW_INIT` CB Bootloader step) is functional up to the latest Dev kernel: 2.0.17489.0. Kernel boots and reaches INIT_SECURITY boot stage, soon after main thread dies because of unimplemented/improper SFCX routines.
- Kernel debugging via WindDBG is implemented and works as expected with some minor glitches.

***Xell Reloaded***
![Xell Reloaded Second Stage.](/Xenon/Assets/images/Xell_Second_Stage.png)

Xell is successfully loaded from NAND and stages 1 and 2 execute.
Linux executables or .elfs are probably runnable, but the lack of a proper USB/HDD/ODD implementation makes it imposible to test ATM.

No binaries are provided so far, so you must build it yourself, you'll also need your own dump of the **1BL**, and a 16/64Mb Retail/XDK NAND dump with its own set of matching CPU key/eFuses.

### Whats implemented:
- **XCPU interpreter:** Featuring a custom MMU and a pretty good amount of the PowerPC 64 ISA. Based on the CELL-BE as the PPU inside the Xbox 360 CPU is the same as that, with a few changes/additions. Everything is being implemented as needed, as I find this method more easy for debugging code/instructions.
- **System Bus:**
 ***- Parts of the HostBridge and the BIU.***
- **PCI Bus:**
 ***- Several PCI devices, mostly stubs ATM. PCI Interrupt routing and proper PCI Bus implementation and auto configuration of PCI Devices via PCI BAR's and registers is also implemented.***
- **NAND:** Simple nand loading system, able to load 16/64Mb Jasper/Trinity/Corona nands. Further work is needed, as BigBlock/4Gb eMMC's arent yet suported.
- **I/O:** Proper UART communication via a Virtual Serial Port is implemented. POST output is also implemented.

### Whats missing:
- **Proper Memory Controller Handling:** In **CB** in **early** versions it is possible to just let the `HW_INIT` VM execute. It outputs several error messages regarding memory/bit errors but continues sucessfully. In **later** kernel/system versions as of now we just skip the call to HW_INIT because of such unimplemented hardware, as the checks fail and the VM Hangs.
- **Xenos GPU:** Basic device skeleton and several registers are implemented, as well as an early form of the Xenos framebuffer with correct Blitting interpretation,this will do it for now, at least until the kernel loads to the point that a most proper GPU emulation is needed. Hopefully Soon :) 
- **Xbox 360 Specific Hardware** (H)ANA, Sonus(?) Sound Chip, XMA Decoder, and several CPU specific undocumented configuration ring registers.

### Setup:
1. Clone the repo and build as usual using Visual Studio 2022 or later.
2. Edit the `Xe_Main.cpp` file for custom NAND/1BL locations. The defaults are `C://Xbox/1bl.bin` and `C://Xbox/xenon_nand.bin`.
3. Edit the xenon_fuses structure to match your set of CPUKey/Fuses. This step is pretty straightforward as this struct is identicall to what Xell outputs when displaying a console's fuseset. This step will be replaced by editing a text file later on.
4. If your nand dump is a RGH/XDKBuild nand, you must edit the code in order to make the signature/hash checks valid, else you'll get a PANIC error code.
	***-XDKBuild 2.0.17489.0 in JRunner is already setup in the code, just get your retail nand, make a XDKBuild image and you're good to go.***
5. Enjoy, do RE, Code!

### WinDBG Debugging:
Setting up Windbg to do kernel debugging on the emulated system is pretty straightforward, you'll need "Virtual Serial Port Driver" (Paid software with 7 day trial, I've tested using this, any other free alternative may work also) and a fresh copy of WinDBG. You can grab this form any old SDK, but don't use the Microsoft's Store version, this crashes while trying to connect.
1. Open **VSPD** and create a new Pair, **by default Xenon maps itself to use COM2**, so you need to set your VSPD to COM2 and COM3 respectively. If you don't happen to have COM2 availeable atm, edit the source in SMC.cpp to change to wich ever port number you like.
2. Open WinDBG, goto File/Kernel Debug or just hit `Alt + K`.
3. Select COM tab, and then set the Baud Rate to 115200 and COM Port to COM3 or whatever port you specified in the Pair you created earlier.
4. Hit Ok, load up Xenon, and wait.
5. You should be greeted with a debug prompt.

### Feel free to contribute/study the code!
#### Any info, help, reverse engeneering skills, documentation and knowledge are more than welcome. Help in the form of donations, no matter the size is also appreciated, as this directly goes to hw/funding efforts for reverse engeneering/tinkering.
[My Ko-Fi Page, where i'll be posting development advances and info about the 360 HW/Reverse engeneering in general.](https://ko-fi.com/bitsh1ft3r)

## Showcase
**1BL** Running on the emulated XCPU.

![1BL,running on an early stage](/Xenon/Assets/images/1bl_boot.png)

**Xell First Stage**

![Xell First Stage running up to Second Stage.](/Xenon/Assets/images/Xell_First_Stage.png)

**WinDBG connected via KDNet via a Virtual Serial Port to an early debug kernel**

![xboxkrnl.exe V2.0.2853.](/Xenon/Assets/images/windbg_2853.png)
