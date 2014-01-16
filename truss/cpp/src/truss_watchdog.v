/*
   Trusster Open Source License version 1.0a (TRUST)
   copyright (c) 2006 Mike Mintz and Robert Ekendahl.  All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * Redistributions in any form must be accompanied by information on how to obtain
 complete source code for this software and any accompanying software that uses this software.
 The source code must either be included in the distribution or be available in a timely fashion for no more than
 the cost of distribution plus a nominal fee, and must be freely redistributable under reasonable and no more
 restrictive conditions. For an executable file, complete source code means the source code for all modules it
 contains. It does not include source code for modules or files that typically accompany the major components
 of the operating system on which the executable file runs.


 THIS SOFTWARE IS PROVIDED BY MIKE MINTZ AND ROBERT EKENDAHL ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 OR NON-INFRINGEMENT, ARE DISCLAIMED. IN NO EVENT SHALL MIKE MINTZ AND ROBERT EKENDAHL OR ITS CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
`timescale 1ns / 1ns

module watchdog(output timeout);
parameter TIMEOUT              = 10_000_000; //Default time-out is 10 ms
parameter POST_TIMEOUT         = 100;       //Default post time-out count is 100 ns
parameter COUNTER_WIDTH        = 64;        //Number of bits in time-out counter
parameter POST_COUNTER_WIDTH   = 8;         //Number of bits in final time-out counter
parameter CLK_PERIOD           = 1;         //Default clock period length


reg [COUNTER_WIDTH-1:0]       counter;
reg [POST_COUNTER_WIDTH-1:0]  post_counter;
reg                           clk;
reg                           timeout_reg;

assign timeout = timeout_reg;

initial begin
    counter       = TIMEOUT;
    post_counter  = POST_TIMEOUT;
    clk           = 0;
    timeout_reg   = 0;
end

always #CLK_PERIOD clk = ~clk;

// Time-out "event" - Signals truss that test has taken to long (default = 5 ms)
always @(posedge clk) begin
    if (counter == 0) begin
        timeout_reg = 1;
    end
    else begin
        counter = counter -2;
    end
end

// Post counter "event" - Shuts down simulation as a last resort if truss/teal don't notice the time-out event
always @(posedge clk) begin
    if (timeout_reg == 1) begin
        if (post_counter == 0) begin
            $display("Error! [%t][%m] Time-out occured and final counter exired. System must be hung! Shutting down simulation\n", $time);
            $finish;
        end
        else begin
            post_counter = post_counter -2; //use -2 to make POST_TIMEOUT be in "ns".
        end
    end
end
endmodule
