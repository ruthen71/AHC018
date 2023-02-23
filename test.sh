g++ main.cpp -o main -O2 -std=gnu++17 -D_RUTHEN
cd tools
cargo run --release --bin tester ../main < ./in/0000.txt > ./out/0000.txt