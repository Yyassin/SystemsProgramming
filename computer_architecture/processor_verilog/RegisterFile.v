module RegisterFile
(
    input[4:0] reg1_addr,
    input[4:0] reg2_addr,
    input[4:0] w_addr,
    input[31:0] w_data,
    input wen,
    output[31:0] reg1_out,
    output[31:0] reg2_out,
    input clk, rst
);
    reg[31:0] registers[31:0];

    integer i;
    always@(posedge clk) begin
        if (rst) begin
            for (i = 0; i < 32; i = i + 1) registers[i] <= 8'd0;
        end
    end

    always@(posedge clk) begin
        if (wen) begin
            registers[w_addr] <= w_data;
        end
    end

    assign reg1_out = registers[reg1_addr];
    assign reg2_out = registers[reg2_addr];
endmodule

