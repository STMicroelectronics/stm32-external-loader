# Demo project for MT25TL256 (32MiB dual-quad FLASH) + STM32H7A3ZIT6Q

Builds `STM32H7A3ZIT6Q_MT25TL256.stldr`

## MT25TL256 
- 32MiB (2*16MiB)
- dual-quad/twin-quad. Two 16MiB quad dies in one package (not to be confused with octoflash). 
- supports 133MHZ STR mode 90MHz DTR mode for respective 133MiB/s and 180MiB/s

## STM32H7A3ZIT6Q
- OctoSPI configured as dual-quad (odd bytes are stored in 1 die, even bytes in the other die)
- In DTR mode quarter cycle delay is used
- In STR mode half cycle delay is used
- MCU @ 270MHz, FLASH @ 90MHz

## Tested

In application with MCU @ 280MHz:

- DTR mode @ 56MHz: tested with 1000*2MiB read: 0 errors
- DTR mode @ 70MHz: tested with 1000*2MiB read: 36 errors
