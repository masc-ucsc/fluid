
#include "Vjoin_test.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include <list>

#include <time.h>

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

void advance_half_clock(Vjoin_test *top) {

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

void advance_clock(Vjoin_test *top, int nclocks=1) {

  for( int i=0;i<nclocks;i++) {
    for (int clk=0; clk<2; clk++) {
      advance_half_clock(top);
    }
  }
}

void sim_finish() {
#ifdef TRACE
  tfp->close();
#endif

  exit(0);
}

struct InputPacketA {
  uint8_t inp_a;
};

struct InputPacketB {
  uint8_t inp_b;
};

struct OutputPacket {
  uint8_t sum;
};

double sc_time_stamp() {
  return 0;
}

std::list<InputPacketA>  inpa_list;
std::list<InputPacketB>  inpb_list;
std::list<OutputPacket>  out_list;

void tb_set_inputs(Vjoin_test *top) {
  top->sumRetry = (rand()&0xF)==0; // randomly, one every 8 packets

  if (((rand() & 0x3)==0) && inpa_list.size() < 7 && inpb_list.size()<7) {
    static uint8_t conta = 0;

    InputPacketA a;
    a.inp_a = conta;
    inpa_list.push_front(a);

    InputPacketB b;
    b.inp_b = conta;
    inpb_list.push_front(b);

    conta++;

    OutputPacket building_out;
    building_out.sum = (a.inp_a + b.inp_b) & 0xFF;
    out_list.push_front(building_out);
  }

  if (inpa_list.empty() || (rand() & 0x3)==1) {
    top->inp_aValid = 0;
    top->inp_a = rand();
  }else{
    top->inp_aValid = 1;
    top->inp_a = inpa_list.back().inp_a;
  }

  if (inpb_list.empty() || (rand() & 0x3)==1) {
    top->inp_bValid = 0;
    top->inp_b = rand();
  }else{
    top->inp_bValid = 1;
    top->inp_b = inpb_list.back().inp_b;
  }


#ifdef DEBUG_TRACE
  if (top->inp_aValid)
    printf("1. a=%d inp_aRetry=%d inp_aValid=%d reset=%d\n",top->inp_a, top->inp_aRetry, top->inp_aValid, top->reset);
  if (top->inp_bValid)
    printf("1. b=%d inp_bRetry=%d inp_bValid=%d reset=%d\n",top->inp_b, top->inp_bRetry, top->inp_bValid, top->reset);
#endif

  if (!top->inp_aRetry && top->inp_aValid)
    inpa_list.pop_back();

  if (!top->inp_bRetry && top->inp_bValid)
    inpb_list.pop_back();
}

void tb_check_outputs(Vjoin_test *top) {
  if (out_list.empty())
    return;

  if (top->sumRetry)
    return;

  if (!top->sumValid)
    return;

  OutputPacket o = out_list.back();
  if (top->sum != o.sum) {
    printf("ERROR: expected %d but sum is %d\n",o.sum,top->sum);
    advance_half_clock(top);
    advance_half_clock(top);
    sim_finish();
  }

  out_list.pop_back();
}


int main(int argc, char **argv, char **env) {
  int i;
  int clk;
  Verilated::commandArgs(argc, argv);
  // init top verilog instance
  Vjoin_test* top = new Vjoin_test;

  int sim_seed;
  if (argc>1) {
    sim_seed = atoi(argv[1]);
  }else{
    sim_seed = (int)time(0);
  }
  srand(sim_seed);
  printf("My RAND Seed is %d\n",sim_seed);

#ifdef TRACE
  // init trace dump
  Verilated::traceEverOn(true);
  tfp = new VerilatedVcdC;

  top->trace(tfp, 99);
  tfp->open("output.vcd");
#endif

  // initialize simulation inputs
  top->clk = 1;

  int niters = 20;
  int ntest_iter = 3000;

  for(int i=0;i<niters;i++) {
    // Reset sequence for fluid
    top->inp_aValid = 0; // unset input valids
    top->inp_bValid = 0; // unset input valids
    top->sumRetry = 1; // set output retries
    top->reset    = 1; // set reset

    advance_clock(top,5);  // Advance clock for a few cycles
    top->reset = 0;    // Clear reset first
    advance_clock(top,1);
    top->sumRetry = 1; // Then lower output retries
    advance_clock(top,1);

    inpa_list.clear();
    inpb_list.clear();
    out_list.clear();

    for(int j=0;j<ntest_iter;j++) {
      tb_set_inputs(top);
      advance_half_clock(top);
      tb_check_outputs(top);
      advance_half_clock(top);
    }
  }

  sim_finish();
}

