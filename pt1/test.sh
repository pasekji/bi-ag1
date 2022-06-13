i=0
for k in {2..227};
do  
  tmp=`./progtest1<"cesta$k.in" | cut -c1-2`
  ref=`cut -c1-2 "cesta$k.out"`
  if [ "$tmp" == "-1" ] || [ "$ref" == "-1" ]; then 
    if [ "$tmp" != "$ref" ]; then
        echo "FUCK $k"; 
        i=$((i+1))
    fi;
  fi;
done
echo "Celkem spatnych vystupu: $i"