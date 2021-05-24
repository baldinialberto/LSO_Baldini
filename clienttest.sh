for i in {1..600}
do
    ./client.exe -f 'MyServerSocket.socket' -w '.',10 -W 'pippo.txt' -D 'evicted' -r 'pluto.txt' -R 10 -d 'readen' -t 10 -l 'pippo.txt','pluto.txt' -u 'pippo.txt','pluto.txt' -c 'pippo.txt' -p &
done
