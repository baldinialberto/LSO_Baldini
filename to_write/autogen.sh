for ((i=100; i<200; i++)) do
  head -c ${i}KB /dev/urandom > file_${i}.txt
done
