
module join_test(
  input       clk,
  input       reset,
  input [7:0] inp_a,
  input       inp_aValid,
  output      inp_aRetry,

  input [7:0] inp_b,
  input       inp_bValid,
  output      inp_bRetry,

  output [7:0] sum,
  output       sumValid,
  input        sumRetry
);

  logic [7:0] f1a_a;
  logic       f1a_aValid;
  logic       f1a_aRetry;

  logic [7:0] f1b_b;
  logic       f1b_bValid;
  logic       f1b_bRetry;

  logic       clear;
  assign clear = 0; // No clear in this example

  fflop #(.Size(8)) f1a (
    .clk      (clk),
    .reset    (reset),
    .clear    (clear),

    .din      (inp_a),
    .dinValid (inp_aValid),
    .dinRetry (inp_aRetry),

    .q        (f1a_a),
    .qValid   (f1a_aValid),
    .qRetry   (f1a_aRetry)
  );

  fflop #(.Size(8)) f1b (
    .clk      (clk),
    .reset    (reset),
    .clear    (clear),

    .din      (inp_b),
    .dinValid (inp_bValid),
    .dinRetry (inp_bRetry),

    .q        (f1b_b),
    .qValid   (f1b_bValid),
    .qRetry   (f1b_bRetry)
  );

  logic [7:0] sum_next;

  always_comb begin
    sum_next = f1a_a + f1b_b;
  end

  logic   inpValid;
  logic   inpRetry;

  always_comb begin
    inpValid = f1a_aValid && f1b_bValid;
  end

  always_comb begin
    f1b_bRetry = inpRetry || !inpValid;
    f1a_aRetry = inpRetry || !inpValid;
  end

  logic [7:0] f2_sum;
  logic       f2_sumValid;
  logic       f2_sumRetry;

  fflop #(.Size(8)) f2 (
    .clk      (clk),
    .reset    (reset),
    .clear    (clear),

    .din      (sum_next),
    .dinValid (inpValid),
    .dinRetry (inpRetry),

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

    .q        (sum),
    .qValid   (sumValid),
    .qRetry   (sumRetry)
  );

endmodule

