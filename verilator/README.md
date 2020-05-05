# Verilator simulation/debug environment for Hummingbird E203

## 1. directory introduction
* build/: verilator compile/build environment
    * e203_soc_top_verilator/: no debug support
    * e203_soc_top_jtagdpi/: with debug support (with JTAG DPI)
* dpi/: JTAG DPI and tcp_server
* util/openocd/: OpenOCD config 

## 2. Simulation without debug support
* Build verilator simulation model: under build/e203_soc_top_verilator/:
    * `verilator -f e203_soc_top_verilator.vc --exe --trace --trace-structs --trace-params --trace-max-array 1024 -CFLAGS " -Wall -DTOPLEVEL_NAME=e203_soc_top_verilator -g -O0" -LDFLAGS "-pthread -lutil -lelf"  -Wno-PINCONNECTEMPTY -Wno-fatal -Wno-WIDTH -Wno-CASEINCOMPLETE -Wno-UNOPTFLAT`
    * `make -f Ve203_soc_top_verilator.mk`
* `mkdir sim`
* `cd sim`
* `../build/e203_soc_top_verilator/Ve203_soc_top_verilator +itcm_init=<your code image>` (add `-t` to dump VCD waveform)
    * for example: `../build/e203_soc_top_verilator/Ve203_soc_top_verilator -t +itcm_init=../../riscv-tools/riscv-tests/isa/generated/rv32ui-p-add.verilog`

## 3. Simulation with debug support
* Build verilator simulation model: under build/e203_soc_top_jtagdpi/:
    * `verilator -f e203_soc_top_jtagdpi.vc --exe --trace --trace-structs --trace-params --trace-max-array 1024 -CFLAGS " -Wall -DTOPLEVEL_NAME=e203_soc_top_verilator -g -O0" -LDFLAGS "-pthread -lutil -lelf"  -Wno-PINCONNECTEMPTY -Wno-fatal -Wno-WIDTH -Wno-CASEINCOMPLETE -Wno-UNOPTFLAT`
    * `make -f Ve203_soc_top_verilator.mk`
* Build a dummy test (if you already have one, skip this step)
    * parallel with e200_pensource: `git clone https://github.com/SI-RISCV/hbird-e-sdk`
    * under hbird-e-sdk/software: `mkdir dummy_test`
    * `cd dummy_test`
    * create a file `dummy_test.c`, see bottom
    * create a file `Makefile` (-Og in common.mk), see bottom
    * `make dummy_test BOARD=hbird-e200 CC=riscv32-unknown-elf-gcc RISCV_ARCH=rv32im RISCV_ABI=ilp32 DOWNLOAD=itcm REPLACE_PRINTF=1`
    * `riscv32-unknown-elf-objdump -fhSD dummy_test > dummy_test.dis`
    * `riscv32-unknown-elf-objcopy -O verilog dummy_test dummy_test.verilog` 
    * change all `@8000*` to `@0000*`
* switch to e200_opensource/verilator: `mkdir sim_debug`
* `cd sim_debug`
* `../build/e203_soc_top_jtagdpi/Ve203_soc_top_verilator +itcm_init=<your dummy_test code image>` 
    * for example: `../build/e203_soc_top_jtagdpi/Ve203_soc_top_verilator +itcm_init=../../../hbird-e-sdk/software/dummy_test/dummy_test.verilog`

## 4. debug with simulation
With the running simulation with debug support:
* in another terminal, under e200_opensource/verilator/: open OpenOCD: `openocd -s util/openocd -f hbird-e203.cfg` (can add `-d` with debug information)
* in yet another terminal, under e200_opensource/verilator/: open GDB: `riscv32-unknown-elf-gdb -ex "target extended-remote :3333" -ex "info reg" <your dummy_test elf image>`
    * for example: `riscv32-unknown-elf-gdb -ex "target extended-remote :3333" -ex "info reg" ../../hbird-e-sdk/software/dummy_test/dummy_test`
    * under gdb window: `set remotetimeout 2000`
    * `load <your elf image to be debugged>`
    * enjoy it

*Refer to the article [将GDB接入仿真的蜂鸟E203系统需要几步？](https://mp.weixin.qq.com/s/Ezt_2gLWGiNcNoKZYOeo_w) for more information (in Chinese).*
---

hbird-e-sdk/software/dummy_test/:

`dummy_test.c`:
```
int main()
{
    while (1);
    return 0;
}
```

`Makefile`:
```
TARGET = dummy_test
CFLAGS += -O2
BSP_BASE = ../../bsp
C_SRCS += dummy_test.c
include $(BSP_BASE)/$(BOARD)/env/common.mk
```
