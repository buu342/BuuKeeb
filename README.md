# BuuKeeb

<img width="1683" height="544" alt="image" src="https://github.com/user-attachments/assets/ec05a558-b9cd-4d6b-80ae-26d65afa1400" />

I've been wanting a decent mechanical keyboard for a while but I've been having a hard time finding something which fit into all of these categories:
- 100% (105 keys)
- ISO Enter
- [QMK](https://qmk.fm/) support
- LED indicators for Caps Lock, Numlock, and Scroll Lock (This one apparently is really difficult to find despite being standard in keyboards for centuries!)

I saw a Glorious PC GMMK Barebones Keyboard for 30€ which seemed to check all of these boxes (including per-key RGB which is a nice to have), but unfortunately the PCB revision I received had a crappy 8-bit MCU, which meant that writing QMK support for it would be infeasible. The case it came with was mostly metal and pretty nice however, so I decided to make a clone of the PCB, but with an STM32. This would also serve as a cool hardware design learning experience for me!

The BuuKeeb™®© features:
- 105 keys with ISO enter for the 0.01% population of the world who types on a Portuguese layout.
- Hotswappable MX switches.
- N-Key Rollover.
- Per Key LEDs on the north side, individually addressible. [OpenRGB](https://openrgb.org/) support. Key colors savable to persistent storage.
- STM32 MCU for QMK support.
- LED indicators for Caps Lock, Numlock, and Scroll Lock (incredible feature, I know).
- My face on the keyboard for added narcissism.
- Functional. Definitely.


### Keyboard Macros:

* **Alt GR + ESC** - Clear the EEPROM
* **Alt GR + F1** - Decrease the screen brightness
* **Alt GR + F2** - Increase the screen brightness
* **Alt GR + F3** - Projection mode (Windows + P)
* **Alt GR + F4** - Open search bar
* **Alt GR + F5** - Previous track
* **Alt GR + F6** - Pause/Play
* **Alt GR + F7** - Next track
* **Alt GR + F8** - Mute
* **Alt GR + F9** - Decrease volume
* **Alt GR + F10** - Increase volume
* **Alt GR + F11** - Decrease backlight brightness
* **Alt GR + F12** - Increase backlight brightness
* **Alt GR + PrntScr** - Mute microphone
* **Alt GR + ScrLck** - Eject disk
* **Alt GR + Pause** - Suspend/Sleep PC
* **CTRL + Alt GR + ESC** - Save OpenRGB Direct Mode colors to persistent memory


### Editing in KiCad

The design uses [marbastlib](https://github.com/ebastler/marbastlib) for some components so make sure you have that installed before opening the project in KiCad.


### Ordering

The `production` folder provides all the files you need for JLCPCB. You can zip up the entire folder and feed it to JLC. 

During the BOM selection, you can refer to the `bom-JLCPCB Assembly Order.xls` file to look at what components I used when I ordered my PCBs.


### Compiling the QMK Firmware:

1. Download the [OpenRGB QMK Community Module](https://gitlab.com/OpenRGBDevelopers/QMK-OpenRGB) repository.
2. Place it inside the `qmk_firmware/modules` folder and rename it to `openrgb`
3. If your copy of the qmk firmware does not ship with a `buukeeb` folder inside the `qmk_firmware/keyboards` folder, copy the QMK folder from my repository and put it inside the `qmk_firmware/keyboards`, renaming it to `buukeeb` (alternatively place a symbolic link instead).
4. Compile the firmware using your preferred method. I am on Linux so I used `qmk compile -kb buukeeb -km default`.
5. To flash the keyboard, hold down the BOOT button on the back of the board (above the 3 status indicator LEDs) and plug it into the PC. You can then let go of the button and flash it using your preferred method. I am on Linux so I used `qmk flash -kb buukeeb -km default`. 


### Special Thanks:

- [BigBass](https://github.com/bigbass1997) for helping me out with a bunch of KiCad questions and reviewing my PCB designs.
- [Keyboard Atelier Discord community](https://kbatelier.org/) for helping me and doing a keyboard review. Specific shoutouts to Quark, Indeed, Moo, The Great Advisor Xaetral™, Sune, and SenseiZéッ for answering general questions. Another big thanks to cadiremar, galile0, Jubakuba, and SenseiZéッ for the keyboard review.
- [QMK Discord community](https://discord.gg/qmk) for helping me bugfix. Specific shoutout to Drashna Jael're for identifying the LED bug.
- [OpenRGB Discord community](https://discord.gg/AQwjJPY) for answering some OpenRGB questions. Specific shoutout to CalcProgrammer1 for answering my questions.