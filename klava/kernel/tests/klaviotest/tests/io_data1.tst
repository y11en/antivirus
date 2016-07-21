# io_data1.tst
#
# (open data1.dat in 'normal' IO mode)
#

open "data1.dat"
expect 0

get_size
expect 0 5716

