echo "///// START BUILD \\\\\\"
make all
echo "/////  END BUILD  \\\\\\"
echo "///// START EXEC \\\\\\"
./test.exe
echo "/////  END EXEC  \\\\\\"
make clean