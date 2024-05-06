all: matmul

matmul:
	g++ -shared -fPIC myMatMul.cpp -o libMatMul.so

clean:
	rm -rf libMatMul.so