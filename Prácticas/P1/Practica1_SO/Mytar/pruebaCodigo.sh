#! /bin/bash
make clean
make
if test -e mytar && test -x mytar; then
	if test -e tmp && test -d tmp ; then
		rm -r tmp
	fi
	echo -e "Creando directorio tmp\n----------"
	mkdir tmp
	cd tmp
	echo -e "Creando archivos...\n----------"
	touch file1.txt file2.txt file3.dat
	echo "Hola Mundo!" > file1.txt
	head /etc/passwd > file2.txt
	head --bytes=1024 /dev/urandom > file3.dat
	echo -e "Creando mtar\n---------"
	../mytar -c -f filetar.mtar file1.txt file2.txt file3.dat
	mkdir out
	echo -e "Creando directorio out\n-------"
	echo -e "Copiando filetar.mtar\n-------"
	cp filetar.mtar out
	cd out
	echo -e "Extranyendo filetar.mtar\n------"
	../../mytar -x -f filetar.mtar
	echo -e "Comparando...\n----"
	if diff file2.txt ../file2.txt && diff file1.txt ../file1.txt && diff file3.dat ../file3.dat; then 
		echo "Correcto!"
	else 
		echo "ERROR: Alguno de los archivos no es igual."
	fi
	cd ../..
else 
	echo "Error: No existe el programa mytar."
fi
