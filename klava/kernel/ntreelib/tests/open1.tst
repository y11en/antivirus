
create bar.td
trace on
add 0x1111 "'foo bar baz qux quux'"
print
commit
dumpb

close

open bar.td
trace on
add 0x2222 "'foo bar abcd efgh'"
print
dumpb
commit
dumpb

close

open bar.td
dumpb
