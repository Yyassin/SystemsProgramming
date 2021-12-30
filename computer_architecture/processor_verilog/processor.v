`include "ALU.v"
`include "ControlUnit.v"
`include "DataMemory.v"
`include "InstructionMemory.v"
`include "RegisterFile.v"

module Processor
(
    input clk, rst
);

    parameter IF = 3'd0;
    parameter ID = 3'd1;
    parameter EX = 3'd2;
    parameter MEM = 3'd3;
    parameter WB = 3'd4;
    
    // Current state
    reg[2:0] state;

    // Control Wires
    wire jump, branch, zero, regWrite, regDst, memToReg, ALUsrc, memWrite, memRead;
    wire[2:0] ALUop;

    // Instruction Memory Wires
    reg[31:0] pc;
    wire[31:0] pc_4 = pc + 32'd4;
    wire[31:0] instruction;
    
    wire[31:0] instruction_se = {{16{instruction[15]}}, instruction[15:0]};
    wire[31:0] jump_addr = { pc_4[31:28], instruction[25:0], 2'b00 };
    wire[31:0] branch_addr = pc_4 + { instruction_se, 2'b00 };
    wire[5:0] opCode = instruction[31:26];
    wire[5:0] func = instruction[5:0];
    wire[4:0] src_addr = instruction[25:21];
    wire[4:0] tgt_addr = instruction[20:16];
    wire[4:0] dst_addr = instruction[15:11];
    wire[31:0] imm     = instruction[15:0];

    // Data Memory
    wire[31:0] read_data;

    // ALU wires
    wire[31:0] ALUresult;

    // Register Wires
    wire[4:0] regw_addr = regDst ? dst_addr : tgt_addr;
    wire[31:0] regw_data = memToReg ? read_data : ALUresult;
    wire[31:0] reg_out1, reg_out2;
    wire[31:0] ALUy = ALUsrc ? instruction_se : reg_out2;

    ControlUnit control(
        .instruction(opCode),
        .func(func),
        .zero(zero),
        .regDst(regDst),
        .jump(jump),
        .branch(branch),
        .memRead(memRead),
        .memToReg(memToReg),
        .ALUop(ALUop),
        .memWrite(memWrite),
        .ALUsrc(ALUsrc),
        .regWrite(regWrite)
    );

    RegisterFile r_file(
        .clk(clk), 
        .rst(rst),
        .reg1_addr(src_addr),
        .reg2_addr(tgt_addr),
        .w_addr(regw_addr),
        .w_data(regw_data),
        .wen(regWrite & state == WB),
        .reg1_out(reg_out1),
        .reg2_out(reg_out2)
    );

    InstructionMemory i_mem(
        .addr(pc), .instruction(instruction)
    );
    
    DataMemory d_mem(
        .clk(clk), .rst(rst), .addr(ALUresult),
        .data_in(reg_out2), .data_out(read_data),
        .mem_w(memWrite & state == MEM), .mem_r(memRead)
    );

    ALU alu(
        .dataX(reg_out1), .dataY(ALUy),
        .control(ALUop), .result(ALUresult),
        .zero(zero)
    );

    always@(posedge clk) begin
        if (rst) begin
            pc <= 32'b0;
            state <= IF;
        end
        else begin
            case (state)
            IF: state <= ID;
            ID: state <= EX;
            EX: state <= MEM;
            MEM: state <= WB;
            WB: begin
                pc <=   (jump)  ?   jump_addr :
                        (branch && zero) ? branch_addr :
                        pc_4;
                state <= IF;
            end
            endcase
        end
    end
endmodule