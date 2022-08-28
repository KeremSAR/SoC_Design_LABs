
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
#include "xparameters.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xtmrctr.h"
#include "xil_io.h"

#define BTN_INT XGPIO_IR_CH1_MASK 	/* 0x1 < Mask for the 1st channel */
#define TMR_LOAD 0xFFFFFF9B //0xFFFE795F //0xF8000000//0xFFFFFF9B //0xF8000000//0xFFFFFF9B
// 0xF01AFD55 2sn
XGpio LEDInst, BTNInst;
XScuGic INTCInst;
static int led_data;
static int btn_value;
XTmrCtr TMRInst;
static int tmr_data=0;
//static int tmr_data2=0;
int button_pressed =0;
int result1 =0;
#define INTC_DEVICE_ID XPAR_PS7_SCUGIC_0_DEVICE_ID
#define BTNS_DEVICE_ID XPAR_AXI_GPIO_0_DEVICE_ID
#define TMR_DEVICE_ID XPAR_TMRCTR_0_DEVICE_ID
#define LEDS_DEVICE_ID XPAR_AXI_GPIO_1_DEVICE_ID
#define INTC_GPIO_INTERRUPT_ID XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR
#define INTC_TMR_INTERRUPT_ID XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR

static void BTN_Intr_Handler(void *InstancePtr);
static int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int IntcInitFunction(u16 DeviceId, XTmrCtr *TmrInstancePtr, XGpio *GpioInstancePtr);
static void TMR_Intr_Handler(void *InstancePtr);
int x=0;
void TMR_Intr_Handler(void *InstancePtr)
{
	if (XTmrCtr_IsExpired(&TMRInst, 0)){

		XTmrCtr_Stop(&TMRInst, 0);


		 if (button_pressed == 1 || button_pressed ==2) {
			tmr_data++;
		}

		else {
			tmr_data=0;
			button_pressed =0;
		}

	}
		XGpio_DiscreteWrite(&LEDInst, 1, led_data);
		XTmrCtr_Reset(&TMRInst, 0);
		XTmrCtr_Start(&TMRInst, 0);

}


int i =0;
void BTN_Intr_Handler(void *InstancePtr)
{
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&BTNInst, BTN_INT);
	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(&BTNInst) & BTN_INT) != BTN_INT) {
		//(void) XGpio_InterruptClear(&BTNInst, BTN_INT);
		 //  XGpio_InterruptEnable(&BTNInst, BTN_INT);
		return;
	}
	btn_value = XGpio_DiscreteRead(&BTNInst, 1);
	// Increment counter based on button value
	// Reset if center button pressed
	if(btn_value != 0){
		button_pressed++;
	xil_printf("button handler %d\n",button_pressed);
	xil_printf("elapse time %d\n", tmr_data);
	}
	/*if(btn_value != 8)
		led_data = led_data + btn_value;
	else
		led_data = 0;*/

	XGpio_DiscreteWrite(&LEDInst, 1, led_data);
	(void) XGpio_InterruptClear(&BTNInst, BTN_INT);
	for (i = 0; i < 1000; ++i);
	// Enable GPIO interrupts
	XGpio_InterruptEnable(&BTNInst, BTN_INT);
}

int IntcInitFunction(u16 DeviceId,  XTmrCtr *TmrInstancePtr,XGpio *GpioInstancePtr) {

	XScuGic_Config *IntcConfig;
	int status;
	// Interrupt controller initialization

	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig,IntcConfig->CpuBaseAddress);

	if(status != XST_SUCCESS) return XST_FAILURE;
	// Call to interrupt setup
	status = InterruptSystemSetup(&INTCInst);

	if(status != XST_SUCCESS) return XST_FAILURE;

	// Connect GPIO interrupt to handler
	status = XScuGic_Connect(&INTCInst, INTC_GPIO_INTERRUPT_ID,(Xil_ExceptionHandler) BTN_Intr_Handler,(void *)GpioInstancePtr);

	if(status != XST_SUCCESS) return XST_FAILURE;

	// Connect timer interrupt to handler
	status = XScuGic_Connect(&INTCInst, INTC_TMR_INTERRUPT_ID,(Xil_ExceptionHandler)TMR_Intr_Handler,(void *)TmrInstancePtr);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Enable GPIO interrupts interrupt
	XGpio_InterruptEnable(GpioInstancePtr, 1);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// Enable GPIO interrupts in the controller
	XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID);

	XScuGic_Enable(&INTCInst, INTC_TMR_INTERRUPT_ID);

	return XST_SUCCESS;
}
int InterruptSystemSetup(XScuGic *XScuGicInstancePtr)
{
	// Enable interrupt
	XGpio_InterruptEnable(&BTNInst, BTN_INT);

	XGpio_InterruptGlobalEnable(&BTNInst);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler) XScuGic_InterruptHandler,XScuGicInstancePtr);

	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

int main()
{
    init_platform();
    printf("Entering to code \n");
    int status;
    // Initialize LEDs
    status = XGpio_Initialize(&LEDInst, LEDS_DEVICE_ID);

    if(status != XST_SUCCESS) return XST_FAILURE;
    // Initialize Push Buttons
    status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);

    if(status != XST_SUCCESS) return XST_FAILURE;
    // Set LEDs direction to outputs

    XGpio_SetDataDirection(&LEDInst, 1, 0x00);
    // Set all buttons direction to inputs
    XGpio_SetDataDirection(&BTNInst, 1, 0xFF);

    //----------------------------------------------------
    // SETUP THE TIMER
    //----------------------------------------------------
    status = XTmrCtr_Initialize(&TMRInst, TMR_DEVICE_ID);
    if(status != XST_SUCCESS)
    return XST_FAILURE;
    XTmrCtr_SetHandler(&TMRInst, (XTmrCtr_Handler)TMR_Intr_Handler, &TMRInst);
    XTmrCtr_SetResetValue(&TMRInst, 0, TMR_LOAD);
    XTmrCtr_SetOptions(&TMRInst, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

    // Initialize interrupt controller
    status = IntcInitFunction(INTC_DEVICE_ID,&TMRInst, &BTNInst);

    if(status != XST_SUCCESS) return XST_FAILURE;

    XTmrCtr_Start(&TMRInst, 0);
    while(1);//{
    	//sleep(1);
    //}

    cleanup_platform();
    return 0;
}
