# BuuKeeb

<img width="1683" height="544" alt="image" src="https://github.com/user-attachments/assets/ec05a558-b9cd-4d6b-80ae-26d65afa1400" />

I've been wanting a decent mechanical keyboard for a while but I've been having a hard time finding something which fit into all of these categories:
- 100% (105 keys)
- ISO Enter
- [QMK](https://qmk.fm/) support
- LED indicators for Caps Lock, Numlock, and Scroll Lock (This one apparently is really difficult to find despite being standard in keyboards for centuries!)
- Per key LEDs are optional but not a priority

I saw a Glorious PC GMMK Barebones Keyboard for 30€ which seemed to check all of these boxes, but unfortunately the PCB revision I received had a crappy 8-bit MCU, which meant that writing QMK support for it would be infeasible. The case it came with was all metal and pretty nice however, so I decided to make a clone of the PCB, but with an STM32. This would also serve as a cool hardware design learning experience for me!

The BuuKeeb™®© features:
- 105 keys with ISO enter for the 0.01% population of the world who types on a Portuguese layout
- Hotswappable MX switches
- N-Key Rollover
- Per Key LEDs on the top side, individually addressible. [OpenRGB](https://openrgb.org/) support.
- STM32 MCU for QMK support
- LED indicators for Caps Lock, Numlock, and Scroll Lock (incredible feature, I know)
- My face on the keyboard for added narcissism
- Functional. Probably.

The design uses [marbastlib](https://github.com/ebastler/marbastlib) for some components so make sure you have that installed before opening the project in KiCad.

### TODO:

- Buy keycaps

### Special Thanks:

- [BigBass](https://github.com/bigbass1997) for helping me out with a bunch KiCad questions and reviewing my PCB designs.
- [Keyboard Atlier Discord community](https://kbatelier.org/) for helping me and doing a keyboard review. Specific shoutouts to Quark, Indeed, Moo, The Great Advisor Xaetral™, Sune, and SenseiZéッ for answering general questions. Another big thanks to cadiremar, galile0, Jubakuba, and SenseiZéッ for the keyboard review.
- [QMK Discord community](https://discord.gg/qmk) for helping me bugfix. Specific shoutout to Drashna Jael're for identifying the LED bug.
- [OpenRGB Discord community](https://discord.gg/AQwjJPY) for answering some OpenRGB questions. Specific shoutout to CalcProgrammer1 for answering my questions.