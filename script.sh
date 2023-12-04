if [ "$#" -ne 1 ]
then
   echo "eroare"
fi

   count=0
char="$1"
   while IFS= read -r line
   do
	 if [[ "$line" =~ ^[A-Z].*[A-Za-z0-9\ ,.!?]$ && ! "$line" =~ ,.*[Ss][Ii].* && "$line" =~ [?.!]$ ]]
	 then
	     if [[ "$line" =~ $char ]]
	     then
	     ((count++))
	     fi
	     fi
	    done
	     
	     echo "$count"
