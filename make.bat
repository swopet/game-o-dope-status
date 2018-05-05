g++ -O0 -g3 -Wall -c -fmessage-length=0 -o main.o main.cpp 
g++ -o hex_map.exe main.o -lglu32 -lopengl32 -lSFML-window -lSFML-graphics -lSFML-system