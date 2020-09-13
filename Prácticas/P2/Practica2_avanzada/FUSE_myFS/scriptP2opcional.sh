#!/bin/bash

cd mount-point
echo -e "Creamos un archivo y un enlace rigido a ese archivo.\n"
echo "Hola" > prueba.txt
ln prueba.txt prueba-ln.txt
ls -l 
echo -e "Mostramos el contenido del archivo original y el enlace\n"
cat prueba.txt 
cat prueba-ln.txt
echo -e "Modificamos el contenido del enlace.\n"
echo 1234 > prueba-ln.txt
echo -e "Mostramos el contenido de nuevo.\n"
cat prueba.txt 
cat prueba-ln.txt
echo -e "Borramos el archivo orginial\n"
rm prueba.txt
sleep 1
ls -l
