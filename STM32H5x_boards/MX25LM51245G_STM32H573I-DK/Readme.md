# Description of the MX25LM51245G_STM32H573I-DK flashloader project.
    @file    readme.md
    @author  ST/GPAM Application Team
    @brief   Description of  how to add MX25LM51245G_STM32H573I-DK external loader support.

    @attention
  
    Copyright (c) 2025 STMicroelectronics.

    All rights reserved.
  
    This software is licensed under terms that can be found in the LICENSE file
    in the root directory of this software component.
    If no LICENSE file comes with this software, it is provided AS-IS.
	
## Directory contents
the MX25LM51245G_STM32H573I-DK flashloader project contains the following folders:

**Project:**
Contains preconfigured EWARM with associated linker files. this folder contains also the MX25LM51245G_STM32H573I-DK.stldr under the folder "Binary".
**Drivers:**
Contains Source and header files providing STM32 drivers for required peripherals.
**Flash_loader:**
Contains Source and header files required by the flashloaders and containing specific information on used  memory.

## Hardware and Software Environment:
**Build Toolchain:** EWARM 9.60.3
**HW:** STM32H573I-DK board REV C01

The MX25LM51245G_STM32H573I-DK.stldr has been successfully tested on STM32CubeProgrammer v2.19.0 giving the following timing measurements:
| Test Procedures                      | Test Duration | Typical   | Maximum   |
|--------------------------------------|---------------|-----------|-----------|
| Write 256B (1 page)                  | ~ 0.66 ms     |   0.15 ms |   0.75 ms |
| 64KB Sector Erase (1 block)          | ~ 269 ms      |   220  ms |   2000 ms |
| Chip Erase                           | ~ 153  s      |   150  s  |   300 s   |
| Sector Erase + Fullchip programming  | ~ 410  s      | ~ 265  s  | ~ 2244 s  |

## How to use it ?
### This repository has been created using the `git submodule` command. Please check the instructions below for proper use. Please check also **the notes at the end of this section** for further information.

1. To **clone** this repository along with the linked submodules, option `--recursive` has to be specified as shown below.

```bash
git clone --recursive [URL to Git repo]
```

### In order to make the program work, you must do the following :
 - Open the project under Project/EWARM.
 - Rebuild all files.
 - The `MX25LM51245G_STM32H573I-DK.stldr` is generated at Project/EWARM and ready to be used 
    within **STM32CubeProgrammer** by copying it at `[STM32CubeProgrammer_ Install directory]\bin\ExternalLoader`.
 - Open STM32CubeProgrammer and click on connect.
 - Go to external loaders menu then select `MX25LM51245G_STM32H573I-DK` to program external OSPI memory.
 - From Erasing and programming Menu browse to select the hex file then click on start programming.

### In order to debug the flashloader project, you must do the following :
 1. `Enable Debug Messages`: 
    - Set the DEBUG macro in `Loader_Src.c` to 1 to enable printf and get debug messages.
 2. `Modify Private Defines`: 
    - Update the private defines for debugging in `Loader_Src.c` with the corresponding:
      #### Debug state machine execution.
      #### RAM buffer address for write operations.
      #### Memory address for write/erase operations.
      #### Size for write/erase operations.
 3. `Start Debugging in EWARM`:
    - Open EWARM and click on the Debug button to start the debugging process.
