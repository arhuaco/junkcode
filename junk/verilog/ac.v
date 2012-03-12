`define INIT 0
`define DOIT 1


module cell(clk, op, in, rule, lneigh, state, rneigh);

  input       clk;
  input [1:0] op;
  input       in;
  input [31:0] rule;
  input [1:0] lneigh;
  input [1:0] rneigh;

  output reg state;

  reg [1:0] s_lneigh;
  reg [1:0] s_rneigh;

  always @(negedge clk) 
  begin
     s_lneigh <= lneigh;
     s_rneigh <= rneigh;
  end

  always @(posedge clk) 
    case(op)
     `INIT: state <= in;
     `DOIT: state <= rule[{s_lneigh, state, s_rneigh}];
    endcase

endmodule

module ac_test;
  parameter T = 10;
  parameter CA_WIDTH = 60;
  parameter INITIAL_SIZE = 8;

  reg clk;

  reg [1:0] op;
  reg [4:0] i;

  reg [31:0] rule;
  wire [CA_WIDTH - 1:0] state;
  reg [INITIAL_SIZE - 1:0] seed;

/* generated with ac-help0.py */
/* cell  cell59(clk, op,  1'b0, rule, state[1:0], state[59], state[58:57]); */

`include "ac-cells.vv" // CA_WIDTH INITIAL_SIZE 

  initial begin
    clk = 0;

    rule = 2868868760; // http://geocities.com/arhuaco/img/gaca/2868868760.gif

    op = `INIT;

    clk = 1;
    #(T) $display ("%t %b", $time,  state);

    op = `DOIT;

    for(i=0;i<10;i=i+1)
    begin
      #(T) clk = 0;
      #(T) clk = 1;
      #(T) $display ("%t %b", $time,  state);
    end
  end

endmodule

