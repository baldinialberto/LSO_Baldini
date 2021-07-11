for ((i=0; i<100; i++)) do
  ./client.exe -f MyServerSocket.socket -W /home/alberto/LSO_Baldini/to_write/file_${i}.txt -r /home/alberto/LSO_Baldini/to_write/file_${i}.txt -d /home/alberto/LSO_Baldini/readen &
done
