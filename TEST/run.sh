echo "///// START BUILD \\\\\\"
make debug
echo "/////  END BUILD  \\\\\\"
echo "///// START EXEC \\\\\\"
./test.exe
./server.exe
echo "/////  END EXEC  \\\\\\"
make clean