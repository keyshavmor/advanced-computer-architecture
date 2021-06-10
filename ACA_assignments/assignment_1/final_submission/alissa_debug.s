#author: Tianhao Alissa Gao, Keyshav Mor
#
#data types:
# .word - 64 bit integer
# .word32 - 32 bit integer
# .double - 64 bit floating point number 
# 
#config:
#	addition delay: 3
#	multiplication delay: 4
#	division delay: 12
#	forwarding and delay slot enabled

.data
N_COEFFS:	.word	3
coeff:		.double	0.5,1.0,0.5

# N_SAMPLES:	.word	5
# sample:		.double 1.0,2.0,1.0,2.0,1.0
# result:		.double	0.0,0.0,0.0,0.0,0.0
N_SAMPLES:	.word	40
sample:		.double 1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0
result:		.double	0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0

CR: 		.word32 0x10000
DR:		    .word32 0x10008

.text
    	ld $t2, N_SAMPLES($zero)	# $t2: N_SAMPLES
		mtc1 $zero,f0		# move zero to f0
		slti $t0,$t2,3		# check if N_SAMPLES < 3
		l.d f1, coeff($zero)	# f1: 1st element of coeff
		l.d f2, coeff+8($zero)	# f2: 2nd element of coeff
		c.lt.d f0,f1		# check if f1 > 0 
		bc1t skip_1		# skip inversion of f1 if f1 > 0
		bnez $t0,exit_code	# if N_SAMPLES < 3: exit immediately (branch delay slot)
		sub.d f1,f0,f1		# f1 = 0 - f1  (7 cycles)
skip_1:		
		c.lt.d f0,f2		# check if f2 > 0 
		bc1t skip_2		# skip inversion of f2 if f2 > 0
		l.d f6, sample($zero)	# f6: sample[0]
		sub.d f2,f0,f2 		# f2 = 0 - f2  (7 cycles)
skip_2:
		l.d f3, coeff+16($zero)	# f3: 3rd element of coeff
		l.d f21, coeff($zero)	# reload coeff[0]
		l.d f22, coeff+8($zero)	# reload coeff[1]
		l.d f23, coeff+16($zero)	# reload coeff[2]		
		add.d f4,f1,f2		# f4 = f1 + f2
		c.lt.d f0,f3		# check if f3 > 0 
		bc1t skip_3		# skip inversion of f3 if f3 > 0
		nop
		sub.d f3,f0,f3 		# f3 = 0 -f3 	
skip_3:	
    	dsll $t1,$t2,3          # $t1: 8 * N_SAMPLES
		add.d f5,f4,f3		# f5 = f4 + f3 -> f5: norm 
		l.d f7, sample+8($zero) # f7:sample[1]
    	l.d f8,sample+16($zero) # f7:sample[2]
		nop
		daddi $t1,$t1,-8        # $t1: 8 * (N_SAMPLES-1)
		mul.d f9,f6,f21		# f9 = sample[i-1]*coeff[0] (8 cycles)
		mul.d f10,f7,f22	# f10 = sample[i]*coeff[1] (8 cycles)
		mul.d f11,f8,f23	# f11 = sample[i+1]*coeff[2] (8 cycles)
		s.d f6, result($zero) 	# result[0] = sample[0]
		l.d f17,sample($t1) 	# load value of last element in f17
		add.d f12,f9,f10	# f12 = f9 + f10
		daddi $t6,$zero,2	# $t6: loop index counter, starts at 2
		daddi $t0,$zero,8	# storage index starts at address of sample[1]
		add.d f13,f11,f12	# f13 = f11 + f12 (weighted sum of samples)
		mov.d f6,f7		# f6 = f7 (update of i-1)	
		mov.d f7,f8		# f7 = f8 (update of i)
		l.d f8, sample+24($zero)	# f8 = new sample (update of i+1)

loop:	
		div.d f14,f13,f5	# f14 = weighted sum of samples / norm
		mul.d f9,f6,f21		# f9 = sample[i-1]*coeff[0] (8 cycles)
		mul.d f10,f7,f22	# f10 = sample[i]*coeff[1] (8 cycles)		
		mul.d f11,f8,f23	# f11 = sample[i+1]*coeff[2] (8 cycles)
		mov.d f6,f7		# f6 = f7 (update of i-1)	
		s.d f14, result($t0) 	# store in result[i]
		dsll $t0,$t6,3		# $t0 = 8 * i		
		add.d f12,f9,f10	# f12 = f9 + f10
		daddi $t6,$t6,1		# increment loop index counter	
		mov.d f7,f8		# f7 = f8 (update of i)
		add.d f13,f11,f12	# f13 = f11 + f12 (weighted sum of samples)
		bne $t6,$t2,loop	# check if loop is over
    	l.d f8, sample+16($t0)	# f8 = new sample (update of i+1)
	
exit_code:  
		s.d f17, result($t1) 	# result[N_SAMPLES-1] = sample[N_SAMPLES-1] 
		   
printResult:		
			lwu $t3,CR($zero) 	# $t3: Control Register
			lwu $t4,DR($zero) 	# $t4: Data Register
			daddi $t2,$zero,3 	# $t2 = 3 (output is floating point)
			daddi $t7,$zero,result	# print loop addr. counter, starts at result base addr.
			ld $t1, N_SAMPLES($zero)	# $t1: N_SAMPLES
			daddi $t5,$t1,-1	# $t5 = N_SAMPLES - 1
			dsll $t8,$t5,3		# $t8: (N_SAMPLES -1) * 8
			dadd $t9,$t7,$t8	# $t9: last addr. of result
			
printNext:			
			l.d f31, ($t7)		# f31: load element from result
			s.d f31, ($t4)		# output element from result
			sd $t2,($t3)		# ... to screen
			bne $t7,$t9,printNext	# print next until last addr of result
			daddi $t7,$t7,8		# increment addr. counter
		   			
	       halt
