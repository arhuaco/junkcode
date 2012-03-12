
/*
 * init_ca : Will initialize the AC with a rising edge of the clock
 */

module init_ca(clk, grid);
  parameter GRID_SIZE = 5;

  input clk;
  output [GRID_SIZE - 1:0]grid;
  reg [GRID_SIZE - 1:0]grid;

  always @(posedge clk) begin
    grid = 0;
    grid[GRID_SIZE/2] = 1;
  end
endmodule
/*
 * copy_ca : Copies a grid to another one
 */

module copy_ca(clk, grid_dst, grid_src);
  parameter GRID_SIZE = 5;
  parameter T = 10;

  input clk;

  input  [GRID_SIZE - 1:0]grid_src;

  output [GRID_SIZE - 1:0]grid_dst;
  reg    [GRID_SIZE - 1:0]grid_dst;

/*
  initial begin
    grid_dst = highz0;
  end
  */

  always @(posedge clk) begin
    $display  ("%t entrando a copy_ca", $time);
    #(T)grid_dst = grid_src;
    $display  ("%t saliendo de copy_ca", $time);
  end
endmodule


module next_neigh3(clk, neigh, next_val, rule);

  input clk;
  input [2:0] neigh;
  input [7:0] rule;
  output next_val;
  reg    next_val;

  always @(posedge clk) begin
    case(neigh[2:0]) 
      3'b000 : next_val = rule[0];
      3'b001 : next_val = rule[1]; 
      3'b010 : next_val = rule[2];
      3'b011 : next_val = rule[3];
      3'b100 : next_val = rule[4];
      3'b101 : next_val = rule[5];
      3'b101 : next_val = rule[6];
      3'b111 : next_val = rule[7];
      default : $display  ("Error en next_neigh3 = %x", neigh);
    endcase
    $display  ("%t saliendo de case", $time);
  end
endmodule

module ac_1d_r1;
  parameter GRID_SIZE = 5;
  parameter T = 10;

  reg clk_init;
  reg clk_iter;
  reg clk_copy;

  wire [GRID_SIZE - 1:0] grid;
  wire [GRID_SIZE - 1:0] new_grid;

  reg [7:0] rule;

  init_ca  #(GRID_SIZE) initmygrid1(clk_init, grid);
  //init_ca  #(GRID_SIZE) initmygrid2(clk_init, new_grid); /* not needed ! */

  next_neigh3          iterate_cell4(clk_iter, {grid[0], grid[4:3]}, new_grid[4], rule);
  next_neigh3          iterate_cell3(clk_iter, grid[4:2], new_grid[3], rule);
  next_neigh3          iterate_cell2(clk_iter, grid[3:1], new_grid[2], rule);
  next_neigh3          iterate_cell1(clk_iter, grid[2:0], new_grid[1], rule);
  next_neigh3          iterate_cell0(clk_iter, {grid[1:0], grid[4]}, new_grid[0], rule);

  /* Si habilito esto no funciona la copia que quiero. Quiero hacer : grid
   * = new_grid */

  //copy_ca #(GRID_SIZE) copy_grid_to_newgrid(clk_copy, grid, new_grid);
  
  initial begin
    /* initialize clocks */
    clk_copy <= 0;
    clk_iter <= 0;
    rule <= 30; /* rule 30 is a fractal, we use 0 only for a test */

    #(T)clk_init = 1; /* initialize grids */
    clk_init = 0;

    #(T) $display ("%t clk, grid=%b", $time, grid);

    #(T)clk_iter = 1; /* iterate, we get a zero */
    clk_iter = 0;

    #(T) $display ("%t %d clk, grid=%b new_grid=%b", $time, clk_init, grid, new_grid);

    #(T)clk_copy = 1; /* Copy not enabled */
    clk_copy = 0;

    #(T) $display ("%t %d clk, grid=%b new_grid=%b", $time, clk_init, grid, new_grid);
  end
endmodule
