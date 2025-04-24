for file in ./lib/*.cxx; do
    g++ -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wconversion -Wsign-conversion -Wnull-dereference \
    --std=c++20 -g3 -O0 \
    -I./lib \
    -c "$file" \
    -o ./build/Debug/$(basename "$file" .cxx).o;
done
