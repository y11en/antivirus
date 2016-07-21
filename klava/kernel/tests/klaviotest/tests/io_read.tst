# io_read.tst
#
# Basic IO reading functionality
#

# read block from offset 0

read 0 0x213 1.tmp
expect 0 0x213 0
compare_file 1.tmp result1.dat

read 0 0x213 1p.tmp portion
expect 0 0x213 0
compare_file 1p.tmp result1.dat

read 0 0x213 1b.tmp iobuf
expect 0 0x213 0x0
compare_file 1b.tmp result1.dat

read 0 0x213 1bp.tmp iobuf portion
expect 0 0x213 0x0
compare_file 1bp.tmp result1.dat

# read block from other offset

read 0x213 0x1EC 2.tmp
expect 0 0x1EC 0
compare_file 2.tmp result2.dat

read 0x213 0x1EC 2p.tmp portion
expect 0 0x1EC 0
compare_file 2p.tmp result2.dat

read 0x213 0x1EC 2b.tmp iobuf
expect 0 0x1EC 0x0
compare_file 2b.tmp result2.dat

read 0x213 0x1EC 2bp.tmp iobuf portion
expect 0 0x1EC 0x0
compare_file 2bp.tmp result2.dat

# read block from the tail

read 0x1584 0xD0 3.tmp
expect 0 0xD0 0
compare_file 3.tmp result3.dat

read 0x1584 0xD0 3p.tmp portion
expect 0 0xD0 0
compare_file 3p.tmp result3.dat

read 0x1584 0xD0 3b.tmp iobuf
expect 0 0xD0 0x0
compare_file 3b.tmp result3.dat

read 0x1584 0xD0 3bp.tmp iobuf portion
expect 0 0xD0 0x0
compare_file 3bp.tmp result3.dat

# try to read past EOF (should succeed in portion mode)

read 0x1584 0x100 4.tmp
expect 0x800000CA 0 0

read 0x1584 0x100 4p.tmp portion
expect 0 0xD0 0
compare_file 4p.tmp result3.dat

read 0x1584 0x100 4b.tmp iobuf
expect 0x800000CA 0 0

read 0x1584 0x100 4bp.tmp iobuf portion
expect 0 0xD0 0x0
compare_file 4bp.tmp result3.dat

# try to read completely past EOF

read 0x5716 0x10 5.tmp
expect 0x800000CA 0 0

read 0x5716 0x10 5p.tmp portion
expect 0 0 0

read 0x5716 0x10 5b.tmp iobuf
expect 0x800000CA 0 0

read 0x5716 0x10 5bp.tmp iobuf portion
expect 0 0 0x0

