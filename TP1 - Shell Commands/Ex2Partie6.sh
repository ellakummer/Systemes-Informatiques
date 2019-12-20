#!/bin/bash

clean(){
mv "$1" `echo $1 |tr ' ' '_' | tr '"' '_' | tr "'" '_'`
}

makeCopy(){
if [ `file -ib "$1" | cut -d '/' -f1` = "image" ];then
cp -r "$1" $2/
fi
}

convertir(){
if [ ! -z $2 ]; then
#convert -resize $2 $1 $1.png
#rm $1
else
#convert $1 $1.png
#rm $1
fi
}

if [ -z $1 -o -z $2 ];then
echo "Veuillez renseigner tout les arguments"
else
if [ -d $1 ] ; then
if [ -d $2 ];then
for fichier in $1/*;do
makeCopy "$fichier" $2
done
else
mkdir $2
for fichier in $1/*;do
makeCopy "$fichier" $2
convertir $3
done
fi

for fichier in $2/* ; do
clean "$fichier"
done

for fichier in $2/*; do
convertir $fichier $3
done

else
echo " '$1' is not a valid directory"
fi
fi
