
# Description of the MX25LM51245G_STM32H7B3I-EVAL flashloader project.

### Directory contents
the MX25LM51245G_STM32H7B3I-EVAL flashloader project contains the following folders:

**Project:**
Contains preconfigured EWARM with associated linker files. this folder contains also the MX25LM51245G_STM32H7B3I-EVAL.stldr under the folder "binary".
**Drivers:**
Contains Source and header files providing STM32 drivers for required peripherals.
**Flash_loader:**
Contains Source and header files required by the flashloaders and containing specific information on used  memory.

### Hardware and Software Environment:
**Build Toolchain:** EWARM 8.50.9
**HW:** STM32H7B3I-Eval board REV B

The MX25LM51245G_STM32H7B3I-DISCO.stldr has been successfully tested on STM32CubeProgrammer v2.7 giving the following timing measurements:
| Test Procedures             | Test Duration | Typical | Maximum |
|-----------------------------|---------------|---------|---------|
| Write 256B (1 page)         | ~0.3 ms       | 0.15 ms | 0.75 ms |
| 64KB Sector Erase (1 block) | < 1 s         | 220 ms  | 2000 ms |
| Chip Erase                  | ~135 s        | 150 s   | 300 s   |
| Fullchip programming        | ~276 s        | 150 s   | 300 s   |
### How to use it ?
In order to make the program work, you must do the following :
 - Open the project under Project/EWARM.
 - Rebuild all files.
 - The MX25LM51245G_STM32H7B3I-EVAL.stldr is generated at Project/EWARM and ready to be used 
    within STM32CubeProgrammer by copying it at [STM32CubeProgrammer_ Install directory]\bin\ExternalLoader.
 - Open STM32CubeProgrammer and click on connect.
 - Go to external loaders menu then select MX25LM51245G_STM32H7B3I-EVAL to program external OSPI memory.
 - From Erasing and programming Menu browse to select the hex file then click on start programming.



## License
Copyright (c) 2021 STMicroelectronics. All rights reserved.
This software component is licensed by ST under BSD 3-Clause license,
the "License"; You may not use this file except in compliance with theLicense. You may obtain a copy of the License at: opensource.org/licenses/BSD-3-Clause
