# Introduction

This repository contains the Flashloaders source code of external memories embedded in STM32 HW boards.
* The branch **master** provides the Flashloaders projects and source files as it's integrated in STM32CubeProgrammer tool.
* The branch **contrib** helps you to integrate the external memory to your Embedded system driven by STM32.


# External memories

External memories are available on many STM32 HW board like the evaluation and discovery boards. It can be a Flash or SRAM and it provides higher storage capabilities. The STM32 boards supports many types of external memories such us Micron, Winbond that are connected to the mcu through different interfaces like FMC, SPI..


# Package structure

The flashloader project is built with EWARM or MDK-ARM IDE and comes with the corresponding source, header and linker files:

* Library : source/headers files providing required drivers to manage read, write erase functionalities of the supported memory that are required to implement the initialization, erase and write functions needed by the flashloader.

* Loader  : source/headers files containing specific information related to the supported memory (name, size…) and functions required by the flashloader

* Project : contains a preconfigured project with the associated linker file


# How to adapt the flashloader project for a customized board

The required steps to build a customized loader for cube programmer are vailable at this [link](https://www.st.com/content/ccc/resource/technical/document/user_manual/e6/10/d8/80/d6/1d/4a/f2/CD00262073.pdf/files/CD00262073.pdf/jcr:content/translations/en.CD00262073.pdf) (Section 3.9).

# Contribution

We propose and accept contribution from community for new loaders and drivers only on **contrib** branch. This branch, **master**, includes original external loaders for STM32 development boards.

