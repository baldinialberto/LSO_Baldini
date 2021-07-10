#for ((i=0; i<100; i++)) do
#  ./valgrind.sh ./client.exe -f MyServerSocket.socket -W ./to_write/file_${i}.txt
#done

#./valgrind.sh ./client.exe -f MyServerSocket.socket -W ./to_write/file_0.txt

./client.exe -f MyServerSocket.socket -W ./to_write/file_0.txt,./to_write/file_2.txt,./to_write/file_4.txt &
./client.exe -f MyServerSocket.socket -W ./to_write/file_1.txt,./to_write/file_3.txt,./to_write/file_5.txt &
./client.exe -f MyServerSocket.socket -W ./to_write/file_6.txt,./to_write/file_8.txt,./to_write/file_10.txt &
./client.exe -f MyServerSocket.socket -W ./to_write/file_7.txt,./to_write/file_9.txt,./to_write/file_11.txt &