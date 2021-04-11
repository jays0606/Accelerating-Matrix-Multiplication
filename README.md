# Accelerating-Matrix-Multiplication

In this project, I implement a multi-threaded matrix multiplication program in a multi-core environment, specifically under 'AMD Ryzen 7 3700X 8-Core Processor'. Given two N=4096 N*N matrices, a naïve approach would take nearly 10 minutes to multiply them. The goal is to reduce this time as much as possible My final performance is 0.597s, which is about 1400% improvement.

## How to run code
1. Generating matrix data 
<br/>**make generate** 
<br/>**./generate 1024 data/input_1024.txt data/output_1024.txt (choose any integer n)**
2. Running matmul code
<br/>**make** 
<br/>**./matmul data/input_1024.txt data/output_1024.txt**


## Techniques implemented
<br/>Time consumed in a matrix multiplication program consists mainly of two parts – arithmetic operations, and read/write operations. The number of arithmetic operations, which are addition and multiplication in this case, are fixed regardless of the program. Another key thing to keep in mind is the fact that mutexs are not needed for this program. Threads doesn’t require to lock any variables or arrays, since different rows and columns of the input matrix write different parts of the output matrix. Therefore, the whole focus should be on reducing the time consumed in read/write operations.

### Technique 0 – Multi-threading
<br/>I’ve simply used openmp to implement parallelization. As mentioned earlier, mutexs are not needed, so adding ‘#pragma omp parallel for collapse(2) num_threads(128)’ in the outermost loop lead to multithreading. Collapsing the loop and setting the number of threads 128 lead to a slightly better performance. The following techniques assume the same multi-threading method.

### Technique 1 – Transposing matrix B
<br/>The naïve approach calculates input matrix A and B column-wise. This leads to the problem of false-sharing, since the cache-line saves nearby data, but data are far apart column-wise. This leads to a significant drop in performance since a great portion of the data stored in the cache are missed. In addition, due to the cache’s property of set associativity, N which are multiple of 2s will only utilize 1/8 of the cache. Luckily, this can be easily solved by transposing matrix B. This operation costs N^2, but can be ignored compared to N^3 reads. Below shows the compared result. Without transpose, N=4096 took over a minute so was killed. Comparing the performance of N=2048, transpose alone lead to 14x performance.
<br />**check code src/matmul.cpp matmul_naive function**

<img width="735" alt="table" src="https://user-images.githubusercontent.com/59599444/114296079-2b068280-9ae4-11eb-9d64-970d1a263484.png">

### Technique 2 – Blocked matrix multiplication
<br/>Although transposing matrix B does lead to significant increase in cache hit rates, there’s still room for improvement. The main problem is that for matrix B, whole row won’t fit into the L1 cache. To solve this issue, matrix A and B are decomposed into small blocks with side b, and then computation is done within each blocks. This leads to more utilizing the cache, if b is carefully chosen such that it fits into the L1 cache. L1 cache size of the server is 32KB, so number of int data it can maximally hold is 32*1024/4 = 8192. For a block with width b too fit into this cache, in theory, ideal b should be chosen around 90. Blocked matrix multiplication was also applied when transposing matrix B, slightly reducing the time taken for transpose from 0.033s to 0.012s. Looking at graph 1, best performance was b=256 with 4.56s, which is slightly better than the previous best performance of 5.74s.
<br />**check code src/matmul.cpp matmul_block function**

![out1](https://user-images.githubusercontent.com/59599444/114296068-1d50fd00-9ae4-11eb-9336-47e497bb9891.png)

### Technique 3 – Saving intermediate variable during calculation
<br/>In the final loop of the blocked matrix multiplication, matrix C is updated in-place as multiplication is done. This operation costs 1 read and 1 write, and this memory is stored in the heap address space. Stack address space is much faster to access and update, so I used an variable tmp to do the intermediate calculations in the final for loop. When the calculations were done, matrix C was updated. Best performance was achieved at b=64 with time 1.03s.
<br />**check code src/matmul.cpp matmul_block_tmp function**

![out2](https://user-images.githubusercontent.com/59599444/114296066-188c4900-9ae4-11eb-8036-df6ce66095c2.png)

### Technique 4 – Manipulating block size b for different loops
<br/>The previous technique 3 was very effective, so I thought of a method to more utilize the tmp variable. Previously, the blocked matrix multiplication used a square block for each matrix A and B, of size b=64 or 128. However, the actual calculation being done was in the final loop, so I decided to increase the block size in the final loop while decreasing it in the first and second loop. I tried with various sizes, and below is the result. Size of block b was fixed to 64 since it achieved the best performance in technique 3. Best performance was achieved when size of outer 2 block sizes=b/2 and 8b in the innermost loop, only taking 0.597s for the execution. 
<br />**check code src/matmul.cpp matmul_optimized function**

![out3](https://user-images.githubusercontent.com/59599444/114296063-14602b80-9ae4-11eb-9058-5c7180b8acc6.png)

### Comparison for all the techniques 
![out4](https://user-images.githubusercontent.com/59599444/114296056-0d391d80-9ae4-11eb-9fbf-82b378267c8b.png)

### References 
<br/> 1. Yonsei University CSI-4119 MULTICORE AND GPU PROGRAMMING Lecture Notes
<br/> 2. Remzi H.Arpaci-Dusseau, Operating Systems: Three Easy Pieces Ch13
<br/> 3. Ziji Shi, Understanding C++ Performance (1): Stack and Heap.


