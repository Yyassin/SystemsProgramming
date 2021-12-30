`timescale 1ns / 1ns
`include "Cache.v"

`define GREEN(x) `"\033[32m\x\033[0m`"
`define CYAN(x) `"\033[36m[x]\033[0m`"
`define RED(x) `"\033[31m[x]\033[0m`"
`define YELLOW(x) `"\033[33m[x]\033[0m`"

module Cache_tb;

    /* Testing IO setup */
    reg Clk, Rst, PR_req, PW_req, MR_rdy, MW_done;
    reg[7:0] PW_data, PAddr;
    reg[31:0]MR_data;
    wire PR_rdy, PW_done, MR_req, MW_req;
    wire[7:0] PR_data, MW_data, MAddr;

    // 64 word memory bank for testing, 16 blocks
    reg[7:0] memory[0:63];

    Cache dut
    (
        // Synchronous Logic Inputs
        .clk(Clk),
        .rst(Rst),

        // IProcessor
        .PRead_request(PR_req),
        .PWrite_request(PW_req),
        .PWrite_data(PW_data),
        .PRead_data(PR_data),
        .PRead_ready(PR_rdy),
        .PWrite_done(PW_done),
        .PAddress(PAddr),

        // IMemory
        .MRead_request(MR_req),
        .MWrite_request(MW_req),
        .MWrite_data(MW_data),
        .MRead_data(MR_data),
        .MRead_ready(MR_rdy),
        .MWrite_done(MW_done), 
        .MAddress(MAddr)
    );

    integer i; // Memory init loop index var
    initial begin
        /* Waveform Setup */
        $dumpfile("Cache_tb.vcd");
        $dumpvars(0, Cache_tb);

        /* --- Initialization Reset --- */
        // Set all memory words to 0
        for (i = 0; i < 64; i = i + 1) memory[i] = i; //$urandom%255

        // Reset inputs
        PR_req = 1'b0;
        PW_req = 1'b0;
        MR_rdy = 1'b0;
        MW_done = 1'b0;
        PW_data = 8'd0;
        PAddr = 8'd0;
        MR_data = 32'd0;

        Clk = 1'b0;
        Rst = 1'b1;
        #5;
        
        /* --- Test Cases --- */
        @(posedge Clk);
        Rst = 1'b0;
        #10;

        /** Read miss
            Description: Should result in a cache miss at 0b101,
            fetch from 0b100 and return the memory.
        */
        $display("\n%s", `GREEN(Read request miss @ 0b101));
        @(posedge Clk);
        PAddr = 8'b00000101;    // Fetch 0b00000101
        PR_req = 1'b1;          // Won't hit, should fetch from mem @ [4]
        #1;
        
        @(posedge MR_req);
        @(posedge Clk);
        $display("%s Memory read request @0b%b", `YELLOW(INFO) ,MAddr);
        MR_data = { memory[MAddr + 8'd3], memory[MAddr + 8'd2], memory[MAddr + 8'd1], memory[MAddr] };
        MR_rdy = 1'b1;
        #1;

        @(posedge PR_rdy);
        @(posedge Clk);
        MR_rdy = 1'b0;
        $display("%s Cache returned: 0b%b, should be: 0b%b.", `CYAN(PASS), PR_data, memory[PAddr]);
        #1;
        @(posedge Clk);
        PR_req = 1'b0; // ack
        #1;

        /** Read miss
            Description: Should result in a cache miss at 0b101,
            fetch from 0b100 and return the memory.
        */
        $display("\n%s", `GREEN(Read request miss @ 0b11));
        @(posedge Clk);
        PAddr = 8'b00000011;    // Fetch @0b00000011
        PR_req = 1'b1;          // Won't hit, should fetch from mem @ [0]
        #1;
        
        @(posedge MR_req);
        @(posedge Clk);
        $display("%s Memory read request @0b%b", `YELLOW(INFO) ,MAddr);
        MR_data = { memory[MAddr + 8'd3], memory[MAddr + 8'd2], memory[MAddr + 8'd1], memory[MAddr] };
        MR_rdy = 1'b1;
        #1;

        @(posedge PR_rdy);
        @(posedge Clk);
        MR_rdy = 1'b0;
        $display("%s Cache returned: 0b%b, should be: 0b%b.", `CYAN(PASS), PR_data, memory[PAddr]);
        #1;
        @(posedge Clk);
        PR_req = 1'b0; // ack
        #1;

        /** Read hit
            Description: Should result in a cache hit at 0b111, 
            and return the memory.
        */
        $display("\n%s", `GREEN(Read request hit @ 0b111));
        @(posedge Clk);
        PAddr = 8'b00000111;    // Fetch @0b00000111 
        PR_req = 1'b1;          // should hit
        #1;

        @(posedge PR_rdy);
        @(posedge Clk);
        $display("%s Cache returned: 0b%b, should be: 0b%b.", `CYAN(PASS), PR_data, memory[PAddr]);
        #1;
        @(posedge Clk);
        PR_req = 1'b0; // ack
        #1;

        /** Read hit
            Description: Should result in a cache hit at 0b1, 
            and return the memory.
        */
        $display("\n%s", `GREEN(Read request hit @ 0b1));
        @(posedge Clk);
        PAddr = 8'b00000001;    // Fetch @0b0000001
        PR_req = 1'b1;          // should hit
        #1;

        @(posedge PR_rdy);
        @(posedge Clk);
        $display("%s Cache returned: %b, should be: %b.", `CYAN(PASS), PR_data, memory[PAddr]);
        #1;
        @(posedge Clk);
        PR_req = 1'b0; // ack
        #1;

        /** Read miss
            Description: Should result in a cache miss at 0b100110 (38).
            This maps to block 1 (starting with 36) and should trash.
        */
        $display("\n%s", `GREEN(Read request miss @ 0b100110));
        @(posedge Clk);
        PAddr = 8'b00100110;    // Fetch @0b00100110
        PR_req = 1'b1;          // Won't hit, should fetch from mem @ [36]
        #1;
        
        @(posedge MR_req);
        @(posedge Clk);
        $display("%s Memory read request @0b%b", `YELLOW(INFO), MAddr);
        MR_data = { memory[MAddr + 8'd3], memory[MAddr + 8'd2], memory[MAddr + 8'd1], memory[MAddr] };
        MR_rdy = 1'b1;
        #1;

        @(posedge PR_rdy);
        @(posedge Clk);
        MR_rdy = 1'b0;
        $display("%s Cache returned: %b, should be: %b.", `CYAN(PASS), PR_data, memory[PAddr]);
        #1;
        @(posedge Clk);
        PR_req = 1'b0; // ack
        #1;

        /** Read miss
            Description: Should result in a cache miss at 0b101.
            Also maps to block 1, was in before but just got trashed.
        */
        $display("\n%s", `GREEN(Read request miss @ 0b101 (trashed block 1)));
        @(posedge Clk);
        PAddr = 8'b00000101;    // Fetch @0b00000101
        PR_req = 1'b1;          // Won't hit, should fetch from mem @ [4]
        #1;
        
        @(posedge MR_req);
        @(posedge Clk);
        $display("%s Memory read request @0b%b", `YELLOW(INFO), MAddr);
        MR_data = { memory[MAddr + 8'd3], memory[MAddr + 8'd2], memory[MAddr + 8'd1], memory[MAddr] };
        MR_rdy = 1'b1;
        #1;

        @(posedge PR_rdy);
        @(posedge Clk);
        MR_rdy = 1'b0;
        $display("%s Cache returned: 0b%b, should be: 0b%b.", `CYAN(PASS), PR_data, memory[PAddr]);
        #1;
        @(posedge Clk);
        PR_req = 1'b0; // ack
        #1;

        /** Write hit
            Description: Should result in a cache hit at 0b10.
            Should update both cache and memory with the value
            0b11111011 (0xfb).
        */
        $display("\n%s", `GREEN(Write request hit 0b11111011 => 0b10));
        @(posedge Clk);
        PAddr = 8'b00000010;
        PW_req = 1'b1;
        PW_data = 8'hfb;
        #1;

        @(posedge MW_req);
        @(posedge Clk);
        memory[MAddr] <= MW_data;
        MW_done <= 1'b1;
        #1;

        @(posedge PW_done);
        MW_done <= 1'b0;
        #1;
        @(posedge Clk);
        PW_req = 1'b0; // ack
        #1;

        @(posedge Clk);
        PAddr = 8'b00000010;    // Fetch @0b0000010 
        PR_req = 1'b1;          // should hit
        #1;

        @(posedge PR_rdy);
        $display("%s Following write hit. Memory has: 0b%b, Cache returned: 0b%b should be: 0b%b.", `CYAN(PASS), memory[PAddr], PR_data, PW_data);
        #1;
        @(posedge Clk);
        PR_req = 1'b0; // ack
        #1;

        /** Write miss
            Description: Should result in a cache miss at 0b100010 (34) => mblock8 => cblock0.
            Should update just memory with the value
            0b10101011 (0xab). Cache should still hit with block 0 values.
        */
        $display("\n%s", `GREEN(Write request miss 0b10101011 => 0b100010));
        @(posedge Clk);
        PAddr = 8'b00100010;
        PW_req = 1'b1;
        PW_data = 8'hab;
        #1;

        @(posedge MW_req);
        @(posedge Clk);
        memory[MAddr] <= MW_data;
        MW_done <= 1'b1;
        PW_req = 1'b0;
        #1;
        @(posedge Clk);
        #1; // Update cache

        @(posedge Clk);
        PAddr = 8'b00000010;    // Fetch @0b00000010
        PR_req = 1'b1;          // should hit
        #1;

        @(posedge PR_rdy);
        PR_req = 1'b0;
        $display("%s Cache returned: 0b%b, should be: 0b%b.", `CYAN(PASS), PR_data, memory[PAddr]); // From the hit write before
        $display("%s Following write miss, memory updated: 0b%b, should be 0b%b.", `CYAN(PASS), memory[8'b00100010], 8'hab);
        #1;
        @(posedge Clk);
        PR_req = 1'b0; // ack
        #1;

        @(posedge Clk) $finish;
    end

    always Clk = #1 ~Clk;
endmodule
