#ifndef _GUTIL_H_
#define _GUTIL_H_

#include <gio/gio.h>

#include <functional>
#include <list>
#include <memory>

namespace bluez {

class BluetoothDevice;
class BluezUtil;
enum BluetoothEvent {
    EV_ADAPTER_OFF,
    EV_ADAPTER_ON,
    EV_ADAPTER_DISCOVERY_OFF,
    EV_ADAPTER_DISCOVERY_ON,
    EV_DEVICE_FOUND,
    EV_DEVICE_REMOVE,
    EV_DEVICE_UNPAIRED,
    EV_DEVICE_UNPAIRING,
    EV_DEVICE_PAIRING,
    EV_DEVICE_PAIRED,
    EV_DEVICE_DISCONNECTED,
    EV_DEVICE_DISCONNECTING,
    EV_DEVICE_CONNECTING,
    EV_DEVICE_CONNECTED,
};

using BluetoothDeviceRef = std::unique_ptr<BluetoothDevice>;
using BluetoothEventCallback = std::function<void(BluetoothEvent, const BluetoothDevice *)>;

class BluetoothDevice {
    friend class BluezUtil;

  private:
    GDBusProxy *proxy;
    char *name_ = nullptr;
    char *address_ = nullptr;
    bool paired;
    bool connected;
    void update_property();
    const char *object_path();
    explicit BluetoothDevice(GDBusConnection *conn, const char *object_path);
    explicit BluetoothDevice(GDBusConnection *conn, const char *object_path, const char *name, const char *address, bool paired, bool connected);

  public:
    const char *name();
    const char *address();

    ~BluetoothDevice();

    bool Connect();
    bool Disconnect();
    bool Pair();
    std::string to_string();
};

class BluezUtil {
  private:
    GDBusConnection *conn;
    GDBusProxy *object_manager;
    GDBusProxy *adapter_proxy;
    guint adapter_handle;
    guint device_handle;
    guint iface_added_handle;
    guint iface_removed_handle;
    BluetoothEventCallback callback;
    static void adapter_callback(GDBusConnection *, const gchar *, const gchar *, const gchar *, const gchar *, GVariant *, gpointer);
    static void device_callback(GDBusConnection *, const gchar *, const gchar *, const gchar *, const gchar *, GVariant *, gpointer);
    static void iface_added_callback(GDBusConnection *, const gchar *, const gchar *, const gchar *, const gchar *, GVariant *, gpointer);
    static void iface_removed_callback(GDBusConnection *, const gchar *, const gchar *, const gchar *, const gchar *, GVariant *, gpointer);

  public:
    explicit BluezUtil();
    ~BluezUtil();
    void RegisterListener(BluetoothEventCallback callback);
    int GetAdapterState();
    int GetDeviceState(const BluetoothDeviceRef &device);
    // adapter methods
    bool StartDiscovery();
    bool StopDiscovery();
    std::list<BluetoothDeviceRef> GetDevices();
    // device methods
    bool Connect(const char *object_path);
    bool Disconnect(const char *object_path);
    bool Pair(const char *object_path);
};

} // namespace bluez

#endif