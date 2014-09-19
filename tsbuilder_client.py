import struct
import zmq

class TimesliceBuilder:
    def __init__(self, socket):
        self.socket = socket

    def send(self, data):
        fmt = '@%dH' % len(data)
        self.socket.send(struct.pack(fmt, *data))

    def quit(self):
        self.send([0xFFFF])

    def next_mc(self, index):
        self.send([0xFFFE, index])

    def add_src(self, eq_id, sys_id, sys_ver, mc_start_idx):
        self.send([0xFFFD, eq_id, sys_id, sys_ver, mc_start_idx])

    def add_dtm(self, index, cbm_addr, data):
        self.send([0, index, cbm_addr] + data)


if __name__=='__main__':
    c = zmq.Context()
    s = c.socket(zmq.PUSH)
    s.connect('ipc:///tmp/tsbuilder.ipc')

    t = TimesliceBuilder(s)
