if test $# -ne 2
then
   echo "Usage: dump.sh table_name file"
   exit 1
else
   if test \! -e "aero"
   then
      echo "The database doesn't exists."
      exit 1
   fi

   if test -e "$2"
   then
      echo "The file $2 exists."
      exit 1
   fi

   if echo ".tables" | sqlite3 aero | grep -q ".*[ \t]$1[ \t].*" 
   then
      echo "Saving..."
      echo -e ".mode csv\nselect * from $1;" | sqlite3 aero > "$2"
      exit 0
   else
      echo "The table $1 is not in the database"
      exit 1
   fi
fi
