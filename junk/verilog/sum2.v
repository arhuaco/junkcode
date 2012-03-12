
`timescale 1ns/1ns

module sumcarry3 (A, B, R, C);
  input [2:0] A, B;
  output [2:0] R;
  output C;

  reg [2:0] R;
  reg C;

  /* always @(posedge clk) begin */
  always @(A or B) begin
    {C, R}  <= A + B;
  end

endmodule

module test;
  reg  [2:0] S1, S2;
  wire [2:0] RES;
  wire C;

  sumcarry3 mysum (S1, S2, RES, C);

  parameter T = 100;

  initial begin
    $display("starting");

    S1  <= 0;
    S2  <= 0;
    #(T) $display("%d = %d + %d (C = %d)",RES, S1, S2, C);

    S1  <= 6;
    S2  <= 2;
    #(T)$display("%d = %d + %d (C = %d)",RES, S1, S2, C);

    S1  <= 6;
    S2  <= 1;
    #(T)$display("%d = %d + %d (C = %d)",RES, S1, S2, C);
 
    $display("ending");
  end
endmodule
