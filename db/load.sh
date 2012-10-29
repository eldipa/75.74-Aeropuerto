. const.sh

#To import dates, the format must be YYYY-MM-DD and must not be any space 
#between the date and the separators:
#  .. | 2012-02-01 |    <- WRONG       ... |2012-02-01|     <- OK
if test $# -ne 2
then
   echo "Usage: load.sh table_name file"
   exit 1
else
   if test \! -e "$DATABASE_NAME"
   then
      echo "The database doesn't exists."
      exit 1
   fi

   if test \! -e "$2"
   then
      echo "The file $2 doesn't exists."
      exit 1
   fi

   if echo ".tables" | sqlite3 $DATABASE_NAME | grep -q ".*[ \t]$1[ \t].*" 
   then
      echo "Loading..."
      echo -e ".separator \"$SEP\"\n.import $2 $1" | sqlite3 $DATABASE_NAME
      exit 0
   else
      echo "The table $1 is not in the database"
      exit 1
   fi
fi
