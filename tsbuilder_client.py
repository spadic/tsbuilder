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

def run_single_component(ts_len, ts_start_idx, output_tsa,
                         eq_id, sys_id, sys_ver, mc_start_idx,
                         cbmnet_addr, dtms_per_mc, words_per_dtm, input_file):
    p = subprocess.Popen(['./tsbuilder',
                          str(ts_len), str(ts_start_idx), output_tsa])
    c = zmq.Context()
    s = c.socket(zmq.PUSH)
    s.connect('ipc:///tmp/tsbuilder.ipc')

    t = TimesliceBuilder(s)
    t.add_src(eq_id, sys_id, sys_ver, mc_start_idx)
    with open(input_file) as f:
        dtm = []
        dtms_left = dtms_per_mc
        for line in f:
            if not dtms_left:
                t.next_mc(0)
                dtms_left = dtms_per_mc
            if len(dtm) == words_per_dtm:
                t.add_dtm(0, cbmnet_addr, dtm)
                dtm = []
                dtms_left -= 1
            dtm.append(int(line, 16))
        t.add_dtm(0, cbmnet_addr, dtm)

    t.quit()
    p.communicate()


if __name__=='__main__':
    def get_arg(argv, option):
        return argv[argv.index(option)+1]
    from sys import argv

    run_single_component(
      ts_len = 25,
      ts_start_idx = 0,
      output_tsa = get_arg(argv, '-o'),
      eq_id = 1,
      sys_id = 0x40,
      sys_ver = 1,
      mc_start_idx = 0,
      cbmnet_addr = 0,
      dtms_per_mc = 0x100,
      words_per_dtm = 32,
      input_file = get_arg(argv, '-i')
    )
