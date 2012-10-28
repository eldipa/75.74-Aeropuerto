if test $# -ne 2
then
   echo "Usage: load.sh table_name file"
   exit 1
else
   if test \! -e "aero"
   then
      echo "The database doesn't exists."
      exit 1
   fi

   if test \! -e "$2"
   then
      echo "The file $2 doesn't exists."
      exit 1
   fi

   if echo ".tables" | sqlite3 aero | grep -q ".*[ \t]$1[ \t].*" 
   then
      echo "Loading..."
      echo -e ".separator \"|\"\n.import $2 $1" | sqlite3 aero
      exit 0
   else
      echo "The table $1 is not in the database"
      exit 1
   fi
fi
