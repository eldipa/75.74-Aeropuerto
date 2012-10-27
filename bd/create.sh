if test -e "aero" 
then
   echo "The database already exists. (Remove it manually)."
else
   cat "schema" | sqlite aero 
fi
