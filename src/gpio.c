#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "gpio.h"

int gpio_export(uint16_t pin)
{
    int rtn;
    char str[8];

    sprintf(str, "%d", pin);

    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1)
    {
        printf("Unable to open /sys/class/gpio/export\n");
        return -1;
    }

    rtn = write(fd, str, strlen(str));

    // It returns EBUSY if the pin is already exported
    if (rtn == -1 && errno != EBUSY)
    {
        printf("Error %d writing %s to /sys/class/gpio/export: %s\n", errno, str, strerror(errno));
        return -1;
    }

    close(fd);

    return 1;
}

int gpio_set_direction(uint16_t pin, int direction)
{
    int fd, rtn;
    char gpio_path[40];

    sprintf(gpio_path, "/sys/class/gpio/gpio%d/direction", pin);

    fd = open(gpio_path, O_WRONLY);
    if (fd == -1)
    {
        printf("Unable to open %s\n", gpio_path);
        return -1;
    }

    if (direction == GPIO_DIR_IN)
    {
        rtn = write(fd, "in", 2);
    }
    else
    {
        rtn = write(fd, "out", 3);
    }

    if (rtn == -1)
    {
        printf("Error %d writing to %s: %s\n", errno, gpio_path, strerror(errno));
        return -1;
    }

    close(fd);

    return 1;
}

int gpio_set_edge(uint16_t pin, char *edge)
{
    int fd;
    char gpio_path[40];

    sprintf(gpio_path, "/sys/class/gpio/gpio%d/edge", pin);

    fd = open(gpio_path, O_WRONLY);
    if (fd == -1)
    {
        printf("Unable to open %s\n", gpio_path);
        return -1;
    }

    if (write(fd, edge, strlen(edge)) == -1)
    {
        printf("Error %d writing to %s: %s\n", errno, gpio_path, strerror(errno));
        return -1;
    }

    close(fd);

    return 1;
}

int gpio_set_value(uint16_t pin, char *value)
{
    int fd;
    char gpio_path[40];

    sprintf(gpio_path, "/sys/class/gpio/gpio%d/value", pin);

    fd = open(gpio_path, O_WRONLY);
    if (fd == -1)
    {
        printf("Unable to open %s\n", gpio_path);
        return -1;
    }

    if (write(fd, value, strlen(value)) == -1)
    {
        printf("Error %d writing to %s: %s\n", errno, gpio_path, strerror(errno));
        return -1;
    }

    close(fd);

    return 1;
}

int gpio_poll_value(uint16_t pin)
{
    int fd, poll_ret;
    struct pollfd poll_gpio;
    char value;
    char gpio_path[40];

    sprintf(gpio_path, "/sys/class/gpio/gpio%d/value", pin);

    fd = open(gpio_path, O_RDONLY);
    if (fd == -1)
    {
        printf("Unable to open %s\n", gpio_path);
        return -1;
    }

    poll_gpio.fd = fd;
    poll_gpio.events = POLL_GPIO;
    poll_gpio.revents = 0;

    while (1)
    {
        // consume any prior interrupt
        lseek(fd, 0, SEEK_SET);
        read(fd, &value, 1);

        poll_ret = poll(&poll_gpio, 1, -1);
        if (poll_ret == -1)
        {
            printf("Error %d at poll(): %s\n", errno, strerror(errno));
            return -1;
        }

        if ((poll_gpio.revents) & (POLL_GPIO))
        {
            lseek(fd, 0, SEEK_SET);
            read(fd, &value, 1);
            close(fd);
            return value - 0x30; // convert char number to int
        }
    }
}

void setup_gpio()
{
    gpio_export(36);
    gpio_set_direction(36, GPIO_DIR_OUT);

    gpio_export(43);
    gpio_set_direction(43, GPIO_DIR_OUT);

    gpio_export(44);
    gpio_set_direction(44, GPIO_DIR_IN);
    gpio_set_edge(44, "none"); // reset the edge setting
    gpio_set_edge(44, "falling");

    gpio_export(70);
    gpio_set_direction(70, GPIO_DIR_OUT);

    gpio_export(71);
    gpio_set_direction(71, GPIO_DIR_OUT);

    gpio_export(72);
    gpio_set_direction(72, GPIO_DIR_OUT);

    gpio_export(73);
    gpio_set_direction(73, GPIO_DIR_IN);

    gpio_export(200);
    gpio_set_direction(200, GPIO_DIR_OUT);

    gpio_export(201);
    gpio_set_direction(201, GPIO_DIR_OUT);
}