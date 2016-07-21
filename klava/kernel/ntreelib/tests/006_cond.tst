# test: merging of multiple ANY/STAR nodes
#

create "test.td"
autocheck on

clear
printf test0.tmp norm

# 001 ------------------
# merging of multiple ANY conditions

add 1 "'abcde' ?? 'tail_1'"
add 2 "'abcde' ?? [5] 'tail_2'"
add 3 "'abcde' ?? [10] 'tail_3'"
add 4 "'abcde' ?? [3] 'tail_4'"
compare 006_cond_001.txt norm

add 5 "'abcdefghijklmn'"
add 6 "'abcde' ?? [5] 'new_tail_5'"
compare 006_cond_002.txt norm

del "'abcde' ?? [5] 'tail_2'"
del "'abcde' ?? [5] 'new_tail_5'"
compare 006_cond_003.txt norm

del "'abcde' ?? [3] 'tail_4'"
del "'abcde' ?? [10] 'tail_3'"
del "'abcde' ?? 'tail_1'"
del "'abcdefghijklmn'"
compare test0.tmp norm

