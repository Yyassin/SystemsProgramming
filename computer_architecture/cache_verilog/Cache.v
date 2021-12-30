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

`define PURPLE(x) `"\033[35m[x]\033[0m`"

/* Cache FSM States */
`define IDLE 3'd0
`define TEST 3'd1
`define R_FETCH 3'd2
`define R_FETCH_RST 3'd3
`define R_READY 3'd4
`define W_FETCH 3'd5
`define W_FETCH_RST 3'd6
`define W_DONE 3'd7

/* Input Command Type */
`define READ 1'b0
`define WRITE 1'b1

module Cache
(
    // Standard Synchronous Signals
    input clk,
    input rst,

    // Interface Processor
    input PRead_request, PWrite_request,
    input[7:0] PWrite_data, PAddress,
    output[7:0] PRead_data,
    output PRead_ready, PWrite_done,

    //Interface Memory
    output MRead_request, MWrite_request,
    output[7:0] MWrite_data, MAddress,
    input[31:0] MRead_data,
    input MRead_ready, MWrite_done
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

reg[2:0] state;         // Internal state
// Design Choice: avoids repetitives states R_xx and W_xx.
reg command;            // Input command type
reg hit;                // 1 if cache hit, else 0
reg[7:0] read_buf;      // Temporary data

/* Split input PAddress into tag, index and offset bits */
wire[2:0] tag;
wire[2:0] block_index;
wire[1:0] offset;
assign tag = PAddress[7:5];
assign block_index = PAddress[4:2];
assign offset = PAddress[1:0];

/* Initialization, Cache Reset */
integer i, j; // Loop vars
always@(posedge clk) begin
    if (rst) begin
        state <= `IDLE;
        command <= `READ;
        read_buf <= 8'd0;

        // Initialize reg arrays to 0
        for (i = 0; i < 8; i = i + 1) begin
            tags[i] <= 3'b000;
            valid[i] <= 1'b0;
            for (j = 0; j < 4; j = j + 1) cache[i][j] <= 8'd0;
        end
    end
end

/* Main - Cache State Machine */
always@(posedge clk) begin
    case (state) 
        `IDLE: begin
            if (PRead_request || PWrite_request) begin
                command = PRead_request ? `READ : `WRITE; 
                state <= `TEST;
            end 
            else begin
                state <= `IDLE;
            end
        end

        `TEST: begin
            // We can skip some states if we hit on a read
            if ((tag == tags[block_index] && valid[block_index]) && ~command) begin
                $display("%s Read cache hit @ 0b%b", `PURPLE(META), PAddress);
                read_buf <= cache[block_index][offset];
                state <= `R_READY;
            end
            else begin
                hit <= (tag == tags[block_index] && valid[block_index]);
                state <= command ? `W_FETCH : `R_FETCH;
            end
        end

        `R_FETCH: begin
            if (MRead_ready) begin
                $display("%s Read cache missed @ 0b%b", `PURPLE(META), PAddress);
                // Litte Endian
                cache[block_index][0] <= MRead_data[7:0];
                cache[block_index][1] <= MRead_data[15:8];
                cache[block_index][2] <= MRead_data[23:16];
                cache[block_index][3] <= MRead_data[31:24];

                tags[block_index] <= tag;
                valid[block_index] <= 1'b1;

                state <= `R_FETCH_RST;
            end
        end

        `R_FETCH_RST: begin
            read_buf <= cache[block_index][offset];
            state <= `R_READY;
        end

        `R_READY: begin
            // Return to idle once processor ack
            if (~PRead_request) begin
                state <= `IDLE;
            end
            else begin
                state <= `R_READY;
            end
        end

        `W_FETCH: begin
            if (MWrite_done) begin
                // Write back: Update memory and cache
                if (hit) begin
                    $display("%s Write cache hit @ 0b%b", `PURPLE(META), PAddress);
                    cache[block_index][offset] <= PWrite_data;
                end
                else begin
                    $display("%s Write cache missed @ 0b%b", `PURPLE(META), PAddress);
                end
                state <= `W_FETCH_RST;
            end 
            else begin
                state <= `W_FETCH;
            end
        end

        `W_FETCH_RST: begin
            state <= `W_DONE;
        end

        `W_DONE: begin
            // Return to idle once processor ack
            if (~PWrite_request) begin
                state <= `IDLE;
            end
            else begin
                state <= `W_DONE;
            end
        end
    endcase
end

// Cache miss, fetch associated block and return word
// Fetch from block base, so mask off offsets.
assign MAddress = (state == `R_FETCH) ? PAddress & 8'b11111100 : 
                  (state == `W_FETCH) ? PAddress : 
                  8'd0; 
 
assign MRead_request = (state == `R_FETCH);
assign MWrite_request = (state == `W_FETCH);
assign PRead_ready = (state == `R_READY);
assign PWrite_done = (state == `W_DONE);
assign PRead_data = read_buf;
assign MWrite_data = PWrite_data;
endmodule