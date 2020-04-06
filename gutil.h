#ifndef _GUTIL_H_
#define _GUTIL_H_

#include <gio/gio.h>

#include <functional>
#include <list>
#include <memory>
#include <mutex>

namespace bluez {

class BluetoothDevice;
class BluezUtil;
enum class BluetoothEvent : int;

using BluetoothDeviceRef = std::unique_ptr<BluetoothDevice>;
using BluetoothEventCallback = std::function<void(BluetoothEvent, const BluetoothDevice *)>;
using BluetoothEventType = std::underlying_type<BluetoothEvent>::type;

class BluezError {
  public:
    const int code;
    const std::string message;
    explicit BluezError(int code, const char *msg) noexcept;
    explicit BluezError(const GError *error) noexcept;
    ~BluezError() = default;
};

class BluetoothDevice {
    friend class BluezUtil;

  private:
    GDBusProxy *proxy;
    char *name_;
    char *address_;
    explicit BluetoothDevice(GDBusConnection *conn, const char *object_path) throw(BluezError);
    explicit BluetoothDevice(GDBusConnection *conn, const char *object_path, const char *name, const char *address) throw(BluezError);

  public:
    const char *name() const noexcept;
    const char *address() const noexcept;
    const bool paired() const throw(BluezError);
    const bool connected() const throw(BluezError);
    const char *object_path() const noexcept;
    BluetoothEvent state() const throw(BluezError);

    ~BluetoothDevice();

    void Connect() throw(BluezError);
    void Disconnect() throw(BluezError);
    void Pair() throw(BluezError);
    std::string to_string() throw(BluezError);
};

class BluezUtil {
  private:
    std::mutex loop_mutex;
    GMainLoop *loop;
    GDBusConnection *conn;
    GDBusProxy *object_manager;
    GDBusProxy *adapter_proxy;
    guint adapter_handle;
    guint device_handle;
    guint iface_added_handle;
    guint iface_removed_handle;
    BluetoothEventCallback callback;
    static void adapter_callback(GDBusConnection *, const gchar *, const gchar *, const gchar *, const gchar *, GVariant *, gpointer) noexcept;
    static void device_callback(GDBusConnection *, const gchar *, const gchar *, const gchar *, const gchar *, GVariant *, gpointer) noexcept;
    static void iface_added_callback(GDBusConnection *, const gchar *, const gchar *, const gchar *, const gchar *, GVariant *, gpointer) noexcept;
    static void iface_removed_callback(GDBusConnection *, const gchar *, const gchar *, const gchar *, const gchar *, GVariant *, gpointer) noexcept;

  public:
    explicit BluezUtil() throw(BluezError);
    ~BluezUtil();
    void RegisterListener(BluetoothEventCallback callback) noexcept;
    BluetoothEvent GetAdapterState() throw(BluezError);
    // adapter methods
    void StartDiscovery() throw(BluezError);
    void StopDiscovery() throw(BluezError);
    std::list<BluetoothDeviceRef> GetDevices() throw(BluezError);
    // device methods
    void Connect(const char *object_path) throw(BluezError);
    void Disconnect(const char *object_path) throw(BluezError);
    void Pair(const char *object_path) throw(BluezError);
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
bool operator==(const BluetoothEvent &p1, int p2) noexcept;
int BluetoothEventValue(const BluetoothEvent &event) noexcept;
// namespace bluez
} // namespace bluez

#endif