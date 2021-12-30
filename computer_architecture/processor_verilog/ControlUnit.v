`include "Instructions.v"

`define R_TYPE 6'b000000
`define RI_TYPE 6'b001000
`define J_TYPE 6'b000010
`define I_TYPE 6'b001000

module ControlUnit
(
    input[5:0] instruction,
    input[5:0] func,
    input zero,
    output reg regDst,
    output reg jump,
    output reg branch, 
    output reg memRead,
    output reg memToReg,
    output reg[2:0] ALUop,
    output reg memWrite,
    output reg ALUsrc,
    output reg regWrite
);

always@(*) begin
    casex(instruction)
        `R_TYPE: begin
            regDst     = 1'b1;
            jump        = 1'b0;
            branch      = 1'b0;
            memRead     = 1'b0;
            memToReg    = 1'b0;
            memWrite    = 1'b0;
            ALUsrc      = 1'b1;
            regWrite    = 1'b1;
            ALUop = `NOP;

            casex(func)
                6'b10000X: ALUop = `ADD;
                6'b10001X: ALUop = `SUB;
                6'b100100: ALUop = `AND;
                6'b100101: ALUop = `OR;
                6'b100110: ALUop = `XOR;
                default: ALUop = `NOP;
            endcase
        end

        // I type
        6'b00100X:  // ADD
                begin
                    regDst =   1'b0;
                    jump =      1'b0;
                    branch =    1'b0;
                    memRead =   1'b0;
                    memToReg =   1'b0;
                    memWrite =  1'b0;
                    ALUsrc =    1'b1;
                    regWrite =  1'b1;
                    ALUop =     `ADD;
                end

        `J_TYPE:
            begin
                regDst =   1'b0;
                jump =      1'b1;
                branch =    1'b0;
                memRead =   1'b0;
                memToReg =  1'b0;
                memWrite =  1'b0;
                ALUsrc =    1'b0;
                regWrite =  1'b0;
                ALUop =     3'b111;  
            end
    endcase
end
endmodule;