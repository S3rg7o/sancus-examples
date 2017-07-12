# sancus-examples

Elementary example programs to test your Sancus distribution.

## Overview

- **hello-world:** minimal working example of a Sancus program with an SM that
  dumps "hello world" on (untrusted) stdout.
- **sensor-reader:** example program from the paper, demonstrating secure
  linking between dummy sensor/reader SMs.
- **sancus-contiki**: Supporting files for some of the demos: port of the
  Contiki OS with support for Sancus modules
- **ta-demo**: "Lightweight and flexible trust assessment modules
  for the Internet of Things", ESORICS 2015;
  https://lirias.kuleuven.be/bitstream/123456789/516201/1/esorics15.pdf

## Running the Examples

All example programs can be built for either the Sancus Verilog simulator, or
for an FPGA. To run the example in `sancus-sim`, proceed as follows:

```bash
$ make SIM=1 clean sim
```

Alternatively, to upload the example to an FPGA using `sancus-loader`:

```bash
$ make clean load
```