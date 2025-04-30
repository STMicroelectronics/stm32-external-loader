
# Description of the MT25QL512A_STM32469-EVAL flashloader project.

This project is used to build the flashloader binary in order to program MT25QL512A memory of STM32469I-EVAL board through the OSPI interface.

### Directory contents

The MT25QL512A_STM32469-EVAL flashloader project contains the following folders:

**Project:**
Contains preconfigured EWARM with associated linker files. this folder contains also the MT25QL512A_STM32469-EVAL.stldr under the folder "Binary".
**Drivers:**
Contains Source and header files providing STM32 drivers for required peripherals.

**Flash_loader:**
Contains Source and header files required by the flashloaders and containing specific information on used  memory.

### Hardware and Software Environment:
**Build Toolchain:** EWARM 8.50.9
**HW:** STM32469-EVAL board REV C

The MT25QL512A_STM32469-EVAL.stldr has been successfully tested on STM32CubeProgrammer v2.8.0 giving the following timing measurements:
|                 Test Procedures                 | Test Duration | Typical | Maximum |
| :---------------------------------------------: | :-----------: | :-----: | :-----: |
|               Write 256B (1 page)               |    ~202ms     | 120 ms  | 1800 ms |
|           64KB Sector Erase (1 block)           |    ~150ms     | 150 ms  |   1s    |
|                   Chip Erase                    |     ~127s     |  153 s  |  460s   |
| Fullchip programming (Downlaod on erase memory) |     ~469s     |  260s   |  500s   |
|     Fullchip programming (Downlaod + Erase)     |     ~613s     |  413s   |  960s   |
### How to use it ?
In order to make the program work, you must do the following :
 - Open the project under Project/EWARM.
 - Rebuild all files.
 - The MT25QL512A_STM32469-EVAL.stldr is generated at Project/EWARM and ready to be used 
    within STM32CubeProgrammer by copying it at [STM32CubeProgrammer_ Install directory]\bin\ExternalLoader.
 - Open STM32CubeProgrammer and click on connect.
 - Go to external loaders menu then select MT25QL512A_STM32469-EVAL to program external OSPI memory.
 - From Erasing and programming Menu browse to select the hex file then click on start programming.

If you want to clone a repository including its submodules you can use the --recursive parameter.
"git clone --recursive [URL to Git repo]"

## License
Copyright (c) 2021 STMicroelectronics. All rights reserved.
This software component is licensed by ST under BSD 3-Clause license,
the "License"; You may not use this file except in compliance with the License. You may obtain a copy of the License at: opensource.org/licenses/BSD-3-Clause
