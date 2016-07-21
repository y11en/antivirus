# test: long (hashed) nodes
#

create "test.td"
autocheck on

clear
printf test0.tmp norm

# 001 ------------------

add 1 "'long signature must be folded to hashed node'"
printf test1.tmp norm

printf test2.tmp norm
compare 002_join_001.txt norm

del "'long signature must be folded to hashed node'"
compare test0.tmp norm

# 002 ------------------

add 1 "'0123456789ABCDEF'"
printf test1.tmp norm

printf test2.tmp norm
compare 002_join_002.txt norm

del "'0123456789ABCDEF'"
compare test0.tmp norm

# 003 ------------------

add 1 "'0123456789ABCDEF0'"
printf test1.tmp norm

printf test2.tmp norm
compare 002_join_003.txt norm

del "'0123456789ABCDEF0'"
compare test0.tmp norm

# 004 ------------------
# just below hashed threshold (should remain joined)

add 1 "'0123456789ABCDEF0123456'"
printf test1.tmp norm

printf test2.tmp norm
compare 002_join_004.txt norm

del "'0123456789ABCDEF0123456'"
compare test0.tmp norm

# 005 ------------------
# just above hashed threshold (should become hashed)

add 1 "'0123456789ABCDEF01234567'"
printf test1.tmp norm

compare 002_join_005.txt norm

del "'0123456789ABCDEF01234567'"
compare test0.tmp norm

# 006 ------------------
# extended above hashed threshold

add 1 "'0123456789ABCDEF0123456'"
printf test1.tmp norm

compare 002_join_004.txt norm

add 2 "'0123456789ABCDEF01234567890123'"
compare 002_join_006.txt norm

del "'0123456789ABCDEF0123456'"
del "'0123456789ABCDEF01234567890123'"
compare test0.tmp norm

# 007 ------------------
# short hashed node - stored as joined

add 1 "'0123456789ABCDEF01234567890123'"
printf test1.tmp norm

add 2 "'0123456789ABCDEF01234567890123456789'"
compare 002_join_007.txt norm

del "'0123456789ABCDEF01234567890123456789'"
del "'0123456789ABCDEF01234567890123'"
compare test0.tmp norm

# 008 ------------------
# long hashed nodes - converting to/from joined

add 1 "'0123456789ABCDEF01234567890123'"
printf test1.tmp norm

add 2 "'0123456789ABCDEF0123456789012345678901234567890123456789'"
printf test2.tmp norm

add 3 "'0123456789ABCDEF01234567890123456789'"
printf test3.tmp norm

add 4 "'0123456789ABCDEF0123456789012345678901234567890'"
printf test4.tmp norm
compare 002_join_008a.txt norm

del   "'0123456789ABCDEF01234567890123'"
printf test5.tmp norm

del   "'0123456789ABCDEF01234567890123456789'"
printf test6.tmp norm

del   "'0123456789ABCDEF0123456789012345678901234567890'"
compare 002_join_008b.txt norm

del   "'0123456789ABCDEF0123456789012345678901234567890123456789'"
compare test0.tmp norm

# 009 ------------------
# splitting node in two short, then merge into hashed

add 1 "'0123456789ABCDEF0123456789012345678901234567890123456789'"
printf test1.tmp norm

add 2 "'0123456789ABCDEF01234567890123456789'"
printf test2.tmp norm

add 3 "'0123456789ABCDEF012345678901234567890123'"
printf test3.tmp norm

add 4 "'0123456789ABCDEF012345678901234567890123456'"
printf test4.tmp norm
compare 002_join_009a.txt norm

del   "'0123456789ABCDEF012345678901234567890123'"
printf test5.tmp norm
compare 002_join_009b.txt norm

del   "'0123456789ABCDEF012345678901234567890123456'"
printf test6.tmp norm
compare 002_join_009c.txt norm

del   "'0123456789ABCDEF01234567890123456789'"
compare test1.tmp norm

del   "'0123456789ABCDEF0123456789012345678901234567890123456789'"
compare test0.tmp norm

