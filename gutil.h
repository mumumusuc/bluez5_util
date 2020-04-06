#ifndef _GUTIL_H_
#define _GUTIL_H_

#include <gio/gio.h>

#include <functional>
#include <list>
#include <memory>

namespace bluez {

class BluetoothDevice;
class BluezUtil;
enum class BluetoothEvent : int;

using BluetoothDeviceRef = std::unique_ptr<BluetoothDevice>;
using BluetoothEventCallback = std::function<void(BluetoothEvent, const BluetoothDevice *)>;
using BluetoothEventType = std::underlying_type<BluetoothEvent>::type;

class BluetoothDevice {
    friend class BluezUtil;

  private:
    GDBusProxy *proxy;
    char *name_ = nullptr;
    char *address_ = nullptr;
    //bool paired;
    //bool connected;
    //void update_property();
    explicit BluetoothDevice(GDBusConnection *conn, const char *object_path);
    explicit BluetoothDevice(GDBusConnection *conn, const char *object_path, const char *name, const char *address);

  public:
    const char *name() const;
    const char *address() const;
    const bool paired() const;
    const bool connected() const;
    const char *object_path() const;
    int state() const;

    ~BluetoothDevice();

    bool Connect();
    bool Disconnect();
    bool Pair();
    std::string to_string();
};

class BluezUtil {
  private:
    GMainLoop *loop;
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

enum class BluetoothEvent : int {
    EV_NONE = 0,
    EV_ADAPTER_OFF = 0x21,
    EV_ADAPTER_ON = 0x22,
    EV_ADAPTER_DISCOVERY_OFF = 0x23,
    EV_ADAPTER_DISCOVERY_ON = 0x24,
    EV_DEVICE_FOUND = 0x11,
    EV_DEVICE_REMOVE = 0x12,
    EV_DEVICE_UNPAIRED = 0x13,
    EV_DEVICE_UNPAIRING = 0x14,
    EV_DEVICE_PAIRING = 0x15,
    EV_DEVICE_PAIRED = 0x16,
    EV_DEVICE_DISCONNECTED = 0x17,
    EV_DEVICE_DISCONNECTING = 0x18,
    EV_DEVICE_CONNECTING = 0x19,
    EV_DEVICE_CONNECTED = 0x1a,
};
static const int EV_ADAPTER = 0x20;
static const int EV_DEVICE = 0x10;
bool operator==(const BluetoothEvent &p1, int p2);

// namespace bluez
} // namespace bluez

#endif