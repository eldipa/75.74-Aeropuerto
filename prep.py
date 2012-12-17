import os
import sys

os.system('make clean')
os.system('mkdir _entregable')
os.system('git archive --format zip -v -o _entregable/entregable.zip HEAD')
os.system('cd _entregable')
os.system('unzip entregable.zip')

os.system('mv db/database.cpp db/file.cpp')
os.system('mv db/database.h db/file.h')
os.system('mv db/dberror.cpp db/fileerror.cpp')
os.system('mv db/dberror.h db/fileerror.h')

os.system('mv db fileutil')

os.system('sed -i s/database/file/ `grep -Rl --exclude-dir="*git*" --exclude="*prep.py*" "database" .`')
os.system('sed -i s/dberror/fileerror/ `grep -Rl --exclude-dir="*git*" --exclude="*prep.py*" "dberror" .`')
os.system('sed -i s/db/fileutil/g Makefile fileutil/Makeconfig processes/Makeconfig api/Makeconfig')
os.system('sed -i s/Database/File/g `grep -Rl --exclude-dir="*git*" --exclude="*prep.py*" "Database" .`')
os.system('sed -i s/DBError/FileError/g `grep -Rl --exclude-dir="*git*" --exclude="*prep.py*" "DBError" .`')

os.system('make')
os.system('make clean')

os.system('cd ..')
os.system('zip entregable.zip _entregable/*')
os.system('rm -Rf _entregable')
