/*
 * Goal:
 *
 * Measure the entropy of a 1-dimensional cellular automata
 *
 * How:
 *
 * * Iterate it 4096 timesteps
 * * Count subsequences (h=4)
 * * Profit
 *
 * About:
 *
 *  Copyright (C) 2007 emQbit
 *    Nelson Castillo < nelson at emqbit.com > 
 *    Andres Calderon < andres.calderon at emqbit.com >
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation. We use the version 2 of the License.
 *
 */

`define COMMAND_CA_INIT 0
`define COMMAND_CA_ITERATE 1
`define COMMAND_SUBSEQ_COUNT 2
`define COMMAND_SUBSEQ_INIT 3
`define COMMAND_SUBSEQ_TRACK 4


/*
 * Rule for a 1-dimensional r=2 cellular automaton
 */

module cell_state(clk, op, in, rule, lneigh, state, rneigh);

  input       clk;
  input [2:0] op;
  input       in;
  input [31:0] rule;
  input [1:0] lneigh;
  input [1:0] rneigh;

  output state; /* we keep the state of the CA */
  reg state;

  reg [1:0] s_lneigh;
  reg [1:0] s_rneigh;

  always @(negedge clk) 
  begin
     s_lneigh <= lneigh;
     s_rneigh <= rneigh;
  end

  always @(posedge clk) 
    case(op)
     `COMMAND_CA_INIT: state <= in;
     `COMMAND_CA_ITERATE: state <= rule[{s_lneigh, state, s_rneigh}];
    endcase
endmodule

/* Subsequence Counter
 *
 * In this module we will count the value of each sub-sequence of
 * length 4. The counters will be used to compute the entropy of
 * a CA.
 *
 * Here we will allow an overflow in th counter. For h=4 in 4096
 * timesteps, the maximum value for the counter is 1024, but we will
 * hold values up to 1023. When computing the entropy, the result
 * will be the same and we'll save many bits in this implementation.
 * Note the counter will be 0 when we overflow (1023 + 1).
 *
 *  long double p = 1024 / (4096.0/ 4.0);
 *  printf("%Lg\n", p * (logl(p) / M_LN2)); // This will print 0!
 *
 */

module subsequence_counter(clk, op, cell_state);
  parameter T = 10;

  input       clk;
  input [2:0] op;
  input       cell_state;

  reg [3:0] sequence_tracker;
  reg [9:0] sequence_counter [15:0];

  always @(posedge clk) 
    case(op)
     `COMMAND_SUBSEQ_TRACK:
               sequence_tracker <= (sequence_tracker << 1) | cell_state;
     `COMMAND_SUBSEQ_INIT:
          begin
sequence_counter[0]=0; sequence_counter[1]=0; sequence_counter[2]=0;
sequence_counter[3]=0; sequence_counter[4]=0; sequence_counter[5]=0;
sequence_counter[6]=0; sequence_counter[7]=0; sequence_counter[8]=0;
sequence_counter[9]=0; sequence_counter[10]=0; sequence_counter[11]=0;
sequence_counter[12]=0; sequence_counter[13]=0; sequence_counter[14]=0;
sequence_counter[15]=0;
$display("ended");
          end 
    `COMMAND_SUBSEQ_COUNT:
           begin
        sequence_counter[sequence_tracker] = sequence_counter[sequence_tracker] + 1;
           end
    endcase
endmodule

module ac_test;
  parameter T = 10;
  parameter CA_WIDTH = 257;
  parameter INITIAL_SIZE = 8;

  parameter N_ITERATIONS = 32;

  reg clk;

  reg [2:0] op;
  reg [11:0] i; /* iterator  */


  reg [31:0] rule;

  wire [CA_WIDTH - 1:0] state;
  reg [INITIAL_SIZE - 1:0] seed;

`include "main-cells.vv" // CA_WIDTH INITIAL_SIZE 
/* cell_state cell17(clk, op, 1'b0, rule, state[19:18], state[17],
 * state[16:15]); */

`include "main-subseq.vv" // CA_WIDTH
/* subsequence_counter count0 (clk, op, state[0]); ... */

  initial begin
    seed = 8'b10000000;
    rule = 32'hAAFF7698; // http://geocities.com/arhuaco/img/gaca/2868868760.gif

    #(T)op  = `COMMAND_CA_INIT;
    #(T)clk = 0;
    #(T)clk = 1;

    #(T)op  = `COMMAND_SUBSEQ_INIT;
    #(T)clk = 0;
    #(T)clk = 1;

    #(T)op = `COMMAND_CA_ITERATE;

    for(i = 0; i < CA_WIDTH / 2; i = i + 1) // Allow the initial config to expand
    begin 
     #(T) clk = 0;
     #(T) clk = 1;
    end

    #(T) $display("starting measures");

    for(i = 0; i < N_ITERATIONS / 4; i = i + 1) begin 
      repeat(4) begin
        #(T) op = `COMMAND_CA_ITERATE;
        #(T) clk = 0;
        #(T) clk = 1;
        #(T) $display("state = %b", state);
        #(T) op = `COMMAND_SUBSEQ_TRACK;
        #(T) clk = 0;
        #(T) clk = 1;
      end

      #(T) op = `COMMAND_SUBSEQ_COUNT;
      #(T) clk = 0;
      #(T) clk = 1;
    end

    #(T) $display("ending measures");

  end
endmodule
