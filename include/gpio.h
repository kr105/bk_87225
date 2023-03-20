#ifndef GPIO_H
#define GPIO_H

#define GPIO_DIR_IN 0
#define GPIO_DIR_OUT 1

// high priority or an error occured in SW
#define POLL_GPIO POLLPRI | POLLERR

void setup_gpio();
int gpio_poll_value(uint16_t pin);
int gpio_set_value(uint16_t pin, char *value);

#endif