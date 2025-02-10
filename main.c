#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "inc/ssd1306.h"

#define DISPLAY_I2C_SDA 14
#define DISPLAY_I2C_SCL 15
#define DISPLAY_MAX_CHARS_PER_LINE 21
#define DISPLAY_LINE_HEIGHT 8

uint8_t ssd[ssd1306_buffer_length];

struct render_area frame_area = {
  start_column : 0,
  end_column : ssd1306_width - 1,
  start_page : 0,
  end_page : ssd1306_n_pages - 1
};

void display_ini();
void display_clr();
void display_put(char *string, int16_t x, int16_t y);

int main() {
  stdio_init_all();
  adc_init();
  
  display_ini();
  
  adc_gpio_init(26);

  while (1) {
    adc_select_input(0);

    unsigned int joystickY = adc_read();

    display_clr();

    display_put("a", 0, 0);
    display_put("b", 0, 1);
    display_put("c", 0, 2);

    if (joystickY < -2000) {
      
    } else if (joystickY > 2000) {

    }
  }

  return 0;
}

void display_ini() {
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(DISPLAY_I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(DISPLAY_I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(DISPLAY_I2C_SCL);

  ssd1306_init();
  calculate_render_area_buffer_length(&frame_area);
  display_clr();
}

void display_clr() {
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);
}

void display_put(char *string, int16_t x, int16_t y) {
  int current_line = 0;
  int str_len = strlen(string);
  int pos = 0;

  while (pos < str_len) {
    int chars_to_copy = str_len - pos;
    if (chars_to_copy > DISPLAY_MAX_CHARS_PER_LINE) {
      chars_to_copy = DISPLAY_MAX_CHARS_PER_LINE;
      for (int i = chars_to_copy - 1; i >= 0; i--) {
        if (string[pos + i] == ' ') {
          chars_to_copy = i;
          break;
        }
      }
      // Se não encontrar espaço, desenha a linha completa mesmo assim
      if (chars_to_copy == 0) chars_to_copy = DISPLAY_MAX_CHARS_PER_LINE;
    }

    // Cria um buffer local para a linha atual
    char line_buffer[DISPLAY_MAX_CHARS_PER_LINE + 1];
    strncpy(line_buffer, string + pos, chars_to_copy);
    line_buffer[chars_to_copy] = '\0'; // Null terminate é crucial

    ssd1306_draw_string(ssd, x, y + (current_line * DISPLAY_LINE_HEIGHT), line_buffer);

    pos += chars_to_copy;
    while (pos < str_len && string[pos] == ' ') {
      pos++;
    }
    current_line++;
  }

  render_on_display(ssd, &frame_area);
}