
`timescale 1ns/1ns

module sumcarry3 (clk, A, B, R, C);
  input clk;
  input [2:0] A, B;
  output [2:0] R;
  output C;

  reg [2:0] R;
  reg C;


  always @(posedge clk) begin
    {C, R}  <= A + B;
    $display("%t", $time);
    $monitor(A, "A changed in %t", $time);
  end

endmodule

module test;
  reg clk;
  reg  [2:0] S1, S2;
  wire [2:0] RES;
  wire C;

  sumcarry3 mysum (clk, S1, S2, RES, C);

  parameter T = 1;

  initial begin
    
    $display("starting");
    $dumpfile("test.vcd");
    $dumpvars(clk, S1, S2, RES, C);


    #(T) clk = 0; 
    S1  <= 0;
    S2  <= 1;
    #(T) clk = 1;
    clk = 0;

    $display("%d = %d + %d (C = %d)",RES, S1, S2, C);

    S1  <= 6;
    S2  <= 2;
    #(T) clk = 1;
    clk = 0;
    $display("%d = %d + %d (C = %d)",RES, S1, S2, C);

    S1  <= 6;
    S2  <= 1;
    #(T) clk = 1;
    clk = 0;
    $display("%d = %d + %d (C = %d)",RES, S1, S2, C);
 
    $display("ending");
  end
endmodule
