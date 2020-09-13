#! bin/bash

fusermount -u mount-point
echo -e "-------------\nRebuiliding project\n-------------"
make clean
make
rm -r mount-point 
mkdir mount-point
read -p -"Press enter..."
./fs-fuse -t 2097152 -a virtual-disk -f '-d -s mount-point'
