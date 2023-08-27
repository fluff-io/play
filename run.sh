gcc server.c -o server -lpthread
python3 -m http.server &
./server
