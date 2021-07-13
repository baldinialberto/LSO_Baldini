cat test2_server_settings.txt > config.txt
make all
./client.sh 50 &> client_output.txt
./server.exe &> server_output.txt &
server_pid=$!
sleep 2
kill -SIGHUP ${server_pid}