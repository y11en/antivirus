# test: basic joined nodes creation
#

create "test.td"
autocheck on

clear
printf test0.tmp norm

# 001 ------------------

add 1 "'foo bar baz'"
printf test1.tmp norm

add 2 "'foo bar baz qux'"
printf test2.tmp norm
compare 001_join_001.txt norm

del "'foo bar baz qux'"
compare test1.tmp norm

del "'foo bar baz'"
compare test0.tmp norm

# 002 ------------------

add 2 "'foo bar baz qux'"
printf test1.tmp norm

add 1 "'foo bar baz'"
printf test2.tmp norm
compare 001_join_001.txt norm

del "'foo bar baz'"
compare test1.tmp norm

del "'foo bar baz qux'"
compare test0.tmp norm

# 003 -------------------

add 1 "'foo bar baz'"
printf test1.tmp norm

add 2 "'f'"
printf test2.tmp norm
compare 001_join_003.txt norm

del "'f'"
compare test1.tmp norm

del "'foo bar baz'"
compare test0.tmp norm

# 004 -------------------

add 1 "'foo bar baz'"
printf test1.tmp norm

add 2 "'foo bar bazz'"
printf test2.tmp norm
compare 001_join_004.txt norm

del "'foo bar bazz'"
compare test1.tmp norm

del "'foo bar baz'"
compare test0.tmp norm

# 005 -------------------

add 1 "'foo bar baz'"
printf test1.tmp norm

add 2 "'f'"
printf test2.tmp norm
compare 001_join_003.txt norm

del "'foo bar baz'"
del "'f'"
compare test0.tmp norm

# 006 -------------------

add 1 "'foo bar baz'"
printf test1.tmp norm

add 2 "'foo bar bazz'"
printf test2.tmp norm

del "'foo bar baz'"
del "'foo bar bazz'"
compare test0.tmp norm

# 007 ------------------

add 1 "'foo bar baz'"
printf test1.tmp norm

add 2 "'foo bar baz qux'"
printf test2.tmp norm

del "'foo bar baz'"
del "'foo bar baz qux'"
compare test0.tmp norm

# 008 -------------------

add 2 "'f'"
printf test1.tmp norm

add 1 "'foo bar baz'"
printf test2.tmp norm
compare 001_join_003.txt norm

del "'foo bar baz'"
compare test1.tmp norm

del "'f'"
compare test0.tmp norm


# 009 ------------------

add 1 "'foo bar baz'"
printf test1.tmp norm

add 3 "'foo bar baz qux quux quuux'"
printf test3_etl.tmp norm

del "'foo bar baz qux quux quuux'"
del "'foo bar baz'"
compare test0.tmp norm

# 010 ------------------

add 1 "'foo bar baz'"
printf test1.tmp norm

add 2 "'foo bar baz qux'"
printf test2.tmp norm

add 3 "'foo bar baz qux quux quuux'"
printf test3.tmp norm
compare 001_join_010.txt norm

del "'foo bar baz qux'"
compare test3_etl.tmp norm

del "'foo bar baz qux quux quuux'"
del "'foo bar baz'"
compare test0.tmp norm


# 010 ------------------

add 1 "'foo bar'"
printf test1.tmp norm

add 4 "'foo bar baz qux quux quuux'"
printf test4.tmp norm

add 2 "'foo bar baz'"
printf test2.tmp norm

add 3 "'foo bar baz qux'"
printf test3.tmp norm
compare 001_join_011.txt norm

del "'foo bar baz'"
del "'foo bar baz qux'"
compare test4.tmp norm

add 3 "'foo bar baz qux'"
add 2 "'foo bar baz'"
compare test3.tmp norm

del "'foo bar baz'"
del "'foo bar baz qux'"
compare test4.tmp norm

del "'foo bar'"
del "'foo bar baz qux quux quuux'"
compare test0.tmp norm

