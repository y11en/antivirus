# vio_write.tst
#
# virtual I/O read-write test
# (basic write functionality)

read 0 5716 1.tmp
expect 0 5716 0

compare_file 1.tmp data1.dat

# write buffer in-between (0x200 - 0x2D0)

write 0x200 result3.dat
expect 0 0xD0

read 0 0x3FF 2.tmp
expect 0 0x3FF 0
compare_file 2.tmp result7.dat

read 0 0x3FF 2p.tmp portion
expect 0 0x3FF 0
compare_file 2p.tmp result7.dat

read 0 0x3FF 2b.tmp iobuf
expect 0 0x3FF 0x0
compare_file 2b.tmp result7.dat

read 0 0x3FF 2bp.tmp iobuf portion
expect 0 0x3FF 0x0
compare_file 2bp.tmp result7.dat

# read FROM INSIDE the written buffer

read 0x200 0xD0 2a.tmp
expect 0 0xD0 0
compare_file 2a.tmp result3.dat

read 0x200 0xD0 2ap.tmp portion
expect 0 0xD0 0
compare_file 2ap.tmp result3.dat

read 0x200 0xD0 2ab.tmp iobuf
expect 0 0xD0 0x0
compare_file 2ab.tmp result3.dat

read 0x200 0xD0 2abp.tmp portion iobuf
expect 0 0xD0 0x0
compare_file 2abp.tmp result3.dat

# write second buffer (the same)

write 0x200 result3.dat
expect 0 0xD0

read 0 0x3FF 3.tmp
expect 0 0x3FF 0
compare_file 3.tmp result7.dat

read 0 0x3FF 3p.tmp portion
expect 0 0x3FF 0
compare_file 3p.tmp result7.dat

read 0 0x3FF 3b.tmp iobuf
expect 0 0x3FF 0x0
compare_file 3b.tmp result7.dat

read 0 0x3FF 3bp.tmp iobuf portion
expect 0 0x3FF 0x0
compare_file 3bp.tmp result7.dat

# write parts of buffer (left, center, right)

write 0x200 data2.dat
expect 0 0x10

write 0x2C0 data2.dat
expect 0 0x10

write 0x240 data2.dat
expect 0 0x10

read 0 0x3FF 4.tmp
expect 0 0x3FF 0
compare_file 4.tmp result8.dat

read 0 0x3FF 4p.tmp portion
expect 0 0x3FF 0
compare_file 4p.tmp result8.dat

read 0 0x3FF 4b.tmp iobuf
expect 0 0x3FF 0x0
compare_file 4b.tmp result8.dat

read 0 0x3FF 4bp.tmp iobuf portion
expect 0 0x3FF 0x0
compare_file 4bp.tmp result8.dat

# read data from the untouched space (near previously written block)

read 0x2D0 0x100 5.tmp
expect 0 0x100 0
compare_file 5.tmp result9.dat

read 0x2D0 0x100 5p.tmp portion
expect 0 0x100 0
compare_file 5p.tmp result9.dat

read 0x2D0 0x100 5b.tmp iobuf
expect 0 0x100 0
compare_file 5b.tmp result9.dat

read 0x2D0 0x100 5bp.tmp iobuf portion
expect 0 0x100 0
compare_file 5bp.tmp result9.dat


# read data from the untouched space (middle of the file)

read 0x1000 0x100 6.tmp
expect 0 0x100 0
compare_file 6.tmp result10.dat

read 0x1000 0x100 6p.tmp portion
expect 0 0x100 0
compare_file 6p.tmp result10.dat

read 0x1000 0x100 6b.tmp iobuf
expect 0 0x100 0
compare_file 6b.tmp result10.dat

read 0x1000 0x100 6bp.tmp iobuf portion
expect 0 0x100 0
compare_file 6bp.tmp result10.dat

# write block at the end of the file

write 0x1644 data2.dat
expect 0 0x10

read 0x1600 0x54 7.tmp
expect 0 0x54 0
compare_file 7.tmp result11.dat

read 0x1600 0x54 7p.tmp portion
expect 0 0x54 0
compare_file 7p.tmp result11.dat

read 0x1600 0x54 7b.tmp iobuf
expect 0 0x54 0x0
compare_file 7b.tmp result11.dat

read 0x1600 0x54 7bp.tmp iobuf portion
expect 0 0x54 0x0
compare_file 7bp.tmp result11.dat

read 0x1600 0x100 8.tmp
expect 0x800000CA 0 0

read 0x1600 0x100 8p.tmp portion
expect 0 0x54 0
compare_file 8p.tmp result11.dat

read 0x1600 0x100 8b.tmp iobuf
expect 0x800000CA 0 0

read 0x1600 0x100 8bp.tmp iobuf portion
expect 0 0x54 0x0
compare_file 8bp.tmp result11.dat

# write overlapping blocks

write 0x1F8 data2.dat
expect 0 0x10

write 0x2C8 data2.dat
expect 0 0x10

read 0 0x3FF 9.tmp
expect 0 0x3FF 0
compare_file 9.tmp result12.dat

