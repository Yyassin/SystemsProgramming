`timescale 1ns / 1ns
`include "processor.v"

module processor_tb;

reg clk, rst;

Processor mips(
    .clk(clk),
    .rst(rst)
);

initial begin
    /* Waveform Setup */
    $dumpfile("Processor_tb.vcd");
    $dumpvars(0, processor_tb);

    // Add immediate: r0 = r0 + 1
    mips.i_mem.memory[3] = 8'b00100000;
    mips.i_mem.memory[2] = 8'b00000000;
    mips.i_mem.memory[1] = 8'b00000000;
    mips.i_mem.memory[0] = 8'b00000001;

    // Jump to 0 (so keep incrementing r0)
    mips.i_mem.memory[7] = 8'b00001000;
    mips.i_mem.memory[6] = 8'b00000000;
    mips.i_mem.memory[5] = 8'b00000000;
    mips.i_mem.memory[4] = 8'b00000000;

    clk = 0;
    rst = 1;
    #5;

    @(posedge clk);
    rst = 0;
    #5;
end

always begin
    clk = #1 ~clk;
    // Stop once r0 reaches 10 or (to stop inf loops on fail) when pc = 12 (jump failed)
    if (mips.r_file.registers[0] == 32'd10 || mips.pc == 32'd12) $finish;
    $display("Clock Cycle. R0 = %d: PC = %d", mips.r_file.registers[0], mips.pc == 32'd12);
end
endmodule;
