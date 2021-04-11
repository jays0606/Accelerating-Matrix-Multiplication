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
transfer_input_files    = data/input_2048.txt, data/output_2048.txt
arguments	            = input_2048.txt output_2048.txt 0
queue
