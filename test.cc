#include <unistd.h>

#include <iostream>

#include "gutil.h"

using namespace std;
using namespace bluez;

int main() {
    int ret = EXIT_SUCCESS;

    BluezUtil util;

    /*
    g_message("adapter state = %d", util.GetAdapterState());
    util.StartDiscovery();
    g_message("adapter state = %d", util.GetAdapterState());
    util.StopDiscovery();
    g_message("adapter state = %d", util.GetAdapterState());
    */

    auto devices = util.GetDevices();

    auto d = devices.cbegin();
    while (d != devices.cend()) {
        auto str = (*d++)->to_string();
        cout << str << endl;
    }

    util.RegisterListener([](BluetoothEvent event, const BluetoothDevice *device) {
        cout << "event = " << BluetoothEventValue(event) << ", adapter ? " << (event == EV_ADAPTER) << ", device = " << device << endl;
    });

    util.StartDiscovery();

    while (1) {
        sleep(10);
    }

    return ret;
}
