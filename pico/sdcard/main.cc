#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
//#include "hardware/adc.h"
//#include "hardware/clocks.h"
//#include "hardware/flash.h"
#include "hardware/gpio.h"
//#include "hardware/irq.h"
//#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "tusb.h" // if you want to use tud_cdc_connected()

//#include "../../1306/pico-sdk/oled.h"

#define spi		spi1
#define	PIN_SCK		10
#define	PIN_MOSI	11
#define PIN_MISO 	12
#define	PIN_CS 		15


#define CMD_TIMEOUT 200 // number of tries before getting bored
#define R1_IDLE_STATE (1<<0)
#define R1_ILLEGAL_COMMAND (1<<2)

#define SDTOUT -2 // timeout waiting for card to be ready
#define SDROUT -3 // timeout waiting for card to respond to command
#define SDCMD0 -4 // failed CMD0 (card couldn't be set to idle state)
#define SDCMD8 -5 // failed CMD8 (identify card version (only version 2 is handled))
#define SDFV2  -6 // failed to initialise card as version 2 type card
#define SDCMD16 -7 // CMD16 failed to set card block size to 512


void cs_low() {	gpio_put(PIN_CS, 0); }

void cs_high() { gpio_put(PIN_CS, 1); }

typedef uint8_t u8;

int cdv = 512; // card is byte addressing, set to 1 if addresses are per block

//u8 ocr[4]; // 
#if 0
void simple_write_read(uint8_t* src, uint8_t* dst, int len)
{
	gpio_put(PIN_CS, 0);
	spi_write_read_blocking(spi, src, dst, len);
	gpio_put(PIN_CS, 1);
}

void simple_write(const uint8_t* buf, int len)
{
	gpio_put(PIN_CS, 0);
	spi_write_blocking(spi, buf, len);
	gpio_put(PIN_CS, 1);
}

/* read into dst, continually sending val */
void simple_read(uint8_t* dst, int len, uint8_t val)
{
	uint8_t src[len];
	for(int i = 0; i< len; i++) src[i] = val;
	simple_write_read(src, dst, len);
}


uint8_t read_byte(uint8_t send)
{
	uint8_t src = send, dst;
	simple_write_read(&src, &dst, 1);
	return dst;
}

#endif

int wait_for_ready()
{
	u8 dst;
	uint32_t start = time_us_32();
	while(time_us_32() - start < 300'000) {
		spi_read_blocking(spi, 0xFF, &dst, 1);
		if(dst == 0xFF) return 0;
	}
	return SDTOUT;

}

class Trans {
	public:
		Trans();
		~Trans();
};

Trans::Trans()
{
	cs_low();
}

Trans::~Trans()
{
	cs_high();
	uint8_t b = 0xFF;
	spi_write_blocking(spi, &b, 1); // just spin our wheels so that the card can complete its operation
}

void call_cmd(int cmd, int arg, int crc)
{
	uint8_t buf[6];
	buf[0] = 0x40 | cmd;
	buf[1] = (arg >> 24) & 0xFF;
	buf[2] = (arg >> 16) & 0xFF;
	buf[3] = (arg >> 8) & 0xFF;
	buf[4] = (arg >> 0) & 0xFF;
	buf[5] = crc;

	//simple_write(buf, sizeof(buf));
	spi_write_blocking(spi, buf, sizeof(buf));
}

int CMD_T1(int cmd, int arg, int crc)
{
	Trans t;
	//if(wait_for_ready()) return SDTOUT;
	wait_for_ready();
	call_cmd(cmd, arg, crc);


	//if(skip1)  read_byte(0xFF);

	// wait for response[7] == 0
	uint8_t resp;
	for(int i = 0; i< CMD_TIMEOUT; i++) {
		spi_read_blocking(spi, 0xFF, &resp, 1);
		//simple_read(buf, 1, 0xFF);
		//if(buf[0] & 0x80) continue;

		if(!(resp & 0x80)) return resp; 
	}

	//printf("latest response is %d\n", (int) resp);



	return SDROUT;
}

int CMD_T2(int cmd, int arg, int crc, u8 quad[4])
{
	Trans t;
	//if(wait_for_ready()) return SDTOUT;
	int status = wait_for_ready();
	if(status) return SDTOUT;

	call_cmd(cmd, arg, crc);

	// wait for response[7] == 0
	uint8_t resp;
	for(int i = 0; i< CMD_TIMEOUT; i++) {
		spi_read_blocking(spi, 0xFF, &resp, 1);
		//simple_read(buf, 1, 0xFF);
		//if(buf[0] & 0x80) continue;

		if(!(resp & 0x80)) {
			//uint8_t resp_buf[4];
			//printf("Got a response from CMD8");
			spi_read_blocking(spi, 0xFF, quad, 4);
			return resp;
		}


	}

	return SDCMD8;
}

int init_card_v2()
{
	u8 ocr[4];
	for(int i=0; i< CMD_TIMEOUT; i++) {
		printf("init_card_v2 try %d\n", i);
		sleep_ms(50);
		CMD_T2(58, 0, 0xFD, ocr);
		CMD_T1(55, 0, 0x65);
		constexpr int arg = 0x40000000;
		static_assert((arg>0) && (sizeof(int)>=4));
		if(CMD_T1(41, arg, 0x77) == 0) {
			CMD_T2(58, 0, 0xFD, ocr);
			if((ocr[0] & 0x40) != 0) cdv = 1;
			return 0;
		}
	}
	return SDFV2;
}

int init_card()
{
	// standard spi stuff
	int spi_speed = 1'200'000;
	spi_speed = 400'000;
	spi_init(spi, spi_speed);
	//spi_set_slave(spi0, true);
	//spi_set_format(spi0, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	//gpio_set_function(PIN_CS,   GPIO_FUNC_SPI);
	//gpio_put(PIN_CS, 1);
	gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
	gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
	gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
	gpio_set_dir(PIN_CS, GPIO_OUT);

	uint8_t b;

	// clock card at least 80 cycles with cs high
	cs_high();
	for(int i = 0; i < 16; i++) spi_write_blocking(spi, &b, 1);
	//{
	//	uint8_t b = 0xFF;
	//	simple_write(&b, 1);
	//}

	// CMD0 go idle
	// max 5 attempts to reach idle state
	int status;
	for(int i = 0; i < 10; i++) {
		printf("\nidle attempt %d\n", i);
		status = CMD_T1(0, 0, 0x95);
		printf("status=%d\n", status);
		if(status == R1_IDLE_STATE) {
			//ssd1306_print(" IDLE ");
			break;
		}
	}
	if(status == R1_IDLE_STATE) {
		printf("CMD0 succeeded\n");
	} else {
		printf("CMD0 failed. aborting\n");
		return SDCMD0;
	}

	// CMD8: card version
	// We only consider version 2 cards at this point
	u8 cmd8_response[4];
	status = CMD_T2(8, 0x01aa, 0x87, cmd8_response);
	printf("\nCMD8 card status %d\n", status);
	if(status != R1_IDLE_STATE) 
		return SDCMD8;

	// we are assuming version 2 cards
	printf("Attempting init_card_v2()\n");	
	status = init_card_v2();
	if(status) return status;

	// skip CMD9 that adafruit uses
	

	// CMD16 set block length to 512 bytes
	if(CMD_T1(16, 512, 0x15) != 0) return SDCMD16;
	printf("CMD16 set block size to 512 successfully\n");


	return 0;



}

int main() 
{
	stdio_init_all();
	while(!tud_cdc_connected()) sleep_ms(250); // wait for usb serial 

	//init_display(64, 6);
	//init_spi();
	int status = init_card();
	if(status) 
		printf("Card init failed with status %d\n", status);
	else
		printf("Card init successfully\n");

	//ssd1306_print("1");
	//show_scr();


#define BTN  14 // GPIO number, not physical pin
#define LED  25 // GPIO of built-in LED
	gpio_init(BTN);
	gpio_set_dir(BTN, GPIO_IN);
	gpio_pull_up(BTN);
	// gpio_get() gets state of pin

	gpio_init(LED);
	gpio_set_dir(LED, GPIO_OUT);

	int i = 0;
	for(;;) {
		//printf("Hello number %d\n", i++);
		gpio_put(LED, 1);
		sleep_ms(100);
		gpio_put(LED, 0);
		sleep_ms(1000);		
	}

	return 0;
}

