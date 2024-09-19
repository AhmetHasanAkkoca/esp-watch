#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

// Chip information.
typedef struct {
  pin_t VCC;
  pin_t GND;
  pin_t CLK;
  pin_t DAT;
  pin_t RST;
  uint8_t year[2];
  uint8_t month[2];
  uint8_t date[2];
  uint8_t hour[2];
  uint8_t minute[2];
  uint8_t second[2];
  uint8_t day[2];
  uint8_t memory[31];
  uint8_t bcd_index;
  uint8_t bit_in;
  uint8_t bit_out;
  uint8_t cmd;
  uint8_t data;
  uint8_t byte_index;
} chip_data_t;

// Returns true if the power source is connected correctly.
bool power_connected(void *data)
{
  chip_data_t *chip = (chip_data_t*)data;
  return pin_read(chip->VCC) && !pin_read(chip->GND);
}

// Increments the clock once per second.
void chip_timer_callback(void *data) 
{
  chip_data_t *chip = (chip_data_t*)data;
  if (power_connected(chip))
  {
    if (chip->second[1] == 5 && chip->second[0] == 9)
    {
      if (chip->minute[0] != 9)
      {
        chip->second[1] = 0;
        chip->second[0] = 0;
        chip->minute[0]++;
      }
      else if (chip->minute[1] == 5 && chip->minute[0] == 9)
      {
        chip->minute[0] = 0;
        chip->minute[1] = 0;
        chip->second[0] = 0;
        chip->second[1] = 0;
        if (chip->hour[0] == 9)
        {
          chip->hour[0] = 0;
          chip->hour[1]++;
        }
        else if (chip->hour[1] == 2 && chip->hour[0] == 3)
        {
          chip->hour[1] = 0;
          chip->hour[0] = 0;
        }
        else
        {
          chip->hour[0]++;
        }
      }
      else if (chip->minute[1] != 5 && chip->minute[0] == 9)
      {
        chip->second[0] = 0;
        chip->second[1] = 0;
        chip->minute[0] = 0;
        chip->minute[1]++;
      }
    }
    else if (chip->second[0] == 9)
    {
      chip->second[0] = 0;
      chip->second[1]++;
    }
    else
    {
      chip->second[0]++;
    }
  }
}

// Writes to the given memory address.
void write_memory(void *user_data)
{
  chip_data_t *chip = (chip_data_t*)user_data;
  if (chip->byte_index < 31)
  {
    chip->memory[chip->byte_index - 1] = chip->data;
    chip->byte_index++;
  }
  else
  {
    chip->byte_index = 0;
  }
}

// Outputs the contents of a memory address on DAT pin during low clock cycle.
void read_memory(void *user_data)
{
  chip_data_t *chip = (chip_data_t*)user_data;
  if (chip->bit_out < 8)
  {
    uint8_t bit = (chip->memory[(chip->cmd - 0xC1) / 2] >> chip->bit_out) & 1;
    pin_write(chip->DAT, bit);
    chip->bit_out++;
  }
  else
  {
    chip->DAT = pin_init("DAT", INPUT);
    chip->cmd = 0;
    chip->bit_out = 0;
    chip->byte_index = 0;
  }
}

// Saves incoming date & time when 0xBE TIMEDATE_BURST command is received.
void write_time(void *user_data)
{
  chip_data_t *chip = (chip_data_t*)user_data;
  
  switch (chip->byte_index)
  {
    case 1:
      if (chip->bcd_index < 2)
      {
        chip->second[chip->bcd_index] = chip->data;
        chip->bcd_index++;
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->byte_index++;
      }
      break;
    case 2:
      if (chip->bcd_index < 2)
      {
        chip->minute[chip->bcd_index] = chip->data;
        chip->bcd_index++;
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->byte_index++;
      }
      break;
    case 3:
      if (chip->bcd_index < 2)
      {
        chip->hour[chip->bcd_index] = chip->data;
        chip->bcd_index++;
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->byte_index++;
      }
      break;
    case 4:
      if (chip->bcd_index < 2)
      {
        chip->date[chip->bcd_index] = chip->data;
        chip->bcd_index++;
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->byte_index++;
      }
      break;
    case 5:
      if (chip->bcd_index < 2)
      {
        chip->month[chip->bcd_index] = chip->data;
        chip->bcd_index++;
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->byte_index++;
      }
      break;
    case 6:
      if (chip->bcd_index < 2)
      {
        chip->day[chip->bcd_index] = chip->data;
        chip->bcd_index++;
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->byte_index++;
      }
      break;
    case 7:
      if (chip->bcd_index < 2)
      {
        chip->year[chip->bcd_index] = chip->data;
        chip->bcd_index++;
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->byte_index = 0;
      }
      break;
  }
}

// Outputs date & time on DAT pin after receiving 0xBF TIMEDATE_BURST command. 
void read_time(void *user_data)
{
  chip_data_t *chip = (chip_data_t*)user_data;
  switch (chip->byte_index)
  {
    case 1:
      if (chip->bcd_index < 2)
      {
        if (chip->bit_out < 4)
        {
          uint8_t bit = (chip->second[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
        else
        {
          chip->bit_out = 0;
          chip->bcd_index++;
          uint8_t bit = (chip->second[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->bit_out = 0;
        uint8_t bit = (chip->minute[chip->bcd_index] >> chip->bit_out) & 1;
        pin_write(chip->DAT, bit);
        chip->bit_out++;
        chip->byte_index++;
      }
      break;
    case 2:
      if (chip->bcd_index < 2)
      {
        if (chip->bit_out < 4)
        {
          uint8_t bit = (chip->minute[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
        else
        {
          chip->bit_out = 0;
          chip->bcd_index++;
          uint8_t bit = (chip->minute[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->bit_out = 0;
        uint8_t bit = (chip->hour[chip->bcd_index] >> chip->bit_out) & 1;
        pin_write(chip->DAT, bit);
        chip->bit_out++;
        chip->byte_index++;
      }
      break;
    case 3:
      if (chip->bcd_index < 2)
      {
        if (chip->bit_out < 4)
        {
          uint8_t bit = (chip->hour[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
        else
        {
          chip->bit_out = 0;
          chip->bcd_index++;
          uint8_t bit = (chip->hour[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->bit_out = 0;
        uint8_t bit = (chip->date[chip->bcd_index] >> chip->bit_out) & 1;
        pin_write(chip->DAT, bit);
        chip->bit_out++;
        chip->byte_index++;
      }
      break;
    case 4:
      if (chip->bcd_index < 2)
      {
        if (chip->bit_out < 4)
        {
          uint8_t bit = (chip->date[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
        else
        {
          chip->bit_out = 0;
          chip->bcd_index++;
          uint8_t bit = (chip->date[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->bit_out = 0;
        uint8_t bit = (chip->month[chip->bcd_index] >> chip->bit_out) & 1;
        pin_write(chip->DAT, bit);
        chip->bit_out++;
        chip->byte_index++;
      }
      break;
    case 5:
      if (chip->bcd_index < 2)
      {
        if (chip->bit_out < 4)
        {
          uint8_t bit = (chip->month[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
        else
        {
          chip->bit_out = 0;
          chip->bcd_index++;
          uint8_t bit = (chip->month[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->bit_out = 0;
        uint8_t bit = (chip->day[chip->bcd_index] >> chip->bit_out) & 1;
        pin_write(chip->DAT, bit);
        chip->bit_out++;
        chip->byte_index++;
      }
      break;
    case 6:
      if (chip->bcd_index < 2)
      {
        if (chip->bit_out < 4)
        {
          uint8_t bit = (chip->day[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
        else
        {
          chip->bit_out = 0;
          chip->bcd_index++;
          uint8_t bit = (chip->day[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->bit_out = 0;
        uint8_t bit = (chip->year[chip->bcd_index] >> chip->bit_out) & 1;
        pin_write(chip->DAT, bit);
        chip->bit_out++;
        chip->byte_index++;
      }
      break;
    case 7:
      if (chip->bcd_index < 2)
      {
        if (chip->bit_out < 4)
        {
          uint8_t bit = (chip->year[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
        else
        {
          chip->bit_out = 0;
          chip->bcd_index++;
          uint8_t bit = (chip->year[chip->bcd_index] >> chip->bit_out) & 1;
          pin_write(chip->DAT, bit);
          chip->bit_out++;
        }
      }
      if (chip->bcd_index == 2)
      {
        chip->bcd_index = 0;
        chip->bit_in = 0;
        chip->bit_out = 0;
        chip->cmd = 0;
        chip->data = 0;
        chip->byte_index = 0;
        chip->DAT = pin_init("DAT", INPUT);
      }
      break;
  }
}

// Starts read sequences or carries out write sequences based on command byte. 
void read_command(void *user_data, pin_t pin, uint32_t value)
{
  chip_data_t *chip = (chip_data_t*)user_data;
  
  switch (chip->cmd)
  {
    case 0xBE:
      write_time(user_data);
      break;
    default:
      if (chip->cmd >= 0xC0 && chip->cmd < 0xFE && chip->cmd % 2 == 0)
      { 
        chip->memory[(chip->cmd - 0xC0) / 2] = chip->data;
      }
      chip->byte_index = 0;
      break;
  }
  
  if (chip->byte_index == 0)
  {
    chip->byte_index++;
    chip->cmd = chip->data;
    if (pin == chip->CLK && value == LOW)
    {
      switch (chip->cmd)
      {
        case 0xBF:
          chip->DAT = pin_init("DAT", OUTPUT);
          read_time(user_data);
        default:
          if (chip->cmd >= 0xC0 && chip->cmd < 0xFE && chip->cmd % 2 != 0)
          { 
            chip->DAT = pin_init("DAT", OUTPUT);
            read_memory(user_data);
          }
          break;
      }
    }
  }

  chip->data = 0;
  chip->bit_in = 0;
}

// Reads incoming bits.
void read_incoming(void *user_data, pin_t pin, uint32_t value)
{
  uint8_t bits = 8;
  chip_data_t *chip = (chip_data_t*)user_data;
  uint8_t limit = (chip->cmd == 0xBE) ? 4 : 8;
  if (chip->bit_in < limit)
  {
    if (pin == chip->CLK && value == HIGH)
    {
      uint8_t binary [8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
      uint8_t add = pin_read(chip->DAT) * binary[chip->bit_in];
      chip->data += add;
      chip->bit_in++;
    }
  }
  else
  {
    read_command(user_data, pin, value);
  }
}

// Called whenever the clock pin changes state.
void chip_pin_change(void *user_data, pin_t pin, uint32_t value)
{
  chip_data_t *chip = (chip_data_t*)user_data;

  if (!power_connected(chip))
  {
    return;
  }

  switch(chip->cmd)
  {
    case 0xBF:
      if (pin == chip->CLK && value == LOW)
      {
        chip->DAT = pin_init("DAT", OUTPUT);
        read_time(user_data);
      }
      break;
    default:
      if (chip->cmd >= 0xC0 && chip->cmd < 0xFE && chip->cmd % 2 != 0)
      { 
        if (pin == chip->CLK && value == LOW)
        {
          chip->DAT = pin_init("DAT", OUTPUT);
          read_memory(user_data);
        }
      }
      else
      {
        read_incoming(user_data, pin, value);
      }
      break;
  }
}

// Initializes the chip.
void chip_init()
{
  chip_data_t *chip_data = (chip_data_t*)malloc(sizeof(chip_data_t));
  
  chip_data->VCC = pin_init("VCC", INPUT);
  chip_data->GND = pin_init("GND", INPUT);
  chip_data->CLK = pin_init("CLK", INPUT);
  chip_data->DAT = pin_init("DAT", INPUT);
  chip_data->RST = pin_init("RST", INPUT);

  const pin_watch_config_t watch_config = {
    .edge = BOTH,
    .pin_change = chip_pin_change,
    .user_data = chip_data,
  };
  pin_watch(chip_data->CLK, &watch_config);

  const timer_config_t config = 
  {
    .callback = chip_timer_callback,
    .user_data = chip_data,
  };

  timer_t timer_id = timer_init(&config);
  timer_start(timer_id, 1000000, true);
}
