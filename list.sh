ls -l processes/ | grep "^-rwx" | sed -n "s/^\(.*\) \([a-Z_]\+\)$/\2/p"
