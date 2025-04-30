
# Description of the MX25R6435F_STM32L496G-DISCO flashloader project.

### Directory contents
the MX25R6435F_STM32L496G-DISCO flashloader project contains the following folders:

**Project:**
Contains preconfigured EWARM with associated linker files. this folder contains also the MX25R6435F_STM32L496G-DISCO.stldr under the folder "Binary".
**Drivers:**
Contains Source and header files providing STM32 drivers for required peripherals.
**Flash_loader:**
Contains Source and header files required by the flashloaders and containing specific information on used  memory.

### Hardware and Software Environment:
**Build Toolchain:** EWARM 8.50.9
**HW:** STM32L496G-DISCO board REV B

The MX25R6435F_STM32L496G-DISCO.stldr has been successfully tested on STM32CubeProgrammer v2.8.0 giving the following timing measurements:
| Test Procedures             | Test Duration | Typical | Maximum |
|-----------------------------|---------------|---------|---------|
| Write 256B (1 page)         | ~0.64 ms      | 0.85 ms | 4 ms    |
| 4KB Sector Erase (1 block)  | ~100 ms       | 40 ms   | 240 ms  |
| Chip Erase                  | ~34 s         | 50 s    | 150 s   |
| Fullchip programming        | ~153 s        | 150 s   | 300 s   |
### How to use it ?
In order to make the program work, you must do the following :
 - Open the project under Project/EWARM.
 - Rebuild all files.
 - The MX25R6435F_STM32L496G-DISCO.stldr is generated at Project/EWARM and ready to be used 
    within STM32CubeProgrammer by copying it at [STM32CubeProgrammer_ Install directory]\bin\ExternalLoader.
 - Open STM32CubeProgrammer and click on connect.
 - Go to external loaders menu then select MX25R6435F_STM32L496G-DISCO to program external QSPI memory.
 - From Erasing and programming Menu browse to select the hex file then click on start programming.

If you want to clone a repository including its submodules you can use the --recursive parameter.
"git clone --recursive [URL to Git repo]"

## License
Copyright (c) 2021 STMicroelectronics. All rights reserved.
This software component is licensed by ST under BSD 3-Clause license,
the "License"; You may not use this file except in compliance with theLicense. You may obtain a copy of the License at: opensource.org/licenses/BSD-3-Clause
