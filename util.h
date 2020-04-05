#include <Python.h>

#include <functional>
#include <list>
#include <memory>

class BluetoothDevice;

using PyObjectDeleter = std::function<void(PyObject*)>;
using PyObjectRef = std::unique_ptr<PyObject, PyObjectDeleter>;
using BluetoothDeviceRef = std::unique_ptr<BluetoothDevice>;

class PyBase {
   protected:
    explicit PyBase();
    virtual ~PyBase();
};

class BluetoothDevice {
   private:
    const char* path;

   public:
    const char* name;
    const char* address;
    int state;
    explicit BluetoothDevice(PyObject*);
    ~BluetoothDevice();
};

class BluezUtil : private PyBase {
   private:
    PyObjectRef module;
    PyObjectRef pyHello;
    PyObjectRef pyGetPairedDevices;
    PyObjectRef pyGetConnectedDevices;

   public:
    explicit BluezUtil();
    explicit BluezUtil(const char* script_path);
    ~BluezUtil();
    void Hello();
    std::list<BluetoothDeviceRef> GetPairedDevices();
    std::list<BluetoothDeviceRef> GetConnectedDevices();
};
