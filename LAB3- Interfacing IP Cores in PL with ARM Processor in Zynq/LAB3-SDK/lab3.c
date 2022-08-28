
// LAB2 Template Program
// EEM464 SoC Design Lab

#include "xparameters.h"
#include "xgpio.h"

#define Switch_channel 2
#define Button_channel 1
#define Led_channel 1
XGpio dip_switches;   // instance of  gpio driver for switch  gpio 0
//XGpio push_buttons;   // instance of  gpio driver for buttons gpio 1

int main (void)
{
	
	//int push_buttons_check;
	int dip_switches_check;
	int led_check;
	int status;
    int k =0;
    xil_printf("-- Entering the Program --\r\n");

    status = XGpio_Initialize(&dip_switches, XPAR_AXI_GPIO_0_DEVICE_ID); // XPAR_SWITCHES_DEVICE_ID parameter has been defined in xparameters.h
	if (status != XST_SUCCESS)
    	return XST_FAILURE;
	
	XGpio_SetDataDirection(&dip_switches, Switch_channel, 0xffffffff); // Setting the direction of the GPIO unit as input by assigning it to 0xffffffff

	/*status = XGpio_Initialize(&push_buttons, XPAR_AXI_GPIO_1_DEVICE_ID); // XPAR_BUTTONS_DEVICE_ID parameter has been defined in xparameters.h
	if (status != XST_SUCCESS)
    	return XST_FAILURE;
	
	XGpio_SetDataDirection(&push_buttons, Button_channel, 0xffffffff); // Setting the direction of the GPIO unit as input by assigning it to 0xffffffff
*/
	/*Led direction*/
	XGpio_SetDataDirection(&dip_switches, Led_channel, 0x00000000); // seting direction of the GPIO unit as output

	while (1) // infinite loop
	{
	 // push_buttons_check = XGpio_DiscreteRead(&push_buttons, Button_channel);
	  //xil_printf("Push Buttons Status %x\r\n", push_buttons_check);
	  dip_switches_check = XGpio_DiscreteRead(&dip_switches, Switch_channel);
	  xil_printf("DIP Switch Status %x\r\n", dip_switches_check);
	 // XGpio_DiscreteWrite(&push_buttons, Button_channel, push_buttons_check);
	 XGpio_DiscreteWrite(&dip_switches, Led_channel, dip_switches_check);

	  //XGpio_DiscreteWrite(&dip_switches, Led_channel, dip_switches_check);
	  //sleep(1);
	}

	xil_printf("-- Exiting the Program --\r\n"); // This will never be displayed!

}

