# DuPAL Firmware

## Introduction

PAL is an acronym which means *Programmable Array Logic*, that is, these ICs are a family of programmable logic devices that can act as (and replace) a bunch of simpler logic devices, saving space on the PCB, costs, providing a way to hinder unauthorized design copying by competitors... and giving a big headache to vintage electronics collectors.

Most of the PAL ICs in the devices floating around are read-protected: this means that you cannot ask the device to read its content, leaving only bruteforce and black-box analisys as a way to recover the programming.

The DuPAL (**Du**mper of **PAL**s) is a set of software and hardware instruments that I developed to help me bruteforcing and analyzing these ICs, with the objective of eventually being able to dump and save all the ones in the circuit boards I have around. Wishful thinking maybe, but I'm one step closer now.

**Note:** The current version of the firmware is compatible with rev.2 boards __only__.

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

DuPAL - 0.1.1
```

Once connected via the serial port, the board will restart automatically and present the user a startup string.

### Operating modes

Newer revisions of the DuPAL support only the remote control mode, and offload all the analisys to the host.

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

For a 20 pins PAL:

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

For a 24 pins PAL:

```text
  31   30   29   28   27   26   25   24
.----.----.----.----.----.----.----.----.
| xx | xx | xx | xx | xx | xx | xx | xx | Byte 3
'----'----'----'----'----'----'----'----'

  23   22   21   20   19   18   17   16
.----.----.----.----.----.----.----.----.
| xx | xx | 23 | 14 | 13 | 11 | 22 | 21 | Byte 2
'----'----'----'----'----'----'----'----'

  15   14   13   12   11   10    9    8
.----.----.----.----.----.----.----.----.
| 20 | 19 | 18 | 17 | 16 | 15 | 10 |  9 | Byte 1
'----'----'----'----'----'----'----'----'

   7    6    5    4    3    2    1    0
.----.----.----.----.----.----.----.----.
|  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 | Byte 0
'----'----'----'----'----'----'----'----'
```

The response repeats the same mask that was written, to make sure the board got the command correctly and set the correct pins.

###### Read

- Syntax: `>R<`
- Response: `[R xx]`

Where `xx` is the hex representation of the status of the following pins, from MSB to LSB, for a 20 pins PAL:

```text
   7    6    5    4    3    2    1    0
.----.----.----.----.----.----.----.----.
| 12 | 19 | 13 | 14 | 15 | 16 | 17 | 18 |
'----'----'----'----'----'----'----'----'
```

From MSB to LSB for a 24 pins PAL:

```text
   7    6    5    4    3    2    1    0
.----.----.----.----.----.----.----.----.
| 22 | 21 | 20 | 19 | 18 | 17 | 16 | 15 |
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

###### Model

- Syntax: `>M<`
- Response: `[M xx]`

Where `xx` is the model of the DuPAL board.

E.g.

```text
[M 02]
```

For a revision 2.x board.

###### LED control

- Syntax `>L xx<`
- Response: `[L xx]`

Where `xx` is thus structured:

- Bit 0: flips the LEN on (1) or off (0)
- Bit 1-7: Select which LED to control:
  - 1: 20 pin PAL LED
  - 2: 24 pin PAL LED

This command is available on rev.2 boards onward.

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
