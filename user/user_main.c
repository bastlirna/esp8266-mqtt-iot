#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"

#include "user_network.h"
#include "user_interface.h"

struct rst_info rtc_info;

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

os_event_t user_procTaskQueue[user_procTaskQueueLen];
//static void user_procTask(os_event_t *events);

static void ICACHE_FLASH_ATTR task_hello(os_event_t *events) {
	//print text to the serial output
	os_printf("Hello World!\r\n");
}


void ICACHE_FLASH_ATTR user_init() {
	// Initialize UART0
	//uart_div_modify(0, UART_CLK_FREQ / 115200);

	system_rtc_mem_read(0, &rtc_info, sizeof(struct rst_info));
	if (rtc_info.flag == 1 || rtc_info.flag == 2) {
		ESP_DBG("flag = %d,epc1 = 0x%08x,epc2=0x%08x,epc3=0x%08x,excvaddr=0x%08x,depc=0x%08x,\nFatal exception (%d): \n",
				rtc_info.flag, rtc_info.epc1, rtc_info.epc2, rtc_info.epc3,
				rtc_info.excvaddr, rtc_info.depc, rtc_info.exccause);
	}
	struct rst_info info = { 0 };

	system_rtc_mem_write(0, &info, sizeof(struct rst_info));

	init_network();

	//Set up the hello world task
	system_os_task(task_hello, user_procTaskPrio, user_procTaskQueue,
			user_procTaskQueueLen);
	//send a message to the "hello world" task to activate it
	system_os_post(user_procTaskPrio, 0, 0);
}
