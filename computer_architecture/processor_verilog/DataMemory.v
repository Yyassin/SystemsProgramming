module DataMemory
(
    input[31:0] addr,
    input[31:0] data_in,
    output[31:0] data_out,
    input mem_w,
    input mem_r,
    input clk, rst
);

    reg[7:0] memory[255:0];

    integer i;
    always@(posedge clk) begin
        if (rst) begin
            for (i = 0; i < 256; i = i + 1) memory[i] <= 8'd0;
        end
    end 

    // Write 32B at a time
    always@(posedge clk) begin
        if (mem_w) begin
            memory[addr + 32'd3] <= data_in[31:24];
            memory[addr + 32'd2] <= data_in[23:16];
            memory[addr + 32'd1] <= data_in[15:8];
            memory[addr] <= data_in[7:0];
        end
    end

    assign data_out = mem_r ? {
        memory[addr + 32'd3],
        memory[addr + 32'd2],
        memory[addr + 32'd1],
        memory[addr]
    } : 32'd0;
endmodule
