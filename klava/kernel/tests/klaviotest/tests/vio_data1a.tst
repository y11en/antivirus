# vio_data1a.tst
#
# (open data1a.dat in VIO mode with offset 0x100)
#

open "data1a.dat"
expect 0

vio 0x100 on
expect 0

get_size
expect 0 5716

