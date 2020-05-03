// Author: Yao Zhao

#include "Ve203_soc_top_verilator.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <iostream>
#include <string>

#define CLK_LOW  {soc->clk=0; soc->eval();}
#define CLK_HIGH {soc->clk=1; soc->eval();}

int main(int argc, char **argv) {
  Verilated::commandArgs(argc, argv);
  Ve203_soc_top_verilator *soc = new Ve203_soc_top_verilator;

  // check if trace is enabled
  int trace_en = 0;
  for (int i=0; i<argc; i++) {
      if (strcmp(argv[i], "-t") == 0)
          trace_en = 1;
      if (strcmp(argv[i], "--trace") == 0)
          trace_en = 1;
  }

  if (trace_en) {
      std::cout << "Trace is enabled.\n";
  }
  else {
      std::cout << "Trace is disabled.\n";
  }

  //enable waveform
  int main_time;
  VerilatedVcdC* tfp = new VerilatedVcdC;
  if (trace_en) {
      Verilated::traceEverOn(true);
      soc->trace(tfp, 99); // Trace 99 levels of hierarchy
      tfp->open("sim.vcd");
  }
  main_time = 0;

  soc->clk = 0;
  soc->rst_n = 0;
  soc->eval();
  if (trace_en) tfp->dump(main_time); main_time+=1;

  // enough time to reset 
  for (int i=0; i<250; i++) {
      CLK_HIGH; if (trace_en) tfp->dump(main_time); main_time+=1;
      CLK_LOW; if (trace_en) tfp->dump(main_time); main_time+=1;
  }

  soc->rst_n = 1;
  soc->eval(); 

  while (!Verilated::gotFinish()) {
      CLK_HIGH; if (trace_en) tfp->dump(main_time); main_time+=1;
      CLK_LOW; if (trace_en) tfp->dump(main_time); main_time+=1;
  }

  if (trace_en) {
      tfp->close();
  }

  delete soc;
  
  return 0;
}
