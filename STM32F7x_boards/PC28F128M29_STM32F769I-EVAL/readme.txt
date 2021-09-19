  /**
  ******************************************************************************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PC28F128M29_STM32F769I-EVAL flashloader project.
  ***********************************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  *************************************************************************************************
   
@par Description
================
 Flashloader package content description
 
++ Directory contents
--------------------- 
  - Project : Contains preconfigured EWARM with associated linker files

  - Sources : Contains :
                  -Library : Source files providing STM32 drivers for required peripherals
                  -Loader  : Source files required by the flashloaders and containing specific information
                             on used memory

  - Includes : Contains CMSIS and header files for the above sources 

++ Hardware and Software Environment
------------------------------------
 - Build Toolchain : EWARM 8.50.6
 - HW : STM32F769I-Eval board rev B

PC28F128M29_STM32F769I-EVAL.stldr has bee successfully tested on CubeProgrammer v2.7.0 giving the following timing measurements:
    
_____________________________________________________________________________________
                               |                 |  PC28F128M29 Timing Constraints   | 
      Test Procedures          |  Test Duration  |___________________________________|
                               |                 |    Typical    |     Maximum       |
_______________________________|_________________|_______________|___________________|
  Write 16B (1 page)           |    ~120 µs      |    70  µs     |     200 µs        |
_______________________________|_________________|_______________|___________________|
  128KB Sector Erase (1 block) |    ~200 ms      |    500 ms     |      4000 ms      |
_______________________________|_________________|_______________| __________________|
  Chip Erase                   |     ~36 s       |     38 s      |      114 s        |
_______________________________|_________________|_______________|___________________|
  Full chip programming (16MB) |                 ~129 s (test duration)              |
_______________________________|_____________________________________________________|
 
@par How to use it ? 
===================
In order to make the program work, you must do the following :
 - Open the project under Project/EWARM
 - Rebuild all files 
 - The PC28F128M29_STM32F769I-EVAL.stldr is generated at Project/EWARM and ready to be used 
    within STM32CubeProgrammer by copying it at [STM32CubeProgrammer_ Install directory]\bin\ExternalLoader
 - Open STM32CubeProgrammer and click on connect
 - Go to external loaders menu then select PC28F128M29_STM32F769I-EVAL to program external nor flash memory
 - From Erasing and programming Menu browse to select the hex file then click on start programming


*********************************************Copyright (c) 2021 STMicroelectronics************************** */
