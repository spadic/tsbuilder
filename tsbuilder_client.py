#!/usr/bin/env python

import os
import struct
import subprocess
import zmq


#==== wrapper for TimesliceBuilder ZMQ interface =====================

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


#==== simple use case (only one component) ===========================

def add_dtms_from_file(ts_builder, filename,
                       dtms_per_mc, words_per_dtm, cbmnet_addr):
    with open(filename) as f:
        dtm = []
        dtms_left = dtms_per_mc
        for line in f:
            if not dtms_left:
                ts_builder.next_mc(0)
                dtms_left = dtms_per_mc
            if len(dtm) == words_per_dtm:
                ts_builder.add_dtm(0, cbmnet_addr, dtm)
                dtm = []
                dtms_left -= 1
            dtm.append(int(line, 16))
        ts_builder.add_dtm(0, cbmnet_addr, dtm)

def run_single_component(ts_len, ts_start_idx, output_tsa,
                         eq_id, sys_id, sys_ver, mc_start_idx,
                         cbmnet_addr, dtms_per_mc, words_per_dtm, input_file):
    p = subprocess.Popen(['./tsbuilder',
                          str(ts_len), str(ts_start_idx), output_tsa])
    c = zmq.Context()
    s = c.socket(zmq.PUSH)
    s.setsockopt(zmq.LINGER, 0) # must do this, otherwise the Python
                                # interpreter hangs after p.terminate()
    s.connect('ipc:///tmp/tsbuilder.ipc')

    t = TimesliceBuilder(s)
    t.add_src(eq_id, sys_id, sys_ver, mc_start_idx)
    try:
        add_dtms_from_file(t, input_file, dtms_per_mc, words_per_dtm, cbmnet_addr)
    except Exception as e:
        print e
        # kill ./tsbuilder
        p.terminate()
    else:
        # send quit command to ./tsbuilder and wait for it to finish
        t.quit()
        p.communicate()

if __name__=='__main__':
    from sys import argv
    def get_arg(option):
        return argv[argv.index(option)+1]

    run_single_component(
      ts_len = 25,
      ts_start_idx = 0,
      output_tsa = get_arg('-o'),
      eq_id = 1,
      sys_id = 0x40,
      sys_ver = 1,
      mc_start_idx = 0,
      cbmnet_addr = 0,
      dtms_per_mc = 0x100,
      words_per_dtm = 32,
      input_file = get_arg('-i')
    )
