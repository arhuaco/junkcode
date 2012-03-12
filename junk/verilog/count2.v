module counter( clk, count );
input clk;
output[2:0] count;

reg[2:0] count;
wire clk;

initial
    count = 0;

always @( negedge clk )
    count[0] <= ~count[0];

always @( negedge count[0] )
    count[1] <= ~count[1];

always @( negedge count[1] )
    count[2] <= ~count[2];

endmodule

module test;
  wire [2:0] count;
  reg clk;

  counter my_count  (clk, count);

  parameter T = 100;

  initial begin
    $display("starting");

    repeat(10) begin

    clk = 0;
    #(T) clk = 1;

    #(T) $display("%d", count);
    end

    $display("ending");
  end
endmodule
