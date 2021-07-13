for ((i=1; i<200; i++)) do
  yes FOR_TESTING_PORPOUSE | head -c ${i}KB > testFile_${i}.txt
done
