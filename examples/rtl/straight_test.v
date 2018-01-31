
module straight_test(
  input       clk,
  input       reset,
  input [7:0] inp_a,
  input       inpValid,
  output      inpRetry,

  output [7:0] sum,
  output       sumValid,
  input        sumRetry
);

  logic [7:0] f1_sum;
  logic       f1_sumValid;
  logic       f1_sumRetry;

  logic [7:0] f2_sum;
  logic       f2_sumValid;
  logic       f2_sumRetry;

  logic [7:0] f3_sum;
  logic       f3_sumValid;
  logic       f3_sumRetry;

  logic [7:0] sum_next;

  always_comb begin
    sum_next = inp_a;
  end

  logic       clear;
  assign clear = 0;

  fflop #(.Size(8)) f1 (
    .clk      (clk),
    .reset    (reset),
    .clear    (clear),

    .din      (sum_next),
    .dinValid (inpValid),
    .dinRetry (inpRetry),

    .q        (f1_sum),
    .qValid   (f1_sumValid),
    .qRetry   (f1_sumRetry)
  );

  fflop #(.Size(8)) f2 (
    .clk      (clk),
    .reset    (reset),
    .clear    (clear),

    .din      (f1_sum),
    .dinValid (f1_sumValid),
    .dinRetry (f1_sumRetry),

    .q        (f2_sum),
    .qValid   (f2_sumValid),
    .qRetry   (f2_sumRetry)
  );

  fflop #(.Size(8)) f3 (
    .clk      (clk),
    .reset    (reset),
    .clear    (clear),

    .din      (f2_sum),
    .dinValid (f2_sumValid),
    .dinRetry (f2_sumRetry),

    .q        (f3_sum),
    .qValid   (f3_sumValid),
    .qRetry   (f3_sumRetry)
  );

  fflop #(.Size(8)) f4 (
    .clk      (clk),
    .reset    (reset),
    .clear    (clear),

    .din      (f3_sum),
    .dinValid (f3_sumValid),
    .dinRetry (f3_sumRetry),

    .q        (sum),
    .qValid   (sumValid),
    .qRetry   (sumRetry)
  );

endmodule

