#g++ main.cpp -o main -O2 -std=gnu++17 -D_RUTHEN
g++ main.cpp -o main -O2 -std=gnu++17
cd tools
for input in $(ls -1 ./in/*.txt); do
    #echo ${input}
    output=`echo ${input} | cut -c 6-`
    echo ${output}
    cargo run --release --bin tester ../main < $input > ./out/$output
    cargo run --release --bin vis $input ./out/$output > hoge
    if [ "$output" = "0499.txt" ]; then
        echo "finish"
        break
    fi
done
