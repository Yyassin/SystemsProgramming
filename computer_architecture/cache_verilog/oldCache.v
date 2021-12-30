/*H**********************************************************************
* FILENAME :        Cache.v            
*
* DESCRIPTION :
*       Module definition for Direct Mapped Cache. The Cache has a total size
*       of 32 bytes, consisting of 8 cache blocks with 4 bytes per block.
*
* MICRO ARCHITECTURE :
*       The Cache module implements a finite state machine with 4 states. The initial
*       state is the IDLE state where the component is waiting for a MR command. 
*
*       Once a command is received, it is recorded as either WRITE or READ. The internal
*       state transitions to the TEST state where we test for a cache hit. Once the test is 
*       completed, the system transitions to the R_FETCH or W_FETCH according to the input command.\
*
*       If a READ command was placed, the system transitions to the R_FETCH state. If we have a cache hit,
*       the request memory word is returned from cache. Otherwise, in the case of a miss, the requested
*       address and it's associated memory block is fetched, cached, and the requested word is returned.
*
*       If a WRITE command was placed, the system transitions to the W_FETCH state. If we have cache hit, 
*       the corresponding word in the cache is written to in addition to the associated word in memory. Otherwise,
*       cache misses are handled by only updating the associated word in memory.
*
* AUTHOR :    Yousef Yassin        START DATE :     8 Oct 21
*H*/

/* Cache FSM States */
`define IDLE 2'd0
`define TEST 2'd1
`define R_FETCH 2'd2
`define W_FETCH 2'd3

/* Input Command Type */
`define READ 1'b0
`define WRITE 1'b1

module Cache
(
    // Synchronous Logic Inputs
    input clk,
    input rst,

    // IProcessor
    input PRead_request,
    input PWrite_request,
    input[7:0] PWrite_data,
    output[7:0] PRead_data,
    output PRead_ready,
    output PWrite_done,
    input[7:0] PAddress,

    //IMemory
    output MRead_request,
    output MWrite_request,
    output[7:0] MWrite_data,
    input[31:0] MRead_data,
    input MRead_ready,
    input MWrite_done, 
    output[7:0] MAddress
);

/*
    Cache and memory blocks consist of 4 bytes.
    8-bit memory address spaces => 256 total words, 64 blocks
    Cache is 32 bytes => 8 blocks

    4 bytes / block => 2-bit offset (o)
    8 cache blocks => 3-bit index (i)
    tag is remainder => 3-bit tag, makes sense since (t)
    8 memory blocks map to each cache block.

    Address Format: 0b tttiiioo
*/

/*
    8 blocks, each with 4 bytes.
    Indexed: cache[block][byte][bit]
*/
reg[7:0] cache[0:7][0:3]; 
reg[2:0] tags[0:7];     // Eight 3-bit tags, for each cache block
reg valid[0:7];         // Tracks invalid data at init

reg[1:0] state;         // Internal state
reg current_command;    // Input command
reg hit;                // 1 if cache hit, else 0

/* Split input PAddress into tag, index and offset bits */
wire[2:0] tag;
wire[2:0] block_index;
wire[1:0] offset;
assign tag = PAddress[7:5];
assign block_index = PAddress[4:2];
assign offset = PAddress[1:0];

/* Internal registers mapped to module outputs */
reg[7:0] PRead_data_reg;
reg[7:0] MWrite_data_reg;
reg[7:0] MAddress_reg;
assign PRead_data = PRead_data_reg;
assign MWrite_data = MWrite_data_reg;
assign MAddress = MAddress_reg;

reg PRead_ready_reg, PWrite_done_reg, MRead_request_reg, MWrite_request_reg;
assign PRead_ready = PRead_ready_reg;
assign PWrite_done = PWrite_done_reg;
assign MRead_request = MRead_request_reg;
assign MWrite_request = MWrite_request_reg;

/* Initialization, Cache Reset */
integer i, j;
always@(posedge clk) begin
    if (rst) begin
        state = `IDLE;
        current_command = `READ; // Temp, will be overriden before use
        hit = 1'b0;
        PRead_data_reg = 8'd0;
        MWrite_data_reg = 8'd0;
        PRead_ready_reg = 1'b1;
        PWrite_done_reg = 1'b0;
        MAddress_reg = 8'd0;
        MRead_request_reg = 1'b0;

        // Initialize reg arrays to 0
        for (i = 0; i < 8; i = i + 1) begin
            tags[i] = 3'b000;
            valid[i] = 1'b0;
            for (j = 0; j < 4; j = j + 1) cache[i][j] <= 8'd0;
        end
    end
end

 /* Main - Cache State Machine */
always@(posedge clk) begin
    /** IDLE State
     * Wait for command, when received, transition to TEST if 
     * cache is ready.
     */
    if (state == `IDLE) begin
        if ((PRead_request || PWrite_request) && PRead_ready_reg) begin
            state <= `TEST;
            PRead_ready_reg <= 1'b0;
            current_command = PRead_request ? `READ : `WRITE; 
        end 
    end

    /** TEST State 
     * Check if requested address is in cache and transition to 
     * fetch state.
     */
    else if (state == `TEST) begin
        // Cache hit on read, return cached word immediately to save CC
        if ((tag == tags[block_index]) & valid[block_index] & ~current_command) begin
            $display("%c[1;33m [INFO] %c[0m Read hit at address: 0b %b.", 27, 27, PAddress);
            PRead_data_reg <= cache[block_index][offset];
            PRead_ready_reg <= 1'b1;
            state <= `IDLE;
        end
        // Either missed or writing, store hit state and transition
        else begin
            hit <= ((tag == tags[block_index]) & valid[block_index]);
            state <= current_command ? `W_FETCH : `R_FETCH; 
        end
    end

    /** R_FETCH State 
     * 
     */
    else if (state == `R_FETCH) begin
        // Cache miss, fetch associated block and return word
        // Fetch from block base, so mask off offsets.
        MAddress_reg <= PAddress & 8'b11111100; 
        MRead_request_reg <= 1'b1;
        // Wait for memory to be ready
        if (MRead_ready) begin
            $display("%c[1;33m [INFO] %c[0m Read missed at address: 0b %b", 27, 27, PAddress);
            $display("%c[1;33m [INFO] %c[0m Cached: %b %b %b %b", 27, 27, MRead_data[7:0], MRead_data[15:8], MRead_data[23:16],  MRead_data[31:24]);

            // Litte Endian
            cache[block_index][0] <= MRead_data[7:0];
            cache[block_index][1] <= MRead_data[15:8];
            cache[block_index][2] <= MRead_data[23:16];
            cache[block_index][3] <= MRead_data[31:24];

            tags[block_index] <= tag;
            valid[block_index] <= 1'b1;

            // Could use a case and return the requested word here
            // but will switch to test (hit now) for simplicity.
            // Note: wastes a clock cycle.
            
            MRead_request_reg <= 1'b0;
            state <= `TEST;
        end
    end

    /** W_FETCH State 
     * 
     */
    else if (state == `W_FETCH) begin
        // Write to memory in both cases
        MAddress_reg <= PAddress;
        MWrite_data_reg <= PWrite_data;
        MWrite_request_reg <= 1'b1;

        // Write back: Update memory and cache
        if (hit) begin
            cache[block_index][offset] <= MWrite_data_reg;
        end

        // Memory write cleanup after write complete
        if (MWrite_done) begin
            // Displays here to only show once, when write is done.
            if (hit) 
                $display("%c[1;33m [INFO] %c[0m Write hit at address: 0b %b.", 27, 27, PAddress);
            else 
                $display("%c[1;33m [INFO] %c[0m Write miss at address: 0b %b", 27, 27, PAddress);

            PWrite_done_reg <= 1'b1;
            PRead_ready_reg <= 1'b1;
            MWrite_request_reg <= 1'b0;
            state <= `IDLE; 
        end
    end
end

endmodule
