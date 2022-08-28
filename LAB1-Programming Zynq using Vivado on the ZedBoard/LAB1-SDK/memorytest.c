/******************************************************************************
* Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <stdio.h>
#include "xparameters.h"
#include "xil_types.h"
#include "xstatus.h"
#include "xil_testmem.h"

#include "platform.h"
#include "memory_config.h"
#include "xil_printf.h"

/*
 * memory_test.c: Test memory ranges present in the Hardware Design.
 *
 * This application runs with D-Caches disabled. As a result cacheline requests
 * will not be generated.
 *
 * For MicroBlaze/PowerPC, the BSP doesn't enable caches and this application
 * enables only I-Caches. For ARM, the BSP enables caches by default, so this
 * application disables D-Caches before running memory tests.
 */
#define N =10
#define M = 7
void putnum(unsigned int num);
u32 *w = (u32*) 0x01000000;
u32 *x = (u32*) 0x010000DF;
u32 *result = (u32*) 0x02000000;

void Mult(){
	u32 i ,j=0;
	u32 result1;

	for (i = 0; i < 7; i++) {
		*(w+i) =1;
	}

	for (i = 0; i < 10; i++) {
		*(x+i) = 1;

	}

	for (i = 0; i < 10; i++) {
		result1=0;
		for (j = 0; j < 7; j++) {
				result1+=x[i+j]*w[j];
		}
		*(result+i)=result1;

		xil_printf("\n\r Answer of result=%d\r\n",result);
	}
}





int main()
{
    //sint32 i;

    init_platform();
    Mult();
    //Mult((u32*)(memory_ranges[0].base));
    print("--Starting Memory Test Application--\n\r");
    print("NOTE: This application runs with D-Cache disabled.");
    print("As a result, cacheline requests will not be generated\n\r");

    /*for (i = 0; i < n_memory_ranges; i++) {
        test_memory_range(&memory_ranges[i]);
    }*/

    print("--Memory Test Application Complete--\n\r");
    print("Successfully ran Memory Test Application");
    cleanup_platform();
    return 0;
}
