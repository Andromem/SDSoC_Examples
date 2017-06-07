/*********** 
# Copyright (c) 2017, Xilinx, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.
#
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY 
# DIRECT, INDIRECT,INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO,PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# 
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
************/


/******************************************************************************

    This is a simple vector addition based example which showcases loop 
    pipelining feature of SDx tool chain.

******************************************************************************/



#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include "vector_addition.h"

const int TEST_DATA_SIZE = 1<<10;

// Compare software and hardware solutions
static int verify(int *gold, int *out, int size) {
    for(int i = 0; i < size; i++){
        if(gold[i] != out[i]){
            std::cout<< "Result Mismatch at index=" << i
                << " Expected=" << gold[i] 
                << " Actual=" << out[i] << "\n";
            return 1;
        }
  }
  return 0;
}


int main(int argc, char** argv)
{

    int test_passed = 0;
    int test_size = TEST_DATA_SIZE;

    // Create buffers using sds_alloc  
    int *a = (int *) sds_alloc(sizeof(int) * test_size);
    int *b = (int *) sds_alloc(sizeof(int) * test_size);
    int *hw_results = (int *) sds_alloc(sizeof(int) * test_size);

    // Software output buffer
    int *gold = (int *)malloc(sizeof(int) * test_size);

    //Creating Test Data and golden data
    for (int i = 0 ; i < test_size ; i++){
        a[i] = rand();  
        b[i] = rand();
        // Calculate Golden value
        gold[i] = a[i] + b[i]; 
        hw_results[i] = 0 ;     
    }
    
    sds_utils::perf_counter hw_ctr;

    hw_ctr.start();
    //Launch the Hardware Solution
    vadd_pipelined_accel(a, b, hw_results, test_size);
    hw_ctr.stop();
    
    test_passed = verify(gold, hw_results, test_size);

    uint64_t hw_cycles = hw_ctr.avg_cpu_cycles();

    std::cout << "Number of CPU cycles running application in hardware: "
				 << hw_cycles << std::endl;
   
    sds_free(a);
    sds_free(b);
    sds_free(hw_results);
    free(gold);

    std::cout << "TEST " << (test_passed ? "FAILED" : "PASSED") << std::endl;

    return (test_passed ? -1 : 0);
}
