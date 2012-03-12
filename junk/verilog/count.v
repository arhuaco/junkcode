module test;
  parameter COUNTW = 3;
  reg  [COUNTW - 1:0] counter;

  parameter T = 1;

  initial begin
    
    $display("starting");
    $dumpfile("test.vcd");
    $dumpvars(counter);

    #(T)counter = 0;

    repeat(10) begin
      #(T)counter = counter + 1;
      $display("%d", counter);
    end

    $display("ending");
  end
endmodule
