# DuPAL Firmware
## Introduction
PAL is an acronym which means *Programmable Array Logic*, that is, these ICs are a family of programmable logic devices that can act as (and replace) a bunch of simpler logic devices, saving space on the PCB, costs... and providing a way to hinder unauthorized design copying by competitors.

Most of the PAL ICs in the devices floating around are read-protected: this means that you cannot ask the device to read its content, leaving only bruteforce as a way to recover the programming.

### The PAL16L8
The **PAL16L8** is a relatively simple device.
It has:
- 10 Input pins (1-9, 11)
- 2 tri-state outputs (12, 19)
- 6 selectable Input / Output pins (13-18)
- 0 registered outputs

Having no registered outputs means that this device has no memory of previous states, so it can be bruteforced with relative ease.

If we're working with a PAL in a known circuit, we can infer which pins are inputs and outputs by looking at the schematics, but if we're working with a PAL placed in a circuit of which we know nothing, we need to find a way to identify which pin is which type.

####  How to discriminate between input, output and tri-state
We can connect unknown pins from the PAL to two pins from our MCU:
- One connection will be direct, and will be to an *input* of our microcontroller
- The second connection will be through a resistor of relatively high value (e.g. 10k) to an output pin of our microcontroller

```
    PAL                             MCU
             |       Resistor  |
Unknown PIN  |-----o--/\/\/\---| Output PIN
             |     |           |
             |     `-----------| Input PIN
```

The procedure will then be the following
1. Set the **MCU output** as **high**
2. Read the **MCU input** 
    - If it is **low**, then the **PAL pin** is an **output**
    - If it is **high**, go on with the test
3. Set the **MCU output** as **low**
4. Read the **MCU input**
    - If it is **high**, then the **PAL pin** is an **output**
    - Otherwise, the **PAL pin** is either an **input** or in **hi-Z** (see below)

Whether a pin is in tri-state or an input produces the same result with this method, we know that pin 12 and 19 can be output only and support tri-state, so those pins will always be of **output** type.
For every other IO pin, we'll go through the 1024 combinations of the input pins (remember we have 10 input pins) and record which of the IO pins is consistently in hi-z state. We'll mark these pins as **input**.

This must be done for every unknown pin (and to test whether the outputs in 12 or 19 are hi-Z). And for pins 13 to 18 this must be done prior starting the bruteforcing.

##### Why this works?
The resistor will avoid a short-circuit in case the PAL pin is an output, and it will also make the MCU output drive weak enough to not be able to change the state of said PAL output, but still strong enough to change the state in case the pin is an input (or an output in hi-Z mode).
In short, the resistor will make the outputs act a dynamic pull-ups/downs.

#### How to bruteforce the content
Once which pins are inputs or outputs is known, we can then proceed to try every input combination and record the output.
It's important to note that when testing an input combination, we can read the output pins between 13 and 18 (minus those that are found as inputs, of course) directly, but outputs on 12 and 19 require that we toggle the MCU output connected to them high/low and check that the output is not in hi-Z mode.

##### Output format
We can then output something in the following form for every combination

```
I1 I2 I3 I4 I5 I6 I7 I8 I9 I10 IO1 IO2 IO3 IO4 IO5 IO6 | IO1 IO2 IO3 IO4 IO5 IO6 O1 O2
 0  0  0  1  1  0  0  0  1   0   0   .   .   .   1   . |   .   1   1   0   .   0  x  1
```

- On the left side we get all the **inputs**
    - If an IO pin is set as output, their value will be presented as `.`
- On the right side we get the **outputs**.
    - If an IO pin is set as input, the value here will be presented as `.`
    - If an output is in hi-z mode, their value will be presented as `x`
