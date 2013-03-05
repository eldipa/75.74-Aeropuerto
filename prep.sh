#!/bin/bash

PACK=entregable.zip

make clean
mkdir _entregable

git archive --format zip -v -o _entregable/$PACK HEAD
cd _entregable
unzip $PACK
rm $PACK

mv db/database.cpp db/file.cpp
mv db/database.h db/file.h
mv db/dberror.cpp db/fileerror.cpp
mv db/dberror.h db/fileerror.h

mv db fileutil

sed -i s/database/file/ `grep -Rl --exclude-dir="*git*" --exclude="*prep.py*" "database" .`
sed -i s/dberror/fileerror/ `grep -Rl --exclude-dir="*git*" --exclude="*prep.py*" "dberror" .`
sed -i s/db/fileutil/g Makefile fileutil/Makeconfig processes/Makeconfig api/Makeconfig
sed -i s/Database/File/g `grep -Rl --exclude-dir="*git*" --exclude="*prep.py*" "Database" .`
sed -i s/DBError/FileError/g `grep -Rl --exclude-dir="*git*" --exclude="*prep.py*" "DBError" .`

make
make clean

cd ..
zip -r $PACK _entregable/*
rm -Rf _entregable
