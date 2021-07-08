#for ((i=0; i<100; i++)) do
#  ./valgrind.sh ./client.exe -f MyServerSocket.socket -W ./to_write/file_${i}.txt
#done

./valgrind.sh ./client.exe -f MyServerSocket.socket -W ./to_write/file_0.txt
