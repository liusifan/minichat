
DIRS="cert presence account profile addrbook seq msgbox logic"

for d in $DIRS; do

    killall -9 "$d"_main

    cd $d

    ./"$d"_main -c "$d"_server.conf -d

    cd ..

done

