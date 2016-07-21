# test: basic condition node creation
#

create "test.td"
autocheck on

clear
printf test0.tmp norm

# 001 ------------------
# single condition node

add 1 "[&0F=05] 'abcde'"
compare 005_cond_001.txt norm

del "[&0F=05] 'abcde'"
compare test0.tmp norm

# 002 ------------------
# multiple condition nodes at root level

add 1 "[&0F=05] 'abcde'"
add 2 "[&1F=1A] 'abcde'"
add 3 "[R10-20] 'abcde'"
compare 005_cond_002.txt norm

del "[&0F=05] 'abcde'"
del "[R10-20] 'abcde'"
del "[&1F=1A] 'abcde'"
compare test0.tmp norm

# 003 ------------------
# multiple condition nodes at non-root level

add 1 "'ABC' [&1F=1A] 'abcde'"
add 2 "'ABC' [&0F=05] 'abcde'"
add 3 "'ABC' ?? 'abcde'"
add 4 "'ABC' [R50-FF] 'abcde'"
compare 005_cond_003.txt norm

del "'ABC' [&0F=05] 'abcde'"
del "'ABC' ?? 'abcde'"
del "'ABC' [&1F=1A] 'abcde'"
del "'ABC' [R50-FF] 'abcde'"
compare test0.tmp norm

# 004 ------------------
# multiple condition nodes at TAIL node level

add 1 "'ABCDEFG'"
add 2 "'ABCDEFG' [&1F=1A] 'abcde'"
add 3 "'ABCDEFG' [&0F=05] 'abcde'"
add 4 "'ABCDEFG' ?? 'abcde'"
add 5 "'ABCDEFG' [R50-FF] 'abcde'"
compare 005_cond_004.txt norm

del "'ABCDEFG' ?? 'abcde'"
del "'ABCDEFG' [&0F=05] 'abcde'"
del "'ABCDEFG'"
del "'ABCDEFG' [&1F=1A] 'abcde'"
del "'ABCDEFG' [R50-FF] 'abcde'"
compare test0.tmp norm

# 005 ------------------
# multiple condition nodes at JOINED node level

add 1 "'ABCDEFG'"
add 2 "'ABCD' [&1F=1A] 'abcde'"
add 3 "'ABCD' [&0F=05] 'abcde'"
add 4 "'ABCD' ?? 'abcde'"
add 5 "'ABCD' [R50-FF] 'abcde'"
compare 005_cond_005.txt norm

del "'ABCD' ?? 'abcde'"
del "'ABCD' [&0F=05] 'abcde'"
del "'ABCDEFG'"
del "'ABCD' [&1F=1A] 'abcde'"
del "'ABCD' [R50-FF] 'abcde'"
compare test0.tmp norm

