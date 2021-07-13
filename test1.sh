SERVER_SOCKET=MyServerSocket.socket
EVICT_FOLDER=${PWD}/evicted
DEST_FOLDER=${PWD}/readen

cat test1_server_settings.txt > config.txt
make all
valgrind --leak-check=full --show-leak-kinds=all -s ./server.exe &> server_output.txt &
server_pid=$!
valgrind --leak-check=full --show-leak-kinds=all -s ./client.exe -f ${SERVER_SOCKET} -D ${EVICT_FOLDER} -d ${DEST_FOLDER} -w ${PWD}/to_write,10 -R 10 &> client_output.txt &
client_pid=$!
sleep 20
kill -SIGKILL ${server_pid}
kill -SIGKILL ${client_pid}