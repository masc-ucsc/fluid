
#include "Vstraight_test.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include <list>

#define MAX_TIME 20000

//#define DEBUG_TRACE 1

vluint64_t global_time   = 0;
int       terminate_set  = -1;
VerilatedVcdC* tfp       = 0;

void do_terminate() {
#ifdef TRACE
      tfp->dump(global_time);
#endif
#ifdef TRACE
  tfp->close();
#endif

  printf("simulation finished at cycle %lld\n",(long long)global_time);

  exit(0);
}

void advance_half_clock(Vstraight_test *top) {

  top->eval();
#ifdef TRACE
  tfp->dump(global_time);
#endif

  top->clk = !top->clk;

  top->eval();
#ifdef TRACE
  tfp->dump(global_time);
#endif

  global_time++;
  if (terminate_set>0)
    terminate_set--;

  if (Verilated::gotFinish() || global_time >= MAX_TIME || terminate_set==0)
    do_terminate();
}

void advance_clock(Vstraight_test *top, int nclocks=1) {

  for( int i=0;i<nclocks;i++) {
    for (int clk=0; clk<2; clk++) {
      advance_half_clock(top);
    }
  }
}

struct InputPacket {
  int inp_a;
};

struct OutputPacket {
  int sum;
};

std::list<InputPacket>  inp_list;
std::list<OutputPacket> out_list;

void tb_set_inputs(Vstraight_test *top) {

  static bool last_retry=false;

  if (last_retry) {
    top->sumRetry = (rand()&0x1)==0; // 50% chance
  }else{
    top->sumRetry = (rand()&0x7)==0; // randomly, one every 8 packets
  }
  last_retry = top->sumRetry!=0;

  if (((rand() & 0x3)==0) && inp_list.size() < 7) {
    InputPacket i;
    static uint8_t conta = 0;
    i.inp_a = conta++;
    inp_list.push_front(i);
  }

#ifdef DEBUG_TRACE
  printf("1. a=%d inpRetry=%d inpValid=%d reset=%d\n",top->inp_a, top->inpRetry, top->inpValid, top->reset);
#endif

  if (inp_list.empty()) {
    top->inpValid = 0;
  }else{
    top->inpValid = 1;

    InputPacket inp = inp_list.back();

    top->inp_a = inp.inp_a;

    if (!top->inpRetry) {
      OutputPacket o;
      o.sum = inp.inp_a;

      out_list.push_front(o);
      inp_list.pop_front();
    }
  }

#ifdef DEBUG_TRACE
  printf("2. a=%d inpRetry=%d inpValid=%d reset=%d\n",top->inp_a, top->inpRetry, top->inpValid, top->reset);
#endif
}

void tb_check_outputs(Vstraight_test *top) {
#ifdef DEBUG_TRACE
  printf("top->sum=%d sumValid=%d sumRetry=%d\n", top->sum, top->sumValid, top->sumRetry);
#endif
  // Clear if reset at the end
  if (top->reset) {
    return;
  }

  if (top->sumRetry)
    return;

  if (top->sumValid && out_list.empty()) {
    printf("FAIL: unexpected result %d @%lld\n",top->sum,(long)global_time);
    terminate_set = 5;
    return;
  }

  if (out_list.empty())
    return;

  if (!top->sumValid)
    return;

  OutputPacket o = out_list.back();
  if (top->sum != o.sum) {
    printf("FAIL: expected %d but sum is %d @%lld\n",o.sum,top->sum,(long)global_time);
    terminate_set = 5;
  }

  out_list.pop_back();
}

int main(int argc, char **argv, char **env) {

  int sim_seed;
  if (argc>1) {
    sim_seed = atoi(argv[1]);
  }else{
    sim_seed = (int)time(0);
  }
  srand(sim_seed);
  printf("My RAND Seed is %d\n",sim_seed);

  Verilated::commandArgs(argc, argv);
  // init top verilog instance
  Vstraight_test* top = new Vstraight_test;

#ifdef TRACE
  // init trace dump
  Verilated::traceEverOn(true);
  tfp = new VerilatedVcdC;

  top->trace(tfp, 99);
  tfp->open("output.vcd");
#endif

  // initialize simulation inputs
  top->clk = 1;   // Must start with 1 (or move half_advance_clock)

  int niters = 20;
  int ntest_iter = 3000;

  for(int i=0;i<niters;i++) {
    // Reset sequence for fluid
    top->inpValid = 0; // unset input valids
    top->sumRetry = 1; // set output retries
    top->reset    = 1; // set reset

    advance_clock(top,5);  // Advance clock for a few cycles
    top->reset = 0;    // Clear reset first
    advance_clock(top,1);
    top->sumRetry = 1; // Then lower output retries
    advance_clock(top,1);

    inp_list.clear();  // Remember to clear pending packets
    out_list.clear();

    for(int j=0;j<ntest_iter;j++) {
      tb_set_inputs(top);
      advance_half_clock(top);
      tb_check_outputs(top);
      advance_half_clock(top);
    }
  }
}

