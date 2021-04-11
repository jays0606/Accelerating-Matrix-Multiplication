############################################
##
## Matrix Multiplication Condor command file
##
############################################

executable	 = matmul
output		 = result/matmul.out
error		 = result/matmul.err
log		     = result/matmul.log
request_cpus = 16
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
transfer_input_files    = data/input_4096.txt, data/output_4096.txt
arguments	            = input_4096.txt output_4096.txt 0
queue