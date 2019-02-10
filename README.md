# sancus-examples
[![Build Status](https://travis-ci.org/S3rg7o/sancus-examples.svg?branch=master)](https://travis-ci.org/S3rg7o/sancus-examples)

Elementary example programs to test your Sancus distribution.

## Overview

The following programs showcase the security properties offered by a Sancus processor.

| Security feature          | Example program                | Comments                                                        |
|-------------------------- |--------------------------------|-----------------------------------------------------------------|
| Software module isolation | [hello-world](hello-world)     | Minimal working example to enable/disable a Sancus module.      |
| Secure linking            | [sensor-reader](sensor-reader) | Caller/callee authentication between sensor and reader SMs.     |
| Remote attestation        | [sensor-reader](sensor-reader) | Compute MAC over fresh challenge (nonce).                       |
| Secure communication      | [sensor-reader](sensor-reader) | Authenticated encryption with associated data primitive.        |
| Confidential deployment   | [hello-world](hello-world)     | SM text section is transparently encrypted at build time.       |
|-------------------------- | DMA working examples |-----------------------------------------------------------------|
| DMA exploitation          | [hello-DMA](hello-DMA) | SM private sections are read/written when direct DMA interface is provided.      |
| MMIO DMA operation        | [MMIO_DMA_op](MMIO-DMA-operation) | Shows DMA intra-memory transfer that fully unburdens CPU during the whole operation.      |


Building upon the above primitives, compiler support has been implemented for the following additional features.

| Compiler feature          | Example program                | Comments                                                        |
|-------------------------- |--------------------------------|-----------------------------------------------------------------|
| Secure memory-mapped I/O  | [sensor-reader](sensor-reader) | Assembly driver SM with exclusive ownership over sensor device. |
| Secure interrupt handling | todo                           | Minimalist program using `SM_ISR` for GPIO IRQ?                 |
| Dynamic loading           | todo                           | Issue #3                                                        |
| Intra-SM multiplication   | [arithmetic](arithmetic)       | Dummy SMs performing secure (inlined) arithmetic operations.    |
| Reactive programming      | todo                           | Minimalist (non-contiki) program with `SM_INPUT/OUTPUT`?        |


## Running the examples

All example binaries can be ran in either the Sancus Verilog simulator, or on an external FPGA. To build and run the example in `sancus-sim`, proceed as follows:

```bash
$ make sim
```

Alternatively, to upload the example to an FPGA using `sancus-loader`:

```bash
$ make load
```

# Note (Sergio)
The sensor-reader-attack uses the Sancus core supporting DMA interface, therefore before the simulation starts it is required to update core components with [Sancus DMA-supporting core](https://github.com/S3rg7o/sancus-core/tree/master/core/rtl/verilog).
Hence, specific files are copies into sancus-core main folder on Travis, and then the simulation starts. 
