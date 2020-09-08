### Wio_Terminal_JLink_Debugging
Example shows and explains how I got debugging with JLink on Seeed Wio Terminal working on PlatformIO. There were some hacks needed to get it working

Despite of the measures described in the code in some applications Platformio/Jlink couldn't reach a next breakpoint when I clicked 'Continue' at the first temporary breakpoint. Then, when I clicked 'Restart (Ctrl Shift F5)' it stopped again at the first temporary breakpoint but after clicking 'Continue' again, it reached the first breakpoint in my setup() routine. For Debugging mode use: PIO Debug (skip Pre-Debug).

All these hacks are rather ugly and I would be happy to find a better solution.

see also:
https://community.platformio.org/t/difficulties-to-debug-seeed-wio-terminal-with-jlink-probe/15718
