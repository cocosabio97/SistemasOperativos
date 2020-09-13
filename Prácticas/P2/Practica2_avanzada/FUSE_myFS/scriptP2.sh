#! bin/bash

echo -e "--------------\nCopying files fuseLib.c, myFS.h to mount-point/ and temp/\n--------------"
read -p "Press enter..."
rm -r temp
mkdir temp
cp src/fuseLib.c mount-point
cp src/myFS.h mount-point
cp src/fuseLib.c temp
cp src/myFS.h temp
echo -e "-------------\nAuditing disk\n-------------"
read -p "Press enter..."
chmod 0755 ./my-fsck 
./my-fsck virtual-disk 	
echo -e "---------\nCalculating differences between new files and original files\n-------------"
read -p "Press enter..."
diff -q -s src/fuseLib.c mount-point/fuseLib.c
diff -q -s src/myFS.h mount-point/myFS.h
diff -q -s src/fuseLib.c temp/fuseLib.c
diff -q -s src/myFS.h temp/myFS.h
echo -e "-------------\nTruncating ./mount-point/fuseLib.c and ./temp/fuseLib.c 1 block less(4K)\n-------------"
read -p "Press enter..."
truncate -s -4K mount-point/fuseLib.c
truncate -s -4K temp/fuseLib.c
echo -e "-------------\nAuditing disk\n-------------"
read -p "Press enter..."
./my-fsck virtual-disk 	
echo -e "-------------\nCalculating differences between truncated files and originals\n-------------"
read -p "Press enter..."
diff -q -s src/fuseLib.c mount-point/fuseLib.c
diff -q -s src/fuseLib.c temp/fuseLib.c
echo -e "-------------\nCopying src/myFileSystem.c to mount-point/\n-------------"
read -p "Press enter..."
cp src/MyFileSystem.c mount-point/
echo -e "-------------------\nAuditing disk\n-------------------"
read -p "Press enter..."
./my-fsck virtual-disk 	
echo -e "-------------\nCalculating differences between new files and original files\n-------------"
read -p "Press enter..."
diff -q -s src/MyFileSystem.c mount-point/MyFileSystem.c
echo -e "----------\nTruncating ./mount-point/myFS.h and ./temp/myFS.h 1 block more(4K)\n----------"
read -p "Press enter..."
truncate -s +4K mount-point/myFS.h
truncate -s +4K temp/myFS.h
echo -e "-------------------\nAuditing disk\n-------------------"
read -p "Press enter..."
./my-fsck virtual-disk 	
echo -e "-------------\nCalculating differences between truncated files and originals\n-------------"
read -p "Press enter..."
diff -q -s src/myFS.h mount-point/myFS.h
diff -q -s src/myFS.h temp/myFS.h
echo -e "-------------\nUnmounting FS\n-------------"
fusermount -u mount-point
echo -e "Done."





