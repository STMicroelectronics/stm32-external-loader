  /**
  ******************************************************************************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the N25Q128A_STM32F413H-DISCO flashloader project.
  ***********************************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics. All rights reserved.
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
 - Build Toolchain : EWARM 8.30.1
 - HW : STM32F413H-Disco board

The N25Q128A_STM32F413H-DISCO.stldr has bee successfully tested on CubeProgrammer v2.5.0 giving the following timing measurements:
    
_____________________________________________________________________________________
                               |                 |   N25Q128A Timing Constraints     | 
      Test Procedures          |  Test Duration  |___________________________________|
                               |                 |    Typical    |     Maximum       |
_______________________________|_________________|_______________|___________________|
  Write 256B (1 page)          |    ~0.7 ms      |    0.5 ms     |       5 ms        |
_______________________________|_________________|_______________|___________________|
  64KB Sector Erase (1 block)  |    ~400 ms      |    700 ms     |      3000 ms      |
_______________________________|_________________|_______________| __________________|
  Chip Erase                   |     ~50 s       |     120 s     |      240 s        |
_______________________________|_________________|_______________|___________________|


@par How to use it ? 
===================
In order to make the program work, you must do the following :
 - Open the project under Project/EWARM
 - Rebuild all files 
 - The N25Q128A_STM32F413H-DISCO.stldr is generated at Project/EWARM and ready to be used 
    within STM32CubeProgrammer by copying it at [STM32CubeProgrammer_ Install directory]\bin\ExternalLoader
 - Open STM32CubeProgrammer and click on connect
 - Go to external loaders menu then select N25Q128A_STM32F413H-DISCO to program external OSPI memory
 - From Erasing and programming Menu browse to select the hex file then click on start programming


*********************************************Copyright (c) 2020 STMicroelectronics************************** */
