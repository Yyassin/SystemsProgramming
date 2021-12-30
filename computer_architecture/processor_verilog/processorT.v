module IMem
(
    r_addr,
    instruction
);

    input[31:0] r_addr;
    output reg[31:0] instruction;

    reg [7:0]  memory[255:0]; // Software in here

    // Portable
    always@(*)
    begin
        instruction <= {
            memory[r_addr + 32'd3],
            memory[r_addr + 32'd2],
            memory[r_addr + 32'd1],
            memory[r_addr + 32'd0]
        };
    end
endmodule

module DMem
(
    addr,
    data_in,
    data_out,
    mem_write,
    mem_read,
    clk
);

    input[31:0] addr;
    input[31:0] data_in;
    output[31:0] data_out;
    input mem_w;
    input mem_r;
    input clk;

    reg [7:0]  memory[255:0]; // Software in here

    // Write 32B at time.
    always@(posedge clk)
    begin
        if (mem_w)
        begin
            memory[addr + 32'd3] <= data_in[31:24];
            memory[addr + 32'd2] <= data_in[23:16];
            memory[addr + 32'd1] <= data_in[15:8];
            memory[addr] <= data_in[7:0];
        end
    end

    assign data_out = (mem_r) ? {
            memory[addr + 32'd3],
            memory[addr + 32'd2],
            memory[addr + 32'd1],
            memory[addr]
    } : 32'd0;
    end
endmodule

module Rfile (
    reg1_addr,
    reg2_addr,
    w_addr,
    w_data,
    wen,
    reg1_out,
    reg2_out,
    clk,
    rst
);
    input[31:0] registers[31:0]; 

    always@(posedge clk)
    begin
        if(rst)
        begin
        end
        else 
        begin
            if (wen)
            begin
                registers[w_addr] <= w_data;
            end
        end
    end

    assign reg1_out = registers[reg1_addr];
    assign reg2_out = registers[reg2_addr];
endmodule

module ALU (
    data1,
    data2,
    result,
    control,
    zero
);

input[31:0] data1;
input[31:0] data2;
output[31:0] result;
output zero;

input[] control;

assign result = (control == `ADD)       ?       data1 + data2 :
                (control == `SUB)       ?       data1 - data2 :
                32'd0; // ...And more

assign zero = (result == 32'd0);

endmodule

module processor(
    clk,
    rst,
);

    reg[31:0] pc;

    // CONTROL SIGNALS
    wire Jump, Branch, Zero, RegWrite, 
        RegDst, MemtoReg, ALUsrc, MemWrite, MemRead;    
    // END CONTROL

    wire[31:0] jump_addr, branch_addr;

    wire[31:0] pc_4 = pc + 32'd4;
    wire[31:0] instruction;
    // Synth faster if using ternary (pc_update..)
    always@(posedge clk)
    begin
        if(rst)
        begin
            pc <= 32'd0;
        end
        else
        begin
            if (Jump)
            begin
                pc <= jump_addr;
            end
            else if (Branch && zero)
            begin
                pc <= branch_addr;
            end
            else
            begin
                pc <= pc_4;
            end
        end
    end

    // The same code can run anywhere (all jumps relative), portable.
    // Shift 2 since 32B byte addressable and use as offset.
    // Can only branch back within the pc 4 bit addressable (can set to 0 min).
    assign jump_addr = { pc_4[31:28], instruction[25:0], 2'b00 }; // Shift left 2, 2'b00 at right.
    
    wire[31:0] instruction_se = { 16[instruction[15]], instruction[15:0] };
    // This is not limited to pc addressable, is conditional beq
    assign branch_addr = pc_4 + { instruction_se, 2'b00 }; // Shift 2 l


    IMem inst_memory
    (
        .r_addr(pc),
        .instruction(instruction)
    );

    wire [31:0] reg1;
    wire [31:0] reg2;
    wire [31:0] mem_read_data;
    wire [31:0] alu_result;
    Rfile reg_file
    (
        .reg1_addr( instruction[25:21] ),
        .reg2_addr( instruction[20:16] ),
        .w_addr( RegDst ? instruction[15:11] : instruction[20:16] ),
        .w_data( MemtoReg ? mem_read_data : alu_result ),
        .wen(RegWrite),
        .reg1_out(reg1),
        .reg2_out(reg2),
        .clk(clk),
        .rst(rst)
    );

    ALU alu
    (
        .data1(reg1),
        .data2(ALUSrc ? instruction_se : reg2),
        .result(alu_result),
        .control(instruction[31:26]),
        .zero(Zero)
    );

    DMem data_memory
    (
        .addr(alu_result),
        .data_in(reg2),
        .data_out(mem_read_data),
        .mem_write(MemWrite),
        .mem_read(MemRead),
        .clk(clk)
    );
endmodule