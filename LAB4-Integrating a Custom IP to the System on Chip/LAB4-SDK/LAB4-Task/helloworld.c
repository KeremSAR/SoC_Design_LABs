/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_io.h"
#include "multiplier_ip.h"

#define ipAddr 0x43C00000
int a[1024];
int b[1024];
int c[1024];
int main()
{
    init_platform();
    int sum1=0;
    int b1=3;
    int c1=4;
    int a1=2;
    int i=0;
    int sum =0;
    sum1 += b1*c1+a1;
    xil_printf("\n sum : %d",sum1);
    print("Hello World\n\r");

    for (i = 0; i < 1023; ++i) {
        	a[i] = i%16;
        	b[i] =i;
        	c[i] =2*i;

        	//sum +=
    	}
    /*for (i = 0; i < 1023; ++i) {
    	a[i] = i%16;
    	b[i] =i;
    	c[i] =2*i;
    	MULTIPLIER_IP_mWriteReg(ipAddr,0,a[i]);
    	MULTIPLIER_IP_mWriteReg(ipAddr,4,b[i]);
    	MULTIPLIER_IP_mWriteReg(ipAddr,8,c[i]);
    	MULTIPLIER_IP_mWriteReg(ipAddr,16,1);
    	sum += MULTIPLIER_IP_mReadReg(ipAddr,12);
	}*/

    xil_printf("\n val:  %d", sum );

    MULTIPLIER_IP_mWriteReg(ipAddr,0,2);


	MULTIPLIER_IP_mWriteReg(ipAddr,4,5);

	MULTIPLIER_IP_mWriteReg(ipAddr,8,7);
	//sleep(1);
	MULTIPLIER_IP_mWriteReg(ipAddr,16,1);
	//sleep(1);
	xil_printf(" okunmasý gereken :%d\n\r", MULTIPLIER_IP_mReadReg(ipAddr,16) );

	xil_printf(" result okunan :%d\n\r", MULTIPLIER_IP_mReadReg(ipAddr,12) );


    print("Successfully ran Hello World application");
    cleanup_platform();
    return 0;
}
