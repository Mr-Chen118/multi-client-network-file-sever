#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include<stdio.h>
#include<sys/types.h>

#define MAX_CLIENT 64

ssize_t recv_line(int fd, char *buf, size_t size);

int send_all(int fd, char *buf, size_t size);

#endif