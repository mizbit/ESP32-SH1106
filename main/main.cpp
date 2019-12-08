#include <string.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"

#include "sdkconfig.h" // generated by "make menuconfig"
#include "oled.h"

#define tag "SH1106"

extern "C" {
	void app_main(void);
	}

void i2c_master_init(int sda, int scl)
	{
	i2c_config_t i2c_config;
	i2c_config.mode = I2C_MODE_MASTER;
	i2c_config.sda_io_num = (gpio_num_t)sda;
	i2c_config.scl_io_num = (gpio_num_t)scl;
	i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
	i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
	i2c_config.master.clk_speed = 1000000;
	i2c_param_config(I2C_NUM_0, &i2c_config);
	i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
	}

static const uint8_t bitmap[] =
	{
		1,2,4,8,16,32,64,128,128,128,192,192,128,128,128,64,32,16,8,4,2,1, // first page (8 vertical bits, 22 columns)
		255,255,255,255,15,15,15,15,15,15,15,15,15,15,15,15,15,15,255,255,255,255 // second page (8 vertical bits, 22 columns)
	};



i2c_cmd_handle_t cmd;

void OLED::i2c_start()
	{
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	}
void OLED::i2c_stop()
	{
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	}
bool OLED::i2c_send(uint8_t byte)
	{
	i2c_master_write_byte(cmd, byte, true);
	return false;
	}
void delay(int x)
	{
	vTaskDelay(x / portTICK_PERIOD_MS);
	}
OLED display(0x3c, 128, 64, true);


void contrast(int value)
	{
	char buffer[4];
	display.clear();
	display.draw_string(0, 0, "Contrast:");
	itoa(value, buffer, 10);
	display.draw_string(64, 0, buffer);
	display.draw_rectangle(0, 20, value / 2, 31, OLED::SOLID);
	display.display();
	display.set_contrast(value);
	delay(500);
	}
void loop()
	{

	display.clear();

// Draw hollow circles
for (uint_least8_t radius = 3; radius < 62; radius += 3)
	{
	delay(50);
	display.draw_circle(64, 16, radius);
	if (radius > 15)
		{
		display.draw_circle(64, 16, radius - 15, OLED::SOLID, OLED::BLACK);
		}
	display.display();
	}

// Draw solid circles
delay(500);
display.draw_circle(36, 16, 14, OLED::SOLID);
display.display();
delay(500);
display.draw_circle(36, 16, 7, OLED::SOLID, OLED::BLACK);
display.display();

// Draw rectangles
delay(500);
display.draw_rectangle(64, 0, 98, 31);
display.display();
delay(500);
display.draw_rectangle(69, 5, 93, 26, OLED::SOLID);
display.display();

// scroll up
delay(1000);
display.scroll_up(32, 20);

// Draw text with normal size
display.draw_string(4, 2, "Hello");
display.display();

// Draw a line
delay(1000);
display.draw_line(4, 10, 34, 10);
display.display();

// Draw text from program memory with double size
delay(1000);
display.draw_string(16, 15, "World!", OLED::DOUBLE_SIZE);
display.display();

// Draw a cross
delay(1000);
display.draw_line(16, 31, 88, 15);
display.draw_line(16, 15, 88, 31);
display.display();

// Draw a raw bitmap
delay(1000);
display.draw_bitmap_P(100, 8, 22, 16, bitmap);
display.display();

// Demonstrate scrolling
delay(1000);
display.set_scrolling(OLED::HORIZONTAL_RIGHT);
delay(3000);
display.set_scrolling(OLED::HORIZONTAL_LEFT);
delay(3000);
display.set_scrolling(OLED::NO_SCROLLING);

// Redraw after scrolling to get the original picture
display.display();

// Flash the display
delay(1000);
for (int i = 0; i < 10; i++)
	{
	display.set_invert(true);
	delay(200);
	display.set_invert(false);
	delay(200);
	}

// Show contrast values
contrast(128);
contrast(64);
contrast(32);
contrast(16);
contrast(8);
contrast(4);
contrast(2);
contrast(1);
contrast(2);
contrast(4);
contrast(8);
contrast(16);
contrast(32);
contrast(64);
contrast(128);
contrast(255);
contrast(128);
delay(3000);

display.clear();
}




void app_main(void)
	{
	i2c_master_init(22, 21);
	display.begin();
	while (1)
		{
		loop();
		}
	}