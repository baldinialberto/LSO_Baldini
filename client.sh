#for ((i=0; i<100; i++)) do
#  ./valgrind.sh ./client.exe -f MyServerSocket.socket -W ./to_write/file_${i}.txt
#done

#./valgrind.sh ./client.exe -f MyServerSocket.socket -W ./to_write/file_0.txt

./client.exe -f MyServerSocket.socket -W /home/alberto/LSO_Baldini/to_write/file_0.txt,/home/alberto/LSO_Baldini/to_write/file_2.txt,/home/alberto/LSO_Baldini/to_write/file_4.txt -r /home/alberto/LSO_Baldini/to_write/file_0.txt,/home/alberto/LSO_Baldini/to_write/file_2.txt,/home/alberto/LSO_Baldini/to_write/file_4.txt -d /home/alberto/LSO_Baldini/readen &
./client.exe -f MyServerSocket.socket -W /home/alberto/LSO_Baldini/to_write/file_1.txt,/home/alberto/LSO_Baldini/to_write/file_3.txt,/home/alberto/LSO_Baldini/to_write/file_5.txt -r /home/alberto/LSO_Baldini/to_write/file_1.txt,/home/alberto/LSO_Baldini/to_write/file_3.txt,/home/alberto/LSO_Baldini/to_write/file_5.txt -d /home/alberto/LSO_Baldini/readen &
./client.exe -f MyServerSocket.socket -W /home/alberto/LSO_Baldini/to_write/file_6.txt,/home/alberto/LSO_Baldini/to_write/file_8.txt,/home/alberto/LSO_Baldini/to_write/file_10.txt -r /home/alberto/LSO_Baldini/to_write/file_6.txt,/home/alberto/LSO_Baldini/to_write/file_8.txt,/home/alberto/LSO_Baldini/to_write/file_10.txt -d /home/alberto/LSO_Baldini/readen &
./client.exe -f MyServerSocket.socket -W /home/alberto/LSO_Baldini/to_write/file_7.txt,/home/alberto/LSO_Baldini/to_write/file_9.txt,/home/alberto/LSO_Baldini/to_write/file_11.txt -r /home/alberto/LSO_Baldini/to_write/file_7.txt,/home/alberto/LSO_Baldini/to_write/file_9.txt,/home/alberto/LSO_Baldini/to_write/file_11.txt -d /home/alberto/LSO_Baldini/readen &

