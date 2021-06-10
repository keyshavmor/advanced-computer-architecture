#authors: Alissa, Keyshav
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
N_SAMPLES:	.word	10
sample:		.double 1.0,2.0,1.0,2.0,1.0,1.0,2.0,1.0,2.0,1.0
result:		.double	0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0

one:		.double 1.0

CR: 		.word32 0x10000
DR:		.word32 0x10008

.text
		ld $t1, N_SAMPLES($zero)	# $t1: N_SAMPLES
		mtc1 $zero,f0		# move zero to f0
		slti $t0,$t1,3		# check if N_SAMPLES < 3
		l.d f1, coeff($zero)	# f1: 1st element of coeff
		l.d f2, coeff+8($zero)	# f2: 2nd element of coeff	
		c.lt.d f0,f1		# check if f1 > 0 (branch delay slot)
		bc1t skip_1		# skip inversion of f1 if f1 > 0
		bnez $t0,exit		# if N_SAMPLES < 3: exit immediately (branch delay slot)
		sub.d f1,f0,f1		# f1 = 0 - f1 (7 cycles) (branch delay slot)
skip_1:		
		c.lt.d f0,f2		# check if f2 > 0
		bc1t skip_2		# skip inversion of f2 if f2 > 0
		l.d f3, coeff+16($zero)	# f3: 3rd element of coeff (branch delay slot)
		sub.d f2,f0,f2 		# f2 = 0 - f2 (7 cycles)
skip_2:
		l.d f15, one($zero)	# f15: 1 
		l.d f6, sample($zero)	# f6: sample[0]
		add.d f4,f1,f2		# f4 = f1 + f2 (7 cycles)
		c.lt.d f0,f3		# check if f3 > 0
		bc1t skip_3		# skip inversion of f3 if f3 > 0
		daddi $t5,$t1,-2	# $t5 = N_SAMPLES - 2 (branch delay slot)
		sub.d f3,f0,f3 		# f3 = 0 -f3 (7 cycles)
skip_3:
		s.d f6, result($zero) 	# result[0] = sample[0]
		l.d f1, coeff($zero)	# f1: 1st element of coeff (reload original value)
		add.d f5,f4,f3		# f5 = f4 + f3 -> f5: norm (7 cycles)
		l.d f2, coeff+8($zero)	# f2: 2nd element of coeff (reload original value)
		l.d f3, coeff+16($zero)	# f3: 3rd element of coeff (reload original value)		
		div.d f5,f15,f5		# f5 = 1/norm (norm factor) (16 cycles)
		dadd $t6,$zero,$zero	# $t6: loop index counter, starts at 0
		dsll $t5,$t5,3		# $t5: 8 * (N_SAMPLES-2)
loop:
		daddi $t6,$t6,8		# increment loop index counter	
		l.d f6, sample-8($t6)	# f6: sample[i-1]
		l.d f7, sample($t6)	# f7: sample[i]
		l.d f8, sample+8($t6)	# f8: sample[i+1]
		mul.d f9,f6,f1		# f9 = sample[i-1]*coeff[0] (8 cycles)
		mul.d f10,f7,f2		# f10 = sample[i]*coeff[1] (8 cycles)
		mul.d f11,f8,f3		# f11 = sample[i+1]*coeff[2] (8 cycles)		
		add.d f12,f9,f10	# f12 = f9 + f10
		add.d f13,f11,f12	# f13 = f11 + f12 (weighted sum of samples)
		mul.d f14,f13,f5	# f14 = weighted sum of samples * (1/norm)
		s.d f14, result($t6) 	# store in result[i]
		bne $t6,$t5,loop	# check if we are done
exit:		
		s.d f8, result+8($t6)	# result[N_SAMPLES-1] = samples[N_SAMPLES-1]			
		halt
