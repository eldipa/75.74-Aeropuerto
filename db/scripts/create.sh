. const.sh
if test -e "$DATABASE_NAME" 
then
   echo "The database already exists. (Remove it manually)."
else
   cat $SCHEMA | sqlite3 $DATABASE_NAME 
fi
