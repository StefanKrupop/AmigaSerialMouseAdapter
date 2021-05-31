# Amiga Serial Mouse Adapter

Converts data from Serial Mouse format to quadrature encoder outputs usabe e.g. for an Amiga.
I used it to attach a Microsoft EasyBall trackball to my Amiga 500.
The Amiga interface code is (partially) based on https://gist.github.com/Quantum-cross/892be82bd60753944225ff22bf564737.

## Connections

I used an "Arduino Pro Micro" clone. My connections are as follows, but this should be easily adaptable to other Arduinos:

### Serial

An MAX232 is used as a level shifter for the serial signals.

Arduino - MAX232
TXO - 10
RXI - 9
9 - 11
10 - 12

MAX323 - DB9 (male)
7 - 3
8 - 2
13 - 8
14 - 7
15 - 5

### Amiga

Arduino - DB9 (female)
A2 - 1
A1 - 2
A3 - 3
A0 - 4
14 - 6
VCC (+5V) - 7
GND - 8
15 - 9