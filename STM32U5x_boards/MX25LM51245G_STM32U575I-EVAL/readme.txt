  /**
  ******************************************************************************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the MX25LM51245G_STM32U575I-EVAL flashloader project.
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
 - Build Toolchain : EWARM 8.50.9
 - HW : STM32U575I-Eval board rev B

The MX25LM51245G_STM32U575I-EVAL.stldr has bee successfully tested on CubeProgrammer v2.7 giving the following timing measurements:
    
_____________________________________________________________________________________
                               |                 |   MT25T01G     Timing Constraints | 
      Test Procedures          |  Test Duration  |___________________________________|
                               |                 |    Typical    |     Maximum       |
_______________________________|_________________|_______________|___________________|
  Write 256B (1 page)          |    ~0.3 ms      |    0.15 ms    |     1.5 ms        |
_______________________________|_________________|_______________|___________________|
  64KB Sector Erase (1 block)  |    < 1 s        |    220 ms     |      2000 ms      |
_______________________________|_________________|_______________| __________________|
  Chip Erase                   |     ~150 s      |     150 s     |      300 s        |
_______________________________|_________________|_______________|___________________|
  Full chip programming (64Mb) |                540s (Test Duration)                 |
_______________________________|_____________________________________________________|


@par How to use it ? 
===================
In order to make the program work, you must do the following :
 - Open the project under Project/EWARM
 - Rebuild all files 
 - The MX25LM51245G_STM32U575I-EVAL.stldr is generated at Project/EWARM and ready to be used 
    within STM32CubeProgrammer by copying it at [STM32CubeProgrammer_ Install directory]\bin\ExternalLoader
 - Open STM32CubeProgrammer and click on connect
 - Go to external loaders menu then select MX25LM51245G_STM32U575I-EVAL to program external OSPI memory
 - From Erasing and programming Menu browse to select the hex file then click on start programming


*********************************************Copyright (c) 2021 STMicroelectronics************************** */
