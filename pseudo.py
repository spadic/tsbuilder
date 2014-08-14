class Timeslice:
    def __init__(self, ts_len):
        self._len = ts_len
        self._components = []

    def append_component(self, ts_len, ts_index):
        assert (ts_len == self._len)
        self._idx = ts_index
        self._components.append({})
        return len(self._components) - 1

    def append_microslice(self, comp_index, mc_index, content):
        comp = self._components[comp_index]
        comp[mc_index] = content

    def __str__(self):
        return '\n'.join('(%d) %s' % (i, ', '.join('%d: %s' % (j, content)
            for (j, content) in sorted(comp.iteritems())))
            for (i, comp) in enumerate(self._components))

MC_MIN = 3
MC_MAX = 10
TS_LEN = 4

microslice_sources = {
    21: {4: 'hans', 5: 'dieter', 8: 'steffen'},
    33: {5: 'wurst', 6: 'brot'}
}

mc_start = MC_MIN
while mc_start < MC_MAX:
    ts_index = mc_start/TS_LEN
    ts = Timeslice(TS_LEN)
    for eq_id, mc_src in microslice_sources.iteritems():
        comp_index = ts.append_component(TS_LEN, ts_index)
        for mc_index in range(mc_start, mc_start + TS_LEN):
            content = mc_src.get(mc_index, '---')
            ts.append_microslice(comp_index, mc_index, content)
    mc_start += TS_LEN
    print '---- timeslice %d ----' % ts_index
    print ts
