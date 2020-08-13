# DuPAL Firmware

## Introduction

PAL is an acronym which means *Programmable Array Logic*, that is, these ICs are a family of programmable logic devices that can act as (and replace) a bunch of simpler logic devices, saving space on the PCB, costs... and providing a way to hinder unauthorized design copying by competitors.

Most of the PAL ICs in the devices floating around are read-protected: this means that you cannot ask the device to read its content, leaving only bruteforce and black-box analisys as a way to recover the programming.

The DuPAL (**Du**mper of **PAL**s) is a set of software and hardware instruments that I developed to help me bruteforcing and analyzing these ICs, with the objective of eventually being able to dump and save all the ones in the old circuit boards I have around.

### Building

Building was tested on *Debian 10* and *Fedora 32*.

Once the required dependecies are installed, it should be sufficient to type `make` to start the compilation, and `make program` to update the board (this requires optiboot bootloader already flashed on the chip).

Edit the `Makefile` to change the default serial port for flashing (default `/dev/ttyUSB0`).

#### Debian 10 packages

Install the AVR development packages:

```shell
sudo apt install avrdude avr-libc binutils-avr gcc-avr
```

#### Fedora 32 packages

Install the following packages:

```shell
sudo dnf install avr-binutils avr-gcc avr-libc avrdude
```

### Connecting to the DuPAL

The DuPAL uses an RS232 connection with the following parameters:

- Speed 57600bps
- 8 data bits
- No parity
- 1 stop bit
- No flow control

You can use any terminal emulator with basic VT100 support.
For, example, you can use `microcom`:

```shell
$ microcom -s 57600 -p /dev/ttyUSB0
connected to /dev/ttyUSB0
Escape character: Ctrl-\
Type the escape character to get to the prompt.

DuPAL - 0.0.8

Select which PAL type to analyze:
---------------------------------
a) PAL16L8/PAL10L8
b) PAL12L6
x) Remote control
Press the corresponding letter to start analisys.
```

Once connected via the serial port, the board will restart automatically and present the user a menu.

### Operating modes

The DuPAL firmware has two operating modes, that can be selected via the menu presented after connecting via terminal.

#### Standalone mode

The **standalone mode** is the easiest to use but also the most limited mode: it's sufficient to select the type of PAL that was previously inserted in the board (**TURN THE POWER OFF FIRST!**) and the board will perform the correct analisys for the selected type of chip. The drawback is that only purely combinatorial PAL devices are supported.

Once the analisys start, a truth table in a format compatible with **espresso** will be printed out.

This table can be used to get minimized equations. You can refer to the documentation in the **DuPAL Analyzer** repository for further details on minimization and feedback outputs.

##### Feeding the truth table to espresso

To capture the truth table that the DuPAL generates, we need to record the output coming from the serial port:

```shell
$ microcom -s 57600 -p /dev/ttyUSB0 | tee ~/output.tbl
connected to /dev/ttyUSB0
Escape character: Ctrl-\
Type the escape character to get to the prompt.

DuPAL - 0.0.8

Select which PAL type to analyze:
---------------------------------
a) PAL16L8/PAL10L8
b) PAL12L6
x) Remote control
Press the corresponding letter to start analisys.

-[ PAL12L6 analyzer ]-

----CUT_HERE----
.i 12
.o 6
.ilb i1 i2 i3 i4 i5 i6 i7 i8 i9 i11 i12 i19
.ob o18 o17 o16 o15 o14 o13
.phase 000000
000000000000 000000
100000000000 000100
010000000000 000000
...
101111111111 110000
011111111111 100010
111111111111 111111
.e
----CUT_HERE----
Analisys complete.
```

We can then copy all the text between the two `----CUT_HERE----` headers and feed it to espresso to obtain our equations:

```shell
espresso -Dexact -oeqntott ~/output_clean.tbl
```

##### Supported PALs in standalone mode

###### PAL10L8

The **PAL10L8** has:

- 10 input pins (1-9, 11)
- 8 output pins (12-19)

This device has no tri-state outputs and no registered outputs. Bruteforcing is a simple matter of trying all the combinations.
The analisys logic is a subset of the **PAL16L8** below.

###### PAL16L8

The **PAL16L8** has:

- 10 input pins (1-9, 11)
- 2 tri-state outputs (12, 19)
- 6 selectable Input / Output pins (13-18)

Having no registered outputs means that this device has no memory of previous states, so it can be bruteforced with relative ease.

###### PAL12L6

The **PAL12L6** has:

- 12 input pins (1-9, 11, 12, 19)
- 6 output pins (13-18)

Having no registered or open collector outputs means that bruteforcing is just a matter of trying all input combinations.

#### Remote Control mode

Entering **Remote Control** mode puts the board in a state where it waits commands from the host.

The commands are in ASCII format and can be input by hand, but the mode is meant to be leveraged by an external application that can pilot the board to perform advanced analisys.

A client for this mode is the **DuPAL Analyzer**, which has support for some registered (stateful) PAL devices.

##### Remote Control protocol

The **Remote Control** protocol is pretty simple and ASCII based. It supports 4 commands, each with its own syntax and response.

A string `CMD_ERR` will be sent in case the command is not recognized.

###### Write

- Syntax: `>W xxxxxxxx<`
- Response: `[W xxxxxxxx]`

Where `xxxxxxxx` is the hex representation of the status to apply to the following pins, from MSB to LSB:

```text
  31   30   29   28   27   26   25   24
.----.----.----.----.----.----.----.----.
| xx | xx | xx | xx | xx | xx | xx | xx | Byte 3
'----'----'----'----'----'----'----'----'

  23   22   21   20   19   18   17   16
.----.----.----.----.----.----.----.----.
| xx | xx | xx | xx | xx | xx | 12 | 19 | Byte 2
'----'----'----'----'----'----'----'----'

  15   14   13   12   11   10    9    8
.----.----.----.----.----.----.----.----.
| 13 | 14 | 15 | 16 | 17 | 18 | 11 |  9 | Byte 1
'----'----'----'----'----'----'----'----'

   7    6    5    4    3    2    1    0
.----.----.----.----.----.----.----.----.
|  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 | Byte 0
'----'----'----'----'----'----'----'----'
```

Pins 12 through 19 are connected to the MCU via a 10k resistor.

The response repeats the same mask that was written, to make sure the board got the command correctly and set the correct pins.

###### Read

- Syntax: `>R<`
- Response: `[R xx]`

Where `xx` is the hex representation of the status of the following pins, from MSB to LSB:

```text
   7    6    5    4    3    2    1    0
.----.----.----.----.----.----.----.----.
| 12 | 19 | 13 | 14 | 15 | 16 | 17 | 18 |
'----'----'----'----'----'----'----'----'
```

###### Reset

- Syntax: `>L<`
- Response: N/A

This command will force a reset by watchdog of the DuPAL board.

###### Exit

- Syntax: `>X<`
- Response: N/A

This command is pretty useless for now, all it does is exiting from the Remote Control mode and leave the board waiting for another serial connection or a reset.

## Hardware notes

### Recognizing three-state outputs

To recognize wether a pin is an input, an output, or an output in hi-z mode, the DuPAL uses the following method:

Every possible output is connected to the MCU by using two pins:

- One connection will be direct, and will be to an *input* of our microcontroller
- The second connection will be through a resistor of relatively high value (e.g. 10k) to an *output* pin of our microcontroller

```text
    PAL                             MCU
             |       Resistor  |
Unknown PIN  |-----o--/\/\/\---| Output PIN
             |     |           |
             |     `-----------| Input PIN
```

The board will then do the following:

1. Set the **MCU output** as **high**
2. Read the **MCU input**
    - If it is **low**, then the **PAL pin** is an **output** (because it's in a different state than what we're pulling it to be)
    - If it is **high**, go on with the test
3. Set the **MCU output** as **low**
4. Read the **MCU input**
    - If it is **high**, then the **PAL pin** is an **output**
    - Otherwise, the **PAL pin** is either an **input** or an output in **hi-Z** (see below)

Whether a pin is in tri-state or an input produces the same result with this method. In some models, we know that some pins can only be three-state outputs and can never be input: those are easy to discriminate.

For every other IO pin, we'll go through the combinations of the input pins and record which of the IO pins is consistently in hi-z state. We'll mark these pins as **input**.

This must be done for every unknown pin.

#### Why it works

The resistor will avoid a short-circuit in case the PAL pin is an output, and it will also make the MCU output drive weak enough to not be able to change the state of said PAL output, but still strong enough to change the state in case the pin is an input (or an output in hi-Z mode).
In short, the resistor will make the outputs act a dynamic pull-ups/downs.
