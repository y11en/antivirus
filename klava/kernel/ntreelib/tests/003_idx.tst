# test: basic index nodes creation
#

create "test.td"
autocheck on

clear
printf test0.tmp norm

# 001 ------------------
# level 0 index creation

add 1 "'first signature'"
add 2 "'second signature'"
add 3 "'third signature'"
compare 003_idx_001.txt norm

del   "'first signature'"
del   "'second signature'"
del   "'third signature'"
compare test0.tmp norm

# 002 ------------------
# level N index creation

add 1 "'the first signature'"
add 2 "'the second signature'"
add 3 "'the third signature'"
compare 003_idx_002.txt norm

del   "'the first signature'"
del   "'the second signature'"
del   "'the third signature'"
compare test0.tmp norm

# 003 ------------------
# level N index creation (in hashed zone)

add 1 "'entering the hashed zone: the first signature'"
add 2 "'entering the hashed zone: the second signature'"
add 3 "'entering the hashed zone: the third signature'"
compare 003_idx_003.txt norm

del   "'entering the hashed zone: the first signature'"
del   "'entering the hashed zone: the second signature'"
del   "'entering the hashed zone: the third signature'"
compare test0.tmp norm

# 003 ------------------
# level double index creation (level 0)

add 1 "'a first signature'"
add 2 "'another signature'"
add 3 "'the third signature'"
add 4 "'the fourth signature'"
compare 003_idx_004.txt norm

del   "'a first signature'"
del   "'another signature'"
del   "'the third signature'"
del   "'the fourth signature'"
compare test0.tmp norm

