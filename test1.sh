cat test1_server_settings.txt > config.txt
make all
./client.sh 50 &> client_output.txt
valgrind --leak-check=full --show-leak-kinds=all ./server.exe &> server_output.txt &
server_pid=$!
sleep 2
kill -SIGHUP ${server_pid}