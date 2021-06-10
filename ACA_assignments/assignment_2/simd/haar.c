//authors: Keyshav Mor, Tianhao Alissa Gao

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <immintrin.h>

#define ROWS 16
#define COLS 16

#define NO_INLINE __attribute__((noinline))
#define ALIGNED16 __attribute__((aligned(16)))

static inline uint8_t avg(uint8_t a, uint8_t b) {
	return (uint8_t) (((uint16_t) a + (uint16_t) b + 1) / 2); //result shall be rounded up (thus +1)
}

static inline void haar_x_scalar(uint8_t *output, const uint8_t *input) {
	for (size_t y = 0; y < ROWS; y++) { //loop through all rows
		uint8_t tmp_input_row[COLS];
		memcpy(tmp_input_row, &input[y * COLS], COLS); //copy the yth row of input to tmp_input_row

		for (size_t lim = COLS; lim > 1; lim /= 2) { //loop though all recursive steps needed to accumulate sum of one row
			for (size_t x = 0; x < lim; x += 2) { //loop through all sum elements in tmp_input_row
				uint8_t a = tmp_input_row[x]; //a,b: 2 adjacent elements in tmp_input_row
				uint8_t b = tmp_input_row[x+1];
				uint8_t s = avg(a, b); //sum average
				uint8_t d = avg(a, -b); //diff average
				tmp_input_row[x/2] = s; //store sum average in left half of tmp_input_row
				output[y * COLS + (lim+x)/2] = d; //store diff average in right half of output
			}
		}

		output[y * COLS] = tmp_input_row[0]; //1st elem. of every row: accumulated sum, other elem.: diff averages
	}
}

static inline void haar_y_scalar(uint8_t *output, const uint8_t *input) {
	for (size_t x = 0; x < COLS; x++) { //loop through cols
		uint8_t tmp_input_col[ROWS];
		for (size_t y = 0; y < ROWS; y++) {
			tmp_input_col[y] = input[y * COLS + x]; //copy col from input to tmp_input_cols
		}

		for (size_t lim = COLS; lim > 1; lim /= 2) { //loop through all recursive steps needed to acc. the sum of one col
			for (size_t y = 0; y < lim; y += 2) { //loop through all sum elem. in col
				uint8_t a = tmp_input_col[y]; //a,b: 2 adjacent elements in col
				uint8_t b = tmp_input_col[y+1];
				uint8_t s = avg(a, b); //sum average
				uint8_t d = avg(a, -b); //diff average
				tmp_input_col[y/2] = s; // store sum average in upper half of tmp_input_col
				output[(lim+y)/2 * COLS + x] = d; //store diff average in lower half of output
			}
		}

		output[x] = tmp_input_col[0]; //1st elem. of every col: accumulated sum, other elem: diff averages
	}
}

NO_INLINE static void haar_scalar(uint8_t *output, const uint8_t *input) { //call both row and column op
	uint8_t tmp[ROWS*COLS];
	haar_x_scalar(tmp, input); //first transform all rows
	haar_y_scalar(output, tmp); //then transform all columns
	//the output is a matrix of the same size as the input
}

void print128(__m128i var, char variableName[]) { //debug print function from stackoverflow
    uint8_t *value = (uint8_t *) &var;
    printf("%s: ", variableName);
    for (int z=0; z<16; z++){
        printf("%i ", value[z]);
    }
    printf("\n");
}

void printWholeArray(const uint8_t *input, char name[]){ //print array
    int counter = 0;
    printf("%s: \n", name);
    for(int i = 0; i < 256; i++){
        printf("%i ", input[i]);
        counter ++;
        if(counter == 16){
            printf("\n");
            counter = 0;
        }
    }
    printf("\n");
}


static inline void haar_x_simd(uint8_t *output, const uint8_t *input) { //SIMD row op
	// TODO Vectorize me
	__m128i *out_vec = (__m128i *) output; // vectorize output
	__m128i *in_vec = (__m128i *) input; //vectorize input
    __m128i currentRow;
    __m128i a;
    __m128i b;
    __m128i s1, s2, s3, s4;

    /* masks for first transform iteration (lim = 16) */
    __m128i evenMask_1 = _mm_set_epi8(14,12,10,8,6,4,2,0,14,12,10,8,6,4,2,0);
    __m128i oddMask_1 = _mm_set_epi8(15,13,11,9,7,5,3,1,15,13,11,9,7,5,3,1);
    __m128i xorMask_1 = _mm_set_epi8(-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0);
    __m128i addMask_1 = _mm_set_epi8(1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0);


    /* masks for second transform iteration (lim = 8) */
    __m128i evenMask_2 = _mm_set_epi8(0,0,0,0,0,0,0,0,6,4,2,0,6,4,2,0);
    __m128i oddMask_2 = _mm_set_epi8(0,0,0,0,0,0,0,0,7,5,3,1,7,5,3,1);
    __m128i xorMask_2 = _mm_set_epi8(0,0,0,0,0,0,0,0,-1,-1,-1,-1,0,0,0,0);
    __m128i addMask_2 = _mm_set_epi8(0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0);

    /* masks for third transform iteration (lim = 4) */
    __m128i evenMask_3 = _mm_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,2,0,2,0);
    __m128i oddMask_3 = _mm_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,3,1,3,1);
    __m128i xorMask_3 = _mm_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,0,0);
    __m128i addMask_3 = _mm_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0);

    /* masks for fourth transform iteration (lim = 2) */
    __m128i evenMask_4 = _mm_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    __m128i oddMask_4 = _mm_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1);
    __m128i xorMask_4 = _mm_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0);
    __m128i addMask_4 = _mm_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0);

    /* blend masks */
    //use this to blend s1(first arg) and s2(2nd arg), take elements 0 to 7 from s2 and elements 8 to 15 from s1
    __m128i blendMask_1 = _mm_set_epi8(0,0,0,0,0,0,0,0,128,128,128,128,128,128,128,128);
    // use this to blend s1+s2(first arg) with s3(2nd arg), overwrite elements 2 and 3 with elements from s3
    __m128i blendMask_2 = _mm_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,128,128,0,0);
    // use this to blend s1+s2+s3(first arg) with s4(2nd arg), overwrite elements 0 and 1 with elements from s4
    __m128i blendMask_3 = _mm_set_epi8(0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,128);

    for (size_t y = 0; y < ROWS; y++) { //loop through all rows
        currentRow = _mm_load_si128(&in_vec[y]); // load row
        /* first transform iteration (lim = 16) */
        a = _mm_shuffle_epi8(currentRow, evenMask_1); //load all even elements
        b = _mm_shuffle_epi8(currentRow, oddMask_1); //load all odd elements
        b = _mm_xor_si128(b, xorMask_1); // invert high half of b (first xor then +1)
        b = _mm_add_epi8(b, addMask_1);
        s1 = _mm_avg_epu8(a, b); //calc unsigned sum and diff averages
        /* second transform iteration (lim = 8) */
        a = _mm_shuffle_epi8(s1, evenMask_2);
        b = _mm_shuffle_epi8(s1, oddMask_2);
        b = _mm_xor_si128(b, xorMask_2);
        b = _mm_add_epi8(b, addMask_2);
        s2 = _mm_avg_epu8(a, b);
        /* third transform iteration (lim = 4) */
        a = _mm_shuffle_epi8(s2, evenMask_3);
        b = _mm_shuffle_epi8(s2, oddMask_3);
        b = _mm_xor_si128(b, xorMask_3);
        b = _mm_add_epi8(b, addMask_3);
        s3 = _mm_avg_epu8(a, b);
        /* fourth transform iteration (lim = 2) */
        a = _mm_shuffle_epi8(s3, evenMask_4);
        b = _mm_shuffle_epi8(s3, oddMask_4);
        b = _mm_xor_si128(b, xorMask_4);
        b = _mm_add_epi8(b, addMask_4);
        s4 = _mm_avg_epu8(a, b);
        /* blend s1, s2, s3, s4 to get transform output */
        s1 = _mm_blendv_epi8(s1,s2,blendMask_1);
        s1 = _mm_blendv_epi8(s1,s3,blendMask_2);
        s1 = _mm_blendv_epi8(s1,s4,blendMask_3);
        /* store in output */
        _mm_store_si128(&out_vec[y], s1); //store s1 in output
    }
}


static inline void haar_y_simd(uint8_t *output, const uint8_t *input) { //SIMD col op
	// TODO Vectorize me
	__m128i *out_vec = (__m128i *) output; // vectorize output
	__m128i *in_vec = (__m128i *) input;
    __m128i a, b, d, inv;
    __m128i s1, s2, s3, s4, s5, s6, s7, s8;
    __m128i zero = _mm_set1_epi8(0);

    /* first transform iteration (lim = 16) */
    a = _mm_load_si128(&in_vec[0]); // lines 0 and 1
    b = _mm_load_si128(&in_vec[1]);
    s1 = _mm_avg_epu8(a,b); // sum average -> first line of new matrix
    inv = _mm_sub_epi8(zero, b);
    d = _mm_avg_epu8(a, inv); //calc diff averages
    _mm_store_si128(&out_vec[8], d); //store diff avg

    a = _mm_load_si128(&in_vec[2]); // lines 2 and 3
    b = _mm_load_si128(&in_vec[3]);
    s2 = _mm_avg_epu8(a,b); // sum avg -> 2nd line of new matrix
    inv = _mm_sub_epi8(zero, b);
    d = _mm_avg_epu8(a, inv); //diff avg
    _mm_store_si128(&out_vec[9], d); //store diff avg

    a = _mm_load_si128(&in_vec[4]); //lines 4 and 5
    b = _mm_load_si128(&in_vec[5]);
    s3 = _mm_avg_epu8(a,b);
    inv = _mm_sub_epi8(zero, b);
    d = _mm_avg_epu8(a, inv);
    _mm_store_si128(&out_vec[10], d);

    a = _mm_load_si128(&in_vec[6]);
    b = _mm_load_si128(&in_vec[7]);
    s4 = _mm_avg_epu8(a,b);
    inv = _mm_sub_epi8(zero, b);
    d = _mm_avg_epu8(a, inv);
    _mm_store_si128(&out_vec[11], d);

    a = _mm_load_si128(&in_vec[8]);
    b = _mm_load_si128(&in_vec[9]);
    s5 = _mm_avg_epu8(a,b);
    inv = _mm_sub_epi8(zero, b);
    d = _mm_avg_epu8(a, inv);
    _mm_store_si128(&out_vec[12], d);

    a = _mm_load_si128(&in_vec[10]);
    b = _mm_load_si128(&in_vec[11]);
    s6 = _mm_avg_epu8(a,b);
    inv = _mm_sub_epi8(zero, b);
    d = _mm_avg_epu8(a, inv);
    _mm_store_si128(&out_vec[13], d);

    a = _mm_load_si128(&in_vec[12]);
    b = _mm_load_si128(&in_vec[13]);
    s7 = _mm_avg_epu8(a,b);
    inv = _mm_sub_epi8(zero, b);
    d = _mm_avg_epu8(a, inv);
    _mm_store_si128(&out_vec[14], d);

    a = _mm_load_si128(&in_vec[14]);
    b = _mm_load_si128(&in_vec[15]);
    s8 = _mm_avg_epu8(a,b);
    inv = _mm_sub_epi8(zero, b);
    d = _mm_avg_epu8(a, inv);
    _mm_store_si128(&out_vec[15], d);

    /* second transform iteration (lim = 8) */
    inv = _mm_sub_epi8(zero, s2);
    d = _mm_avg_epu8(s1,inv);
    _mm_store_si128(&out_vec[4], d);
    s1 = _mm_avg_epu8(s1,s2);

    inv = _mm_sub_epi8(zero,s4);
    d = _mm_avg_epu8(s3,inv);
    _mm_store_si128(&out_vec[5], d);
    s2 = _mm_avg_epu8(s3,s4);

    inv = _mm_sub_epi8(zero, s6);
    d = _mm_avg_epu8(s5, inv);
    _mm_store_si128(&out_vec[6], d);
    s3 = _mm_avg_epu8(s5,s6);

    inv = _mm_sub_epi8(zero,s8);
    d = _mm_avg_epu8(s7, inv);
    _mm_store_si128(&out_vec[7], d);
    s4 = _mm_avg_epu8(s7,s8);

    /* third transform iteration (lim = 4) */
    inv = _mm_sub_epi8(zero,s2);
    d = _mm_avg_epu8(s1,inv);
    _mm_store_si128(&out_vec[2], d);
    s1 = _mm_avg_epu8(s1,s2);

    inv = _mm_sub_epi8(zero, s4);
    d = _mm_avg_epu8(s3, inv);
    _mm_store_si128(&out_vec[3], d);
    s2 = _mm_avg_epu8(s3,s4);

    /* fourth loop iteration (lim = 2) */
    inv = _mm_sub_epi8(zero, s2);
    d = _mm_avg_epu8(s1, inv);
    s1 = _mm_avg_epu8(s1,s2);
    _mm_store_si128(&out_vec[1], d);
    _mm_store_si128(&out_vec[0], s1);
}

NO_INLINE static void haar_simd(uint8_t *output, const uint8_t *input) { //SIMD call to row and col ops
	uint8_t tmp[ROWS*COLS] ALIGNED16;
	haar_x_simd(tmp, input);
	haar_y_simd(output, tmp);
}

static int64_t time_diff(struct timespec start, struct timespec end) { //tool to analyse the performance
    struct timespec temp;
    if (end.tv_nsec-start.tv_nsec < 0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
	return temp.tv_sec*1000000000+temp.tv_nsec;
}

static void benchmark(
		void (*fn)(uint8_t *, const uint8_t *),
		uint8_t *output, const uint8_t *input, size_t iterations, const char *msg) { //tool for benchmarking
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    for (size_t i = 0; i < iterations; i++) {
        fn(output, input);
	}

    clock_gettime(CLOCK_REALTIME, &end);
    double avg = (double) time_diff(start, end) / iterations;
    printf("%10s:\t %.3f ns\n", msg, avg);
}

static uint8_t *alloc_matrix() {
	return memalign(16, ROWS * COLS);
}

static void init_matrix(uint8_t *matrix) { //init matrix with random numbers
	for (size_t y = 0; y < ROWS; y++) {
		for (size_t x = 0; x < COLS; x++) {
			matrix[y * COLS + x] = (uint8_t) (rand() & UINT8_MAX);
		}
	}
}

static bool compare_matrix(uint8_t *expected, uint8_t *actual) {
	bool correct = true;
	for (size_t y = 0; y < ROWS; y++) {
		for (size_t x = 0; x < COLS; x++) {
			uint8_t e = expected[y * COLS + x];
			uint8_t a = actual[y * COLS + x];
			if (e != a) {
				printf(
					"Failed at (y=%zu, x=%zu): expected=%u, actual=%u\n",
					y, x, e, a
				);
				correct = false;
			}
		}
	}
	return correct;
}

int main() {
	uint8_t *input = alloc_matrix();
	uint8_t *output_scalar = alloc_matrix();
	uint8_t *output_simd = alloc_matrix();

	/* Check for correctness */
	for (size_t n = 0; n < 100; n++) {
		init_matrix(input);
		haar_scalar(output_scalar, input);
		haar_simd(output_simd, input);
		if (!compare_matrix(output_scalar, output_simd)) {
			break;
		}
	}

	/* Benchmark */
	init_matrix(input);
	benchmark(haar_scalar, output_scalar, input, 3000000, "scalar");
	benchmark(haar_simd, output_simd, input, 3000000, "simd");
	benchmark(haar_x_scalar, output_scalar, input, 3000000, "scalar_x");
	benchmark(haar_x_simd, output_simd, input, 3000000, "simd_x");
	benchmark(haar_y_scalar, output_scalar, input, 3000000, "scalar_y");
	benchmark(haar_y_simd, output_simd, input, 3000000, "simd_y");

	return 1;
}
