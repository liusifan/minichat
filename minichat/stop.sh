
DIRS="cert presence account profile addrbook seq msgbox logic"

for d in $DIRS; do

    killall -9 "$d"-main

done

