
DIRS="cert presence account profile addrbook seq msgbox logic"

for d in $DIRS; do

    killall -9 "$d"-main

    cd $d

    ./"$d"_main -c "$d"_server.conf -l 6 -d

    cd ..

done

