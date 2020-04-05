#include "util.h"

#include <iostream>

static void log(const char* msg) {
    std::cout << "[BluezUtil] " << msg << std::endl;
}

static void UnrefPyObject(PyObject* ptr) {
    // log("UnrefPyObject");
    Py_DECREF(ptr);
}

static inline PyObjectRef MakePyObjectRef(PyObject* obj) {
    return std::unique_ptr<PyObject, PyObjectDeleter>(obj, UnrefPyObject);
}

BluetoothDevice::BluetoothDevice(PyObject* obj) {
    PyArg_Parse(obj, "(sssi)", &path, &name, &address, &state);
    std::cout << "{ " << path << ", " << name << ", " << address << ", "
              << state << " }" << std::endl;
}

BluetoothDevice::~BluetoothDevice() {}

PyBase::PyBase() {
    Py_Initialize();
    if (!Py_IsInitialized()) {
        log("python init failed");
        throw std::runtime_error("python init failed");
    }
}
PyBase::~PyBase() { Py_Finalize(); }

BluezUtil::BluezUtil(const char* script_path) {
    char path[64];
    sprintf(path, "sys.path.append('%s')", script_path);
    // log(path);
    PyRun_SimpleString("import sys");
    PyRun_SimpleString(path);
    module = MakePyObjectRef(PyImport_ImportModule("bluez"));
    if (!module) {
        log("import bluez failed");
        throw std::runtime_error("import bluez failed");
    }
    pyHello = MakePyObjectRef(PyObject_GetAttrString(module.get(), "hello"));
    if (!pyHello || !PyCallable_Check(pyHello.get())) {
        log("get function 'hello' failed");
        throw std::runtime_error("get function 'hello' failed");
    }
    pyGetPairedDevices = MakePyObjectRef(
        PyObject_GetAttrString(module.get(), "get_paired_devices"));
    if (!pyGetPairedDevices || !PyCallable_Check(pyGetPairedDevices.get())) {
        log("get function 'get_paired_devices' failed");
        throw std::runtime_error("get function 'get_paired_devices' failed");
    }
    pyGetConnectedDevices = MakePyObjectRef(
        PyObject_GetAttrString(module.get(), "get_connected_devices"));
    if (!pyGetConnectedDevices ||
        !PyCallable_Check(pyGetConnectedDevices.get())) {
        log("get function 'get_connected_devices' failed");
        throw std::runtime_error("get function 'get_connected_devices' failed");
    }
}

BluezUtil::BluezUtil() : BluezUtil{"./scripte"} {}
BluezUtil::~BluezUtil() { log("~BluezUtil"); }

void BluezUtil::Hello() { PyObject_CallObject(pyHello.get(), nullptr); }

std::list<BluetoothDeviceRef> BluezUtil::GetPairedDevices() {
    std::list<BluetoothDeviceRef> list;
    auto devices =
        MakePyObjectRef(PyObject_CallObject(pyGetPairedDevices.get(), nullptr));
    auto len = PyList_Size(devices.get());
    std::cout << "devices length = " << len << std::endl;
    for (int i = 0; i < len; i++) {
        auto dev = MakePyObjectRef(PyList_GetItem(devices.get(), i));
        list.emplace_back(
            std::make_unique<BluetoothDevice>(BluetoothDevice(dev.get())));
    }
    return list;
}

std::list<BluetoothDeviceRef> BluezUtil::GetConnectedDevices() {
    std::list<BluetoothDeviceRef> list;
    auto devices = MakePyObjectRef(
        PyObject_CallObject(pyGetConnectedDevices.get(), nullptr));
    auto len = PyList_Size(devices.get());
    std::cout << "devices length = " << len << std::endl;
    for (int i = 0; i < len; i++) {
        auto dev = MakePyObjectRef(PyList_GetItem(devices.get(), i));
        list.emplace_back(
            std::make_unique<BluetoothDevice>(BluetoothDevice(dev.get())));
    }
    return list;
}
