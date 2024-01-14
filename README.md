# Xenon - Xbox 360 Low Level Emulator Research Project.

**Current State**  
The project is in a really early stage, so far it can only load and execute several parts of the Xbox 360 bootchain, until it reaches **CD**.  
  
No binaries are provided so far, so you must build it yourself, you'll also need your own dump of the **1BL**, and a 16 Mb NAND dump with its own set of matching CPU key/eFuses.

### Whats implemented:
- **XCPU interpreter:** Featuring a custom MMU and a pretty good amount of the PowerPC 64 ISA. Based on the CELL-BE as the PPU inside the Xbox 360 CPU is the same as that, with a few changes/additions. Everything is being implemented as needed, as I find this method more easy for debugging code/instructions.
- **System Bus:**
- **PCI Bus:**
- **NAND:** Is able to load 16Mb Jasper/Trinity/Corona nands. Further work is needed, as BigBlock/4Gb eMMC's arent yet suported.
- **I/O:** UART/POST output is implemented.  
  
### Whats missing:
- **PCI devices:** USB/SATA/Ethernet/XMA Audio/etc... In **CB** i just skip the call to HW_INIT/PCI_INIT since my priority is the CPU.  
- **Xenos GPU:** Need some early form of GPU/framebuffer functionality, at least until the kernel loads to the point that a most proper GPU emulation is needed.  
- **Xbox 360 Specific Hardware** SMC, (H)ANA, Secure flash controller?

### Feel free to contribute/study the code!   
#### Any info, help, reverse engeneering skills, documentation and knowledge are more than welcome. 

## Showcase
**1BL** Running on the emulated XCPU.  
![1BL,running on an early stage](/Xenon/Assets/images/1bl_boot.png)
