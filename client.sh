declare -i l=$1
declare -i x=100
l=$(( l + x ))
for ((i=x; i<${l}; i++)) do
  valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./client.exe -f MyServerSocket.socket -W /home/alberto/LSO_Baldini/to_write/file_${i}.txt -r /home/alberto/LSO_Baldini/to_write/file_${i}.txt -d /home/alberto/LSO_Baldini/readen -D /home/alberto/LSO_Baldini/evicted &
done
