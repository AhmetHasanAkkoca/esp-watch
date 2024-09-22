
// Set  time and date function for easier use of the module
void updateRTC( uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t dow) {
  Ds1302::DateTime dt = {
    .year = year,
    .month = month,
    .day = day,
    .hour = hour,
    .minute = minute,
    .second = second,
    .dow = dow
  };
  rtc.setDateTime(&dt);
}