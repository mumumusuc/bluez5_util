#include "gutil.h"

#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#define BLUEZ "org.bluez"
#define BLUEZ_MANAGER_IFACE "org.freedesktop.DBus.ObjectManager"
#define BLUEZ_PROPERTY_IFACE "org.freedesktop.DBus.Properties"
#define BLUEZ_ADAPTER_IFACE "org.bluez.Adapter1"
#define BLUEZ_DEVICE_IFACE "org.bluez.Device1"
#define BLUEZ_ADAPTER_OBJ "/org/bluez/hci0"

#define TARGET_PATH "/org/bluez/hci0/dev_"
#define TARGET_DEVICE "org.bluez.Device1"

#define log(fmt, ...) g_message("[BluezUtil]" fmt "", __VA_ARGS__)

using namespace bluez;

bool bluez::operator==(const BluetoothEvent &p1, int p2) {
    return (static_cast<BluetoothEventType>(p1) & p2) == p2;
}

BluetoothDevice::BluetoothDevice(GDBusConnection *conn, const char *object_path, const char *name, const char *address)
    : name_(const_cast<char *>(name)), address_(const_cast<char *>(address)) {
    g_assert(conn && object_path);
    proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, nullptr, BLUEZ, object_path, BLUEZ_DEVICE_IFACE, nullptr, nullptr);
    if (!proxy) throw std::runtime_error("get device proxy error");
};

BluetoothDevice::BluetoothDevice(GDBusConnection *conn, const char *object_path)
    : BluetoothDevice{conn, object_path, nullptr, nullptr} {};

BluetoothDevice::~BluetoothDevice() {
    g_object_unref(proxy);
    free(name_);
    free(address_);
};

const char *BluetoothDevice::object_path() const {
    return g_dbus_proxy_get_object_path(proxy);
}

const char *BluetoothDevice::name() const {
    if (!name_) {
        auto name = g_dbus_proxy_get_cached_property(proxy, "Name");
        if (!name) return "";
        g_variant_get(name, "s", &name_);
        g_variant_unref(name);
    }
    return name_;
}

const char *BluetoothDevice::address() const {
    if (!address_) {
        auto address = g_dbus_proxy_get_cached_property(proxy, "Address");
        if (!address) return "";
        g_variant_get(address, "s", &address_);
        g_variant_unref(address);
    }
    return address_;
}

const bool BluetoothDevice::paired() const {
    auto value = g_dbus_proxy_get_cached_property(proxy, "Paired");
    auto paired = g_variant_get_boolean(value);
    g_variant_unref(value);
    return paired;
}

const bool BluetoothDevice::connected() const {
    auto value = g_dbus_proxy_get_cached_property(proxy, "Connected");
    auto connected = g_variant_get_boolean(value);
    g_variant_unref(value);
    return connected;
}

int BluetoothDevice::state() const {
    auto state = connected() ? BluetoothEvent::EV_DEVICE_CONNECTED : paired() ? BluetoothEvent::EV_DEVICE_PAIRED : BluetoothEvent::EV_NONE;
    return static_cast<BluetoothEventType>(state);
}

/*
void BluetoothDevice::update_property() {
    auto paired = g_dbus_proxy_get_cached_property(proxy, "Paired");
    this->paired = g_variant_get_boolean(paired);
    g_variant_unref(paired);

    auto connected = g_dbus_proxy_get_cached_property(proxy, "Connected");
    this->connected = g_variant_get_boolean(connected);
    g_variant_unref(connected);
}
*/

bool BluetoothDevice::Connect() {
    auto value = g_dbus_proxy_call_sync(proxy, "Connect", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, nullptr);
    if (!value) {
        log("%s", "call Connect failed");
        return false;
    }
    g_variant_unref(value);
    return true;
}

bool BluetoothDevice::Disconnect() {
    auto value = g_dbus_proxy_call_sync(proxy, "Disconnect", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, nullptr);
    if (!value) {
        log("%s", "call Disconnect failed");
        return false;
    }
    g_variant_unref(value);
    return true;
}

bool BluetoothDevice::Pair() {
    auto value = g_dbus_proxy_call_sync(proxy, "Pair", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, nullptr);
    if (!value) {
        log("%s", "call Pair failed");
        return false;
    }
    g_variant_unref(value);
    return true;
}

std::string BluetoothDevice::to_string() {
    std::ostringstream ostr;
    ostr << "Name\t\t: " << name() << "\nAddress\t\t: " << address()
         << "\nPath\t\t: " << object_path()
         << "\nPaired\t\t: " << std::boolalpha << paired()
         << "\nConnected\t: " << std::boolalpha << connected() << std::endl;
    return ostr.str();
}

BluezUtil::BluezUtil() {
    conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, nullptr, nullptr);
    if (!conn) {
        log("%s", "get system bus error");
        throw std::runtime_error("get system bus error");
    }
    object_manager = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, nullptr, BLUEZ, "/", BLUEZ_MANAGER_IFACE, nullptr, nullptr);
    if (!object_manager) {
        log("%s", "get object_manager error");
        throw std::runtime_error("get object_manager error");
    }
    adapter_proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, nullptr, BLUEZ, BLUEZ_ADAPTER_OBJ, BLUEZ_ADAPTER_IFACE, nullptr, nullptr);
    if (!adapter_proxy) {
        log("%s", "get adapter_proxy error");
        throw std::runtime_error("get adapter_proxy error");
    }
    log("%s", "add listeners to bluez");
    adapter_handle = g_dbus_connection_signal_subscribe(conn, BLUEZ, BLUEZ_PROPERTY_IFACE, "PropertiesChanged", nullptr, BLUEZ_ADAPTER_IFACE, G_DBUS_SIGNAL_FLAGS_NONE, adapter_callback, this, nullptr);
    device_handle = g_dbus_connection_signal_subscribe(conn, BLUEZ, BLUEZ_PROPERTY_IFACE, "PropertiesChanged", nullptr, BLUEZ_DEVICE_IFACE, G_DBUS_SIGNAL_FLAGS_NONE, device_callback, this, nullptr);
    iface_added_handle = g_dbus_connection_signal_subscribe(conn, BLUEZ, BLUEZ_MANAGER_IFACE, "InterfacesAdded", nullptr, nullptr, G_DBUS_SIGNAL_FLAGS_NONE, iface_added_callback, this, nullptr);
    iface_removed_handle = g_dbus_connection_signal_subscribe(conn, BLUEZ, BLUEZ_MANAGER_IFACE, "InterfacesRemoved", nullptr, nullptr, G_DBUS_SIGNAL_FLAGS_NONE, iface_removed_callback, this, nullptr);
    loop = g_main_loop_new(nullptr, false);
    std::thread t_loop([this]() {
        log("%s", "g_main_loop running");
        g_main_loop_run(loop);
        log("%s", "g_main_loop exit");
    });
    t_loop.detach();
}

BluezUtil::~BluezUtil() {
    //log("%s", "~BluezUtil");
    g_dbus_connection_signal_unsubscribe(conn, adapter_handle);
    g_dbus_connection_signal_unsubscribe(conn, device_handle);
    g_dbus_connection_signal_unsubscribe(conn, iface_added_handle);
    g_dbus_connection_signal_unsubscribe(conn, iface_removed_handle);
    g_main_loop_quit(loop);
    g_main_loop_unref(loop);
    g_object_unref(conn);
    g_object_unref(object_manager);
    g_object_unref(adapter_proxy);
}

int BluezUtil::GetAdapterState() {
    GVariant *value;
    value = g_dbus_proxy_get_cached_property(adapter_proxy, "Discovering");
    auto discovering = g_variant_get_boolean(value);
    g_variant_unref(value);
    value = g_dbus_proxy_get_cached_property(adapter_proxy, "Powered");
    auto powered = g_variant_get_boolean(value);
    g_variant_unref(value);
    auto state = discovering ? BluetoothEvent::EV_ADAPTER_DISCOVERY_ON : powered ? BluetoothEvent::EV_ADAPTER_ON : BluetoothEvent::EV_NONE;
    return static_cast<BluetoothEventType>(state);
}

int BluezUtil::GetDeviceState(const BluetoothDeviceRef &device) {
    //device->update_property();
    return device->state();
}

std::list<BluetoothDeviceRef> BluezUtil::GetDevices() {
    std::list<BluetoothDeviceRef> devices;
    GVariant *result;
    GVariantIter *iter;
    GVariantIter *v_iter;
    gchar *object_path;
    result = g_dbus_proxy_call_sync(object_manager, "GetManagedObjects", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, nullptr);
    if (!result) {
        log("%s", "call GetManagedObjects error");
        return devices;
    }
    g_variant_get(result, "(a{oa{sa{sv}}})", &iter);
    while (g_variant_iter_next(iter, "{oa{sa{sv}}}", &object_path, &v_iter)) {
        //log("path = %s", object_path);
        if (strncmp(object_path, TARGET_PATH, strlen(TARGET_PATH)) == 0) {
            gchar *key;
            GVariantIter *vv_iter;
            while (g_variant_iter_next(v_iter, "{sa{sv}}", &key, &vv_iter)) {
                //log("-- key = %s", key);
                auto ready = strcmp(key, TARGET_DEVICE) == 0;
                if (ready) {
                    gchar *attr;
                    GVariant *value;
                    auto device = new BluetoothDevice(conn, object_path);
                    while (g_variant_iter_next(vv_iter, "{sv}", &attr, &value)) {
                        //log("---- attr = %s, type = %s", attr, g_variant_get_type_string(value));
                        if (strcmp(attr, "Name") == 0) {
                            g_variant_get(value, "s", &device->name_);
                        } else if (strcmp(attr, "Address") == 0) {
                            g_variant_get(value, "s", &device->address_);
                        }
                        /*
                        else if (strcmp(attr, "Paired") == 0) {
                            device->paired = g_variant_get_boolean(value);
                        } else if (strcmp(attr, "Connected") == 0) {
                            device->connected = g_variant_get_boolean(value);
                        }
                        */
                        g_free(attr);
                        g_variant_unref(value);
                        if (device->name_ && device->address_) break;
                    }
                    devices.emplace_back(device);
                }
                g_variant_iter_free(vv_iter);
                g_free(key);
                if (ready) break;
            }
        }
        g_variant_iter_free(v_iter);
        g_free(object_path);
    }
    g_variant_iter_free(iter);
    g_variant_unref(result);
    return devices;
}

bool BluezUtil::StartDiscovery() {
    auto result = g_dbus_proxy_call_sync(adapter_proxy, "StartDiscovery", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, nullptr);
    if (!result) {
        log("%s", "call StartDiscovery failed");
        return false;
    }
    g_variant_unref(result);
    return true;
}

bool BluezUtil::StopDiscovery() {
    auto result = g_dbus_proxy_call_sync(adapter_proxy, "StopDiscovery", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, nullptr);
    if (!result) {
        log("%s", "call StopDiscovery failed");
        return false;
    }
    g_variant_unref(result);
    return true;
}

void BluezUtil::RegisterListener(BluetoothEventCallback callback) {
    this->callback = callback;
}

bool BluezUtil::Connect(const char *object_path) {
    try {
        BluetoothDevice device(conn, object_path);
        return device.Connect();
    } catch (std::runtime_error e) {}
    return false;
}

bool BluezUtil::Disconnect(const char *object_path) {
    try {
        BluetoothDevice device(conn, object_path);
        return device.Disconnect();
    } catch (std::runtime_error e) {}
    return false;
}

bool BluezUtil::Pair(const char *object_path) {
    try {
        BluetoothDevice device(conn, object_path);
        return device.Pair();
    } catch (std::runtime_error e) {}
    return false;
}

/*
** Message: 15:19:04.532: adapter_callback path = /org/bluez/hci0, interface =
*org.freedesktop.DBus.Properties, signal = PropertiesChanged, (sa{sv}as)
** Message: 15:19:04.532: str = org.bluez.Adapter1
** Message: 15:19:04.532: Discovering : type(b)
** Message: 15:19:04.532: Discovering : 1
*/
void BluezUtil::adapter_callback(GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data) {
    log("adapter_callback path = %s, interface = %s, signal = %s, %s", object_path, interface_name, signal_name, g_variant_get_type_string(parameters));
    auto util = reinterpret_cast<BluezUtil *>(user_data);
    if (!util->callback) return;

    gchar *str, *key;
    GVariant *value;
    GVariantIter *iter1, *iter2;
    g_variant_get(parameters, "(sa{sv}as)", &str, &iter1, &iter2);
    log("str = %s", str);
    while (g_variant_iter_next(iter1, "{sv}", &key, &value)) {
        log("%s : type(%s)", key, g_variant_get_type_string(value));
        if (strcmp(key, "Discovering") == 0) {
            // discovery begin
            auto v = g_variant_get_boolean(value);
            log("%s : %u", key, v);
            if (v)
                util->callback(BluetoothEvent::EV_ADAPTER_DISCOVERY_ON, nullptr);
            else
                util->callback(BluetoothEvent::EV_ADAPTER_DISCOVERY_OFF, nullptr);
        } else if (strcmp(key, "Powered") == 0) {
            // on/off
            auto v = g_variant_get_boolean(value);
            log("%s : %u", key, v);
            if (v)
                util->callback(BluetoothEvent::EV_ADAPTER_ON, nullptr);
            else
                util->callback(BluetoothEvent::EV_ADAPTER_OFF, nullptr);
        } /*
         else if (strcmp(key, "DiscoverableTimeout") == 0) {
            // discovery end
            auto v = g_variant_get_uint32(value);
            log("%s : %u", key, v);
            util->callback(EV_ADAPTER_DISCOVERY_OFF, nullptr);
        }*/
        g_free(key);
        g_variant_unref(value);
    }
    g_free(str);
    g_variant_iter_free(iter1);
    g_variant_iter_free(iter2);
}
/*
** Message: 15:41:06.960: device_callback path = /org/bluez/hci0/dev_46_4F_24_13_82_61, interface = org.freedesktop.DBus.Properties, signal = PropertiesChanged, (sa{sv}as)
** Message: 15:41:06.960: - org.bluez.Device1
** Message: 15:41:06.960: -- RSSI : type(n)
** Message: 15:41:06.960: array size = 0
*/
void BluezUtil::device_callback(GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data) {
    log("device_callback path = %s, interface = %s, signal = %s, %s", object_path, interface_name, signal_name, g_variant_get_type_string(parameters));
    auto util = reinterpret_cast<BluezUtil *>(user_data);
    if (!util->callback) return;

    gchar *str, *key;
    GVariant *value;
    GVariantIter *iter1, *iter2;
    g_variant_get(parameters, "(sa{sv}as)", &str, &iter1, &iter2);
    log("- %s", str);
    while (g_variant_iter_next(iter1, "{sv}", &key, &value)) {
        log("-- %s : type(%s)", key, g_variant_get_type_string(value));
        if (strcmp(key, "Connected") == 0) {
            auto v = g_variant_get_boolean(value);
            log("-- %s : %u", key, v);
            try {
                BluetoothDevice device(util->conn, object_path);
                if (v)
                    util->callback(BluetoothEvent::EV_DEVICE_CONNECTED, &device);
                else
                    util->callback(BluetoothEvent::EV_DEVICE_DISCONNECTED, &device);
            } catch (std::runtime_error e) {}
        } else if (strcmp(key, "Paired") == 0) {
            auto v = g_variant_get_boolean(value);
            log("-- %s : %u", key, v);
            try {
                BluetoothDevice device(util->conn, object_path);
                if (v)
                    util->callback(BluetoothEvent::EV_DEVICE_PAIRED, &device);
                else
                    util->callback(BluetoothEvent::EV_DEVICE_UNPAIRED, &device);
            } catch (std::runtime_error e) {}
        }
        g_free(key);
        g_variant_unref(value);
    }
    /*
    log("array size = %zu", g_variant_iter_n_children(iter2));
    while (g_variant_iter_next(iter2, "s", &key)) {
        log("- %s", key);
        g_free(key);
    }
    */
    g_free(str);
    g_variant_iter_free(iter1);
    g_variant_iter_free(iter2);
}
/*
** Message: 15:40:28.494: iface_added_callback path = /, interface =
*org.freedesktop.DBus.ObjectManager, signal = InterfacesAdded, (oa{sa{sv}})
** Message: 15:40:28.494: - /org/bluez/hci0/dev_04_03_D6_1B_C3_10
** Message: 15:40:28.494: -- org.freedesktop.DBus.Introspectable
** Message: 15:40:28.494: -- org.bluez.Device1
** Message: 15:40:28.494: --- Address : type(s)
** Message: 15:40:28.494: --- AddressType : type(s)
** Message: 15:40:28.494: --- Name : type(s)
** Message: 15:40:28.494: --- Alias : type(s)
** Message: 15:40:28.494: --- Class : type(u)
** Message: 15:40:28.494: --- Icon : type(s)
** Message: 15:40:28.494: --- Paired : type(b)
** Message: 15:40:28.494: --- Trusted : type(b)
** Message: 15:40:28.494: --- Blocked : type(b)
** Message: 15:40:28.494: --- LegacyPairing : type(b)
** Message: 15:40:28.494: --- RSSI : type(n)
** Message: 15:40:28.494: --- Connected : type(b)
** Message: 15:40:28.494: --- UUIDs : type(as)
** Message: 15:40:28.494: --- Adapter : type(o)
** Message: 15:40:28.494: --- ServicesResolved : type(b)
** Message: 15:40:28.494: -- org.freedesktop.DBus.Properties
*/
void BluezUtil::iface_added_callback(GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data) {
    log("iface_added_callback path = %s, interface = %s, signal = %s, %s", object_path, interface_name, signal_name, g_variant_get_type_string(parameters));
    auto util = reinterpret_cast<BluezUtil *>(user_data);
    if (!util->callback) return;

    gchar *path;         //, *key, *attr;
    GVariantIter *iter1; //, *iter2;
    //GVariant *value;
    g_variant_get(parameters, "(oa{sa{sv}})", &path, &iter1);
    log("- %s", path);
    try {
        BluetoothDevice device(util->conn, path);
        util->callback(BluetoothEvent::EV_DEVICE_FOUND, &device);
    } catch (std::runtime_error e) {}
    // log("array size = %zu", g_variant_iter_n_children(iter));
    /*
    while (g_variant_iter_next(iter1, "{sa{sv}}", &key, &iter2)) {
        log("-- %s", key);
        while (g_variant_iter_next(iter2, "{sv}", &attr, &value)) {
            log("--- %s : type(%s)", attr, g_variant_get_type_string(value));
            g_free(attr);
            g_variant_unref(value);
        }
        g_free(key);
        g_variant_iter_free(iter2);
    }
    */
    g_free(path);
    g_variant_iter_free(iter1);
}
/*
** Message: 15:40:28.412: iface_removed_callback path = /, interface =
*org.freedesktop.DBus.ObjectManager, signal = InterfacesRemoved, (oas)
** Message: 15:40:28.412: - /org/bluez/hci0/dev_04_03_D6_1B_C3_10
** Message: 15:40:28.412: array size = 3
** Message: 15:40:28.412: - org.freedesktop.DBus.Properties
** Message: 15:40:28.412: - org.freedesktop.DBus.Introspectable
** Message: 15:40:28.412: - org.bluez.Device1
*/
void BluezUtil::iface_removed_callback(GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data) {
    log("iface_removed_callback path = %s, interface = %s, signal = %s, %s", object_path, interface_name, signal_name, g_variant_get_type_string(parameters));
    auto util = reinterpret_cast<BluezUtil *>(user_data);
    if (!util->callback) return;

    gchar *path; //, *attr;
    GVariantIter *iter;
    g_variant_get(parameters, "(oas)", &path, &iter);
    log("- %s", path);
    try {
        BluetoothDevice device(util->conn, path);
        util->callback(BluetoothEvent::EV_DEVICE_REMOVE, &device);
    } catch (std::runtime_error e) {}
    // no need to parse detail
    /*
    log("array size = %zu", g_variant_iter_n_children(iter));
    while (g_variant_iter_next(iter, "s", &attr)) {
        log("- %s", attr);
        g_free(attr);
    }
    */
    g_free(path);
    g_variant_iter_free(iter);
}
