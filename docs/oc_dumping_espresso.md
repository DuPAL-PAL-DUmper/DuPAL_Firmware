# Introduction
Simpler PAL devices have dual-state outputs, so just 0 and 1 out of their pins. More complex ones also support high impedence outputs, and we need to take this into account when trying to recover the equations.

# How to handle a three-state output?
The idea is to treat the hi-z state of the output as an independant pin, and create two truth table to minimize:
- The first for binary output values
- The second table will handle the hi-z state

### Example
Suppose we have 3 inputs and 2 outputs. Of these 2 outputs, both are three-state, but just the second one uses the feature.
When an output is in hi-z mode, we're not interested in its binary value (as we can't read it), so we'll have to specify this in the truth table.

#### Table 1: binary values
This table is in the format used by the **Espresso** minimizer
```
.i 3
.o 2
.ilb i1 i2 i3
.ob o1 o2

000 00
001 01
010 10
011 11
100 0-
101 0-
110 0-
111 0-

.e
```

**Espresso** will minimize it to:
```
.i 3
.o 2
.ilb i1 i2 i3
.ob o1 o2
.p 2

01- 10
--1 01

.e
```

Or, if we wish the equations:
```
o1 = (!i1&i2);
o2 = (i3);
```

#### Table 2: hi-z state
We now need to create a table to specify when the outputs are enabled or disabled.
We'll arbitrarily decide that `1` means output enabled and `0` means output disabled (hi-z).
```
.i 3
.o 2
.ilb i1 i2 i3
.ob o1 o2

000 11
001 11
010 11
011 11
100 10
101 10
110 10
111 10

.e
```

If we minimize it we get...
```
.i 3
.o 2
.ilb i1 i2 i3
.ob o1 o2
.p 2

0-- 01
--- 10

.e
```

...or the following equations:
```
o1 = ();
o2 = (!i1);
```
