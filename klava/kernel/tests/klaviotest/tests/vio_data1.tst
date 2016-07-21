# vio_data1.tst
#
# (open data1.dat in VIO mode with offset 0)
#

open "data1.dat"
expect 0

vio 0 on
expect 0

get_size
expect 0 5716

