# dragon_notes #
These are my notes on using an AVR dragon on Linux.

Right now it's mainly for an Arduino Pro Mini (so a Atmega328p), but if I use
other devices I will update my notes.

We will use: 
*  AVR-GCC to compile the program
*  AVRdude to upload the program and to set the fuses on the ATmega
*  Avarice to debug
*  AVR-GDB as interface to Avarice

I'm not really sure how the tasks are divided between Avarice and AVR-GDB, but
I use them together.

The compile commands where taken from the makefile from the book "MAKE: AVR
Programming".

## Permissions for AVR dragon ##
You could run all upload commands using sudo, but that is not a good idea.

Better is to add a rules file. Add 99-avrdragon.rules to:

    /etc/udev/rules.d/

and add yourself to the plugdev group using:

    # groupadd plugdev
    # useradd -G plugdev myusername

The easiest way to apply these settings is to restart.

## Arduino Pro Mini ##
### Connections ###
What to connect on the dragon to the arduino:

*  VCC (on dragon) - Arduino RAW
*  GND (on dragon) - Arduino GND
*  ISP pin 1 (MISO) - Arduino Pin 12
*  ISP pin 2 (VTG) - Arduino VCC
*  ISP pin 3 (SCK) - Arduino Pin 13
*  ISP pin 4 (MOSI) - Arduino Pin 11
*  ISP pin 5 (RESET) - Arduino RST
*  ISP pin 6 (GND) - GND header on AVR dragon

Of course, we use a USB cable to connect the AVR dragon to the computer.

For my test setup nothing else is connected.

### Fuses ###
We use avrdude to set fuses. 

To read fuses, use:

    avrdude -c dragon_isp -p atmega328p -nv

To set the default fuses, use:

    avrdude -c dragon_isp -p atmega328p \
                    -U lfuse:w:0xFF:m -U hfuse:w:0xDA:m -U efuse:w:0x05:m

These are the default fuses I got on my chinese Arduino clone. Check for
yourself what they should be.

### Compile ###
I've included a sample program, example\_pro\_mini.c

    avr-gcc -Os -g -std=gnu99 -Wall -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -DF_CPU=16000000UL -DBAUD=9600UL -mmcu=atmega328p -c -o example_pro_mini.o example_pro_mini.c
    avr-gcc -Wl,-Map,example_pro_mini.map  -Wl,--gc-sections  -mmcu=atmega328p example_pro_mini.o  -o example_pro_mini.elf
    avr-objcopy -j .text -j .data -O ihex example_pro_mini.elf example_pro_mini.hex
### Upload ###
    avrdude -c dragon_isp -p atmega328p  -U flash:w:example_pro_mini.hex
### Debug ###
This device only supports debugwire. To turn this on you need to set the right
fuse. Check the datasheet for which fuse.

On the Atmega328p the fuce is bit 7 of the High fuse byte. Making something 0
turns it on, so if the byte was 0xDA and you want to turn debug on it will need
to become 0x9A.

    avrdude -c dragon_isp -p atmega328p -U hfuse:w:0x9A:m

Now it's ready for debug. Start avarice.

    avarice --file example_pro_mini.hex -g -w :23232

and in a different terminal

    avr-gdb example_pro_mini.elf
    (gdb) target remote localhost:23232

Now you can debug. For example:

    (gdb) continue

Will run the program. To stop it use CTRL-C.

    (gdb) set delaytime = 40.0
    (gdb) continue

And enjoy the fast flashing lights.

I had some problems with avarice (turns out I was using gdb instead of
avr-gdb), and I got a segfault. My CPU usage was extremely high afterwards, and
it turned out I had to kill it again.
