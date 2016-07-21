# vio_chsize.tst
#
# (virtual IO: operations that change file size)
#

read 0 5716 1.tmp
expect 0 5716 0

compare_file 1.tmp data1.dat

# shrink file

set_size 0x1500
expect 0

read 0x1400 0x100 1.tmp
expect 0 0x100 0
compare_file 1.tmp result13.dat

read 0x1400 0x100 1p.tmp portion
expect 0 0x100 0
compare_file 1p.tmp result13.dat

read 0x1400 0x100 1b.tmp iobuf
expect 0 0x100 0
compare_file 1b.tmp result13.dat

read 0x1400 0x100 1bp.tmp iobuf portion
expect 0 0x100 0
compare_file 1bp.tmp result13.dat

read 0x1400 0x200 2.tmp
expect 0x800000CA 0 0

read 0x1400 0x200 2p.tmp portion
expect 0 0x100 0
compare_file 2p.tmp result13.dat

read 0x1400 0x200 2b.tmp iobuf
expect 0x800000CA 0 0

read 0x1400 0x200 2bp.tmp iobuf portion
expect 0 0x100 0
compare_file 2bp.tmp result13.dat

# grow file

set_size 0x2000
expect 0

get_size
expect 0 0x2000

read 0x1584 0xD0 3.tmp
expect 0 0xD0 0
compare_file 3.tmp result3.dat

read 0x1584 0xD0 3p.tmp portion
expect 0 0xD0 0
compare_file 3p.tmp result3.dat

read 0x1584 0xD0 3b.tmp iobuf
expect 0 0xD0 0
compare_file 3b.tmp result3.dat

read 0x1584 0xD0 3bp.tmp portion iobuf
expect 0 0xD0 0
compare_file 3bp.tmp result3.dat

# read block at the end of real IO

read 0x1584 0x100 4.tmp
expect 0 0x100 0
compare_file 4.tmp result14.dat

read 0x1584 0x100 4p.tmp portion
expect 0 0x100 0
compare_file 4p.tmp result14.dat

read 0x1584 0x100 4b.tmp iobuf
expect 0 0x100 0x0
compare_file 4b.tmp result14.dat

read 0x1584 0x100 4bp.tmp iobuf portion
expect 0 0x100 0x0
compare_file 4bp.tmp result14.dat

# read block at the end of virtual IO

read 0x1F00 0x100 5.tmp
expect 0 0x100 0
compare_file 5.tmp zero256.dat

read 0x1F00 0x100 5p.tmp portion
expect 0 0x100 0
compare_file 5p.tmp zero256.dat

read 0x1F00 0x100 5b.tmp iobuf
expect 0 0x100 0x0
compare_file 5b.tmp zero256.dat

read 0x1F00 0x100 5bp.tmp iobuf portion
expect 0 0x100 0x0
compare_file 5bp.tmp zero256.dat

# read block overlapping end of virtual IO

read 0x1F00 0x200 6.tmp
expect 0x800000CA 0 0

read 0x1F00 0x200 6p.tmp portion
expect 0 0x100 0
compare_file 6p.tmp zero256.dat

read 0x1F00 0x200 6b.tmp iobuf
expect 0x800000CA 0 0

read 0x1F00 0x200 6bp.tmp iobuf portion
expect 0 0x100 0x0
compare_file 6bp.tmp zero256.dat

# read block past the end of virtual IO

read 0x2000 0x100 7.tmp
expect 0x800000CA 0 0

read 0x2000 0x100 7p.tmp portion
expect 0 0 0

read 0x2000 0x100 7b.tmp iobuf
expect 0x800000CA 0 0

read 0x2000 0x100 7bp.tmp iobuf portion
expect 0 0 0x0

# auto-grow file by writing past EOF

write 0x4000 data3.dat
expect 0 0x2000

get_size
expect 0 0x6000

read 0x4000 0x2000 8.tmp
expect 0 0x2000 0
compare_file 8.tmp data3.dat

read 0x4000 0x2000 8p.tmp portion
expect 0 0x2000 0
compare_file 8p.tmp data3.dat

read 0x4000 0x2000 8b.tmp iobuf
expect 0 0x2000 0x0
compare_file 8b.tmp data3.dat

read 0x4000 0x2000 8bp.tmp iobuf portion
expect 0 0x2000 0x0
compare_file 8bp.tmp data3.dat

# read past new EOF

read 0x4000 0x3000 9.tmp
expect 0x800000CA 0 0

read 0x4000 0x3000 9p.tmp portion
expect 0 0x2000 0
compare_file 9p.tmp data3.dat

read 0x4000 0x3000 9b.tmp iobuf
expect 0x800000CA 0 0

read 0x4000 0x3000 9bp.tmp iobuf portion
expect 0 0x2000 0x0
compare_file 9bp.tmp data3.dat

# shrink file and grow it again, the data should be cleared

set_size 0x5F00
expect 0

get_size
expect 0 0x5F00

set_size 0x6000
expect 0

get_size
expect 0 0x6000

read 0x5F00 0x100 10_1.tmp
expect 0 0x100 0
compare_file 10_1.tmp zero256.dat

read 0x5F00 0x100 10_1p.tmp portion
expect 0 0x100 0
compare_file 10_1p.tmp zero256.dat

read 0x5F00 0x100 10_1b.tmp iobuf
expect 0 0x100 0x0
compare_file 10_1b.tmp zero256.dat

read 0x5F00 0x100 10_1bp.tmp iobuf portion
expect 0 0x100 0x0
compare_file 10_1bp.tmp zero256.dat

# this data should be retained

read 0x5E00 0x100 10_2.tmp
expect 0 0x100 0
compare_file 10_2.tmp data4.dat

read 0x5E00 0x100 10_2p.tmp portion
expect 0 0x100 0
compare_file 10_2p.tmp data4.dat

read 0x5E00 0x100 10_2b.tmp iobuf
expect 0 0x100 0x0
compare_file 10_2b.tmp data4.dat

read 0x5E00 0x100 10_2bp.tmp iobuf portion
expect 0 0x100 0x0
compare_file 10_2bp.tmp data4.dat

set_size 0x2000
expect 0

get_size
expect 0 0x2000

read 0x4000 0x2000 10.tmp
expect 0x800000CA 0 0

read 0x4000 0x2000 10p.tmp portion
expect 0 0 0

read 0x4000 0x2000 10b.tmp iobuf
expect 0x800000CA 0 0

read 0x4000 0x2000 10bp.tmp iobuf portion
expect 0 0 0x0

set_size 0x6000
expect 0

get_size
expect 0 0x6000

read 0x4000 0x100 11.tmp
expect 0 0x100 0
compare_file 11.tmp zero256.dat

read 0x4000 0x100 11p.tmp portion
expect 0 0x100 0
compare_file 11p.tmp zero256.dat

read 0x4000 0x100 11b.tmp iobuf
expect 0 0x100 0x0
compare_file 11b.tmp zero256.dat

read 0x4000 0x100 11bp.tmp iobuf portion
expect 0 0x100 0x0
compare_file 11bp.tmp zero256.dat

