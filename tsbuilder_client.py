import os
import struct
import subprocess
import zmq

#---- wrapper for TimesliceBuilder ZMQ interface ---------------------

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

#---- main -----------------------------------------------------------

if __name__=='__main__':
    p = subprocess.Popen(['./tsbuilder', '25', '0', 'spadic.tsa'])
    c = zmq.Context()
    s = c.socket(zmq.PUSH)
    s.connect('ipc:///tmp/tsbuilder.ipc')

    t = TimesliceBuilder(s)
    t.add_src(1, 0x40, 1, 0)
    with open(os.path.expanduser('~/projekte/spadic10/misc/spadicdata/'
                                 'dataStreamAnalogTrigger.dat')) as f:
        dtm = []
        dtms_left = 0x100
        for line in f:
            if not dtms_left:
                t.next_mc(0)
                dtms_left = 0x100
            if len(dtm) == 32:
                t.add_dtm(0, 0, dtm)
                dtm = []
                dtms_left -= 1
            dtm.append(int(line, 16))

    t.quit()
    p.communicate()
