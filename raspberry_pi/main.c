#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

void delay(unsigned int milliseconds) {
  struct timeval t;
  t.tv_sec = milliseconds / 1000;
  t.tv_usec = (milliseconds % 1000) * 1000;
  select(0, NULL, NULL, NULL, &t);
}

int main(int argc, char *argv[]) {
  int sht3x;
  char data[8];
  double temperature = 0.0, humidity = 0.0;

  if (argc != 3) {
    printf("Usage: %s [i2c device path] [i2c address]\n", argv[0]);
    return 0;
  }

  if ((sht3x = open(argv[1], O_RDWR)) < 0) {
    printf("Fail to open device.\n");
    return 0;
  }

  if (ioctl(sht3x, I2C_SLAVE, atoi(argv[2])) < 0) {
    close(sht3x);
    return 0;
  }

  // soft-reset
  data[0] = 0x30;
  data[1] = 0xA2;
  write(sht3x, data, 2);
  delay(1000);

  while (1) {
    data[0] = 0x2C;
    data[1] = 0x06;
    write(sht3x, data, 2);

    delay(500);
    read(sht3x, data, 6);

    temperature = (double)data[0] * 256.0 + (double)data[1];
    temperature = -45.0 + (175.0 * temperature / 65535.0);
    humidity = 100.0 * ((double)data[3] * 256.0 + (double)data[4]) / 65535.0;

    printf("Temperature: %f, Humidity: %f\n", temperature, humidity);

    delay(500);
  }

  return 0;
}
