#include <unistd.h>

#include <iostream>

#include "gutil.h"

using namespace std;
using namespace bluez;

#define BLUEZ "org.bluez"
#define BLUEZ_MANAGER_IFACE "org.freedesktop.DBus.ObjectManager"
#define BLUEZ_PROPERTY_IFACE "org.freedesktop.DBus.Properties"
#define BLUEZ_ADAPTER_IFACE "org.bluez.Adapter1"
#define BLUEZ_DEVICE_IFACE "org.bluez.Device1"
#define BLUEZ_ADAPTER_OBJ "/org/bluez/hci0"

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
    /*
    auto devices = util.GetDevices();

    auto d = devices.cbegin();
    while (d != devices.cend()) {
        auto str = (*d++)->to_string();
        cout << str << endl;
    }

    util.RegisterListener([](BluetoothEvent event, const BluetoothDevice *device) {
        cout << "event = " << (int)event << ", adapter ? " << (event == EV_ADAPTER) << ", device = " << device << endl;
    });

    util.StartDiscovery();

    GMainLoop *loop = g_main_loop_new(nullptr, false);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);
*/
    return ret;
}
