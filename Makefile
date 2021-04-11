#########################
# Environment variable
#########################
build=build
src=src
GPP=g++ -std=c++11 -pthread -lpthread -fopenmp -O3 -Wl,--no-as-needed -march=znver2

#########################
# Run
#########################
matmul: matmul_o $(build)/driver.o
	$(GPP) -o matmul \
		$(build)/driver.o \
		$(build)/matmul.o

matmul_o:
	$(GPP) -c $(src)/matmul.cpp -o $(build)/matmul.o

# You don't need to run this
# $(build)/driver.o is provided
driver_o: matmul_o
	$(GPP) -c $(src)/driver.cpp -o $(build)/driver.o

#########################
# Generate
#########################
generate: gene_o
	$(GPP) -o generate \
		$(build)/generate.o

gene_o:
	$(GPP) -c $(src)/generate.cpp -o $(build)/generate.o

#########################
# Utils
#########################
clean:
	rm -f $(build)/matmul.o $(build)/generate.o matmul generate

format:
	clang-format -i -style=Google $(src)/*.cpp $(src)/*.h

#########################
# Submit
#########################
submit_2048:
	mkdir -p result
	condor_submit matmul2048.cmd

submit_4096:
	mkdir -p result
	condor_submit matmul4096.cmd
