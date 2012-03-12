`define INIT 0
`define DOIT 1

module cell(clk, op, in, rule, lneigh, rneigh, state );
  input       clk;
  input [1:0] op;
  input       in;
  input [7:0] rule;
  input       lneigh;
  input       rneigh;

  output reg state;

  reg s_lneigh;
  reg s_rneigh;

  always @(negedge clk) 
  begin
     s_lneigh <= lneigh;
     s_rneigh <= rneigh;
  end

  always @(posedge clk) 
    case(op)
     `INIT: state <= in;
     `DOIT: state <= rule[{s_lneigh,state,s_rneigh}];
    endcase

endmodule


module ac_test;
  parameter T = 10;

  reg clk;

  reg [1:0] op;
  reg [4:0] i;

  reg [7:0] rule;

  reg [7:0] initial_state;

  wire [7:0] state;

  cell c0(.clk(clk), .op(op), .in(initial_state[0]), .rule(rule), .lneigh(state[1]), .rneigh(state[7]), .state(state[0]) );
  cell c1(.clk(clk), .op(op), .in(initial_state[1]), .rule(rule), .lneigh(state[2]), .rneigh(state[0]), .state(state[1]) );
  cell c2(.clk(clk), .op(op), .in(initial_state[2]), .rule(rule), .lneigh(state[3]), .rneigh(state[1]), .state(state[2]) );
  cell c3(.clk(clk), .op(op), .in(initial_state[3]), .rule(rule), .lneigh(state[4]), .rneigh(state[2]), .state(state[3]) );
  cell c4(.clk(clk), .op(op), .in(initial_state[4]), .rule(rule), .lneigh(state[5]), .rneigh(state[3]), .state(state[4]) );
  cell c5(.clk(clk), .op(op), .in(initial_state[5]), .rule(rule), .lneigh(state[6]), .rneigh(state[4]), .state(state[5]) );
  cell c6(.clk(clk), .op(op), .in(initial_state[6]), .rule(rule), .lneigh(state[7]), .rneigh(state[5]), .state(state[6]) );
  cell c7(.clk(clk), .op(op), .in(initial_state[7]), .rule(rule), .lneigh(state[0]), .rneigh(state[6]), .state(state[7]) );

  initial begin
    clk = 0;
    rule = 150; /* http://mathworld.wolfram.com/ElementaryCellularAutomaton.html */
    initial_state = 8'b00010000;
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
