`timescale 1ns / 1ps

module InstructionMemory
(
    input[31:0] addr,
    output[31:0] instruction
);

    reg[7:0] memory[255:0];

    
    assign instruction = {
            memory[addr + 32'd3],
            memory[addr + 32'd2],
            memory[addr + 32'd1],
            memory[addr]
        };

    always@(addr) begin
        $display("%b %b %b %b", memory[addr + 32'd3], memory[addr + 32'd2], memory[addr + 32'd1], memory[addr]);
        $display("memory[%d] = %x", addr, instruction);
    end
endmodule


    