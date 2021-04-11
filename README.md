# Accelerating-Matrix-Multiplication

In this project, I implement a multi-threaded matrix multiplication program. Given two N=4096 N*N matrices, a naïve approach would take nearly 10 minutes to multiply them. The goal is to reduce this time as much as possible My final performance is 0.597s, which is about 1400% improvement.


## Techniques implemented

  Time consumed in a matrix multiplication program consists mainly of two parts – arithmetic operations, and read/write operations. The number of arithmetic operations, which are addition and multiplication in this case, are fixed regardless of the program. Another key thing to keep in mind is the fact that mutexs are not needed for this program. Threads doesn’t require to lock any variables or arrays, since different rows and columns of the input matrix write different parts of the output matrix. Therefore, the whole focus should be on reducing the time consumed in read/write operations.

### Technique 0 – Multi-threading
I’ve simply used openmp to implement parallelization. As mentioned earlier, mutexs are not needed, so adding ‘#pragma omp parallel for collapse(2) num_threads(128)’ in the outermost loop lead to multithreading. Collapsing the loop and setting the number of threads 128 lead to a slightly better performance. The following techniques assume the same multi-threading method.

### Technique 1 – Transposing matrix B
The naïve approach calculates input matrix A and B column-wise. This leads to the problem of false-sharing, since the cache-line saves nearby data, but data are far apart column-wise. This leads to a significant drop in performance since a great portion of the data stored in the cache are missed. In addition, due to the cache’s property of set associativity, N which are multiple of 2s will only utilize 1/8 of the cache. Luckily, this can be easily solved by transposing matrix B. This operation costs N^2, but can be ignored compared to N^3 reads. Below shows the compared result. Without transpose, N=4096 took over a minute so was killed. Comparing the performance of N=2048, transpose alone lead to 14x performance.

### Technique 2 – Blocked matrix multiplication
Although transposing matrix B does lead to significant increase in cache hit rates, there’s still room for improvement. The main problem is that for matrix B, whole row won’t fit into the L1 cache. To solve this issue, matrix A and B are decomposed into small blocks with side b, and then computation is done within each blocks. This leads to more utilizing the cache, if b is carefully chosen such that it fits into the L1 cache. L1 cache size of the server is 32KB, so number of int data it can maximally hold is 32*1024/4 = 8192. For a block with width b too fit into this cache, in theory, ideal b should be chosen around 90. Blocked matrix multiplication was also applied when transposing matrix B, slightly reducing the time taken for transpose from 0.033s to 0.012s. Looking at graph 1, best performance was b=256 with 4.56s, which is slightly better than the previous best performance of 5.74s.
