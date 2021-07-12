for ((i=0; i<$1; i++)) do
  ./client.exe -f MyServerSocket.socket -W /home/alberto/LSO_Baldini/to_write/file_${i}.txt -r /home/alberto/LSO_Baldini/to_write/file_${i}.txt -d /home/alberto/LSO_Baldini/readen -D /home/alberto/LSO_Baldini/evicted &
done
