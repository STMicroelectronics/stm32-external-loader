# External Loaders

This is **contrib** branch of external loaders repository and includes contributions from community or special set of memory drivers for STM32.
Code on this branch is open-source and is tightly-coupled with our set of videos about development of external loaders.

## Add external memory support to your project

We have prepared set of videos about development of customized external loaders.
It provides you an overview about external loader concept and communication flow.
More information and detailed concept is available on dedicated website.

https://www.st.com/content/st_com/en/support/learning/stm32-education/stm32-moocs/external_QSPI_loader.html

## Contrib folder structure

We have prepared set of different folders for easier understanding of the complete package

- **Loader_Files** folder contains files needed for STM32 external loader design. It provides set of files and functions to interface external loader together with hardware debugger
- **QSPI driver** folder contains memory drivers for specific sales types. This is where actual driver and its linked GPIO configuration for STM32 is prepared.
- **QSPI testing** folder includes routines for memory testing. Part of code that can be used during memory test procedure to verify communication between STM32 and memory is up and properly running.
- **Demo_Project** folder contains demonstration project used with video tutorials as per link above. It can be directly opened with [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) development studio.

> To fully understand flow and usage of the above folders with dedicated files, please follow set of videos that we have prepared and published on link above.

## Contribution

We accept and encourage everyone to contribute with different set of loader or memory drivers. We do accept pull requests on **contrib** branch.

## STM32 board loaders

Loaders for our STM32 development boards are available in this repository, under **main** branch.