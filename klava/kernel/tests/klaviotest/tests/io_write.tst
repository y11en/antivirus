# io_write.tst
#
# Basic IO writing functionality
#

open "data2.tmp"
expect 0x8000004C

open "data2.tmp" write
expect 0

read 0 0x100 1w.tmp portion
expect 0 0 0

write 0 result1.dat
expect 0 0x213

write 0x213 result2.dat
expect 0 0x1EC

write 0x1584 result3.dat
expect 0 0xD0

get_size
expect 0 5716

read 0 5716 2w.tmp
expect 0 5716 0

compare_file 2w.tmp result4.dat

write 0x200 result3.dat
expect 0 0xD0

read 0 5716 3w.tmp
expect 0 5716 0

compare_file 3w.tmp result5.dat

set_size 0x2000
expect 0

get_size
expect 0 0x2000

read 0 0x2000 4w.tmp
expect 0 0x2000 0
compare_file 4w.tmp result6.dat

set_size 0x3FF
expect 0

get_size
expect 0 0x3FF

read 0 0x2000 5w.tmp portion
expect 0 0x3FF 0

compare_file 5w.tmp result7.dat

write 0 result1.dat portion
expect 0 0x213

write 0x213 result2.dat portion
expect 0 0x1EC

write 0x1584 result3.dat portion
expect 0 0xD0

get_size
expect 0 5716

read 0 5716 6w.tmp portion
expect 0 5716 0

compare_file 6w.tmp result4.dat

