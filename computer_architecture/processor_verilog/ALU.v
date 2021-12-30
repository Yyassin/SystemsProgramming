`include "Instructions.v"

module ALU(
    input[31:0] dataX,
    input[31:0] dataY,
    output[31:0] result,
    input[2:0] control,
    output zero
);
    
    assign result = (control == `ADD) ? dataX + dataY :
                    (control == `SUB) ? dataX - dataY :
                    (control == `AND) ? dataX & dataY :
                    (control == `OR)  ?  dataX | dataY:
                    (control == `XOR) ? dataX ^ dataY :
                    32'd0;
            
    assign zero = (result == 32'd0);
endmodule
