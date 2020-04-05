#include <Python.h>

#include <iostream>

#include "util.h"

typedef struct Device {
    char* name;
    char* address;
    char* path;
    int state;
} device_t;

int main() {
    int ret = EXIT_SUCCESS;
   
    BluezUtil util;
    util.Hello();
    auto devices = util.GetPairedDevices();
    std::cout << devices.size() << std::endl;
    return ret;
}