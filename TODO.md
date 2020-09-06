# ToDo

- Try to detect oscillating I/O pins
    - Maybe perform multiple readings of the same I/O pin to detect if it is oscillating?
    - Treat I/O pins that are set as outputs as "inputs" in the espresso truth table, ignore it ('-') for every entry except when the output is oscillating
        - In case the output is oscillating, make two entries in the truth table: one that values the oscillating output as 0, and the other at 1, obviously the input term for this output will have the inverse of the result: e.g. o18 = /o18, /o18 = o18