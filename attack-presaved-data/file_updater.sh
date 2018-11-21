#!/bin/bash
#cp ~/sancus-core/core/sim/rtl_sim/run/tb_openMSP430_view_dma_dev.gtkw .

cd ~/sancus-core/core

cd bench/verilog
sudo cp dma_tasks.v msp_debug.v tb_openMSP430.v /usr/local/share/sancus/rtl/sim

cd ../../rtl/verilog 
sudo cp *.v /usr/local/share/sancus/rtl

cd dma_controller/rtl
sudo cp dma_controller.v fifo.v fifo_reg.v register.v decoder.v counter.v /usr/local/share/sancus/rtl

cd ../../periph
sudo cp simple_dma_device.v /usr/local/share/sancus/rtl


