#!/usr/bin/python3

import dbus


class Device:
    def __init__(self, path, **kw):
        super().__init__()
        self.path = str(path)
        self.name = str(kw['Name'])
        self.address = str(kw['Address'])
        self.paired = bool(kw['Paired'])
        self.connected = bool(kw['Connected'])

    def to_dict(self):
        return {'path': self.path, 'name': self.name, 'address': self.address, 'paired': self.paired, 'conntected': self.connected}

    def to_tuple(self):
        return (self.path, self.name, self.address, 2 if self.connected else 1 if self.paired else 0)

    def __str__(self):
        return '{} - {}\n{}, {}\n{}'.format(self.name, self.address, self.paired, self.connected, self.path)


bus = dbus.SystemBus()
manager = dbus.Interface(bus.get_object("org.bluez", "/"),
                         "org.freedesktop.DBus.ObjectManager")
_filter = ('Joy-Con (L)', 'Joy-Con (R)', 'Pro Controller')
objects = manager.GetManagedObjects()
devices = (Device(path=path, **iface['org.bluez.Device1']) for path, iface in objects.items()
           if "org.bluez.Device1" in iface.keys())


def hello():
    print('hello')


def get_paired_devices():
    return [d.to_tuple() for d in devices if d.paired and d.name in _filter]


def get_connected_devices():
    return [d.to_dict() for d in devices if d.connected and d.name in _filter]


if __name__ == '__main__':
    for d in get_paired_devices():
        print(d, '\n')
    for d in get_connected_devices():
        print(d, '\n')
