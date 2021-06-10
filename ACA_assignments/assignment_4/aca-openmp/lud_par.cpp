
// This is a copy of the sequential code, please try adding openmp directives
// to improve performance over the sequential version
void compute_lud(float *a, int size) {

    int i,j,k;
    float sum;

    for (i=0; i<size; i++) {
        #pragma omp parallel for private(j,k,sum) shared(a,size,i)
        for (j=i; j<size; j++) { //pick elements from upper triangular and transform (including diagonal)
            sum=a[i*size+j];
            //#pragma omp ordered
            for (k=0; k<i; k++) {
                sum -= a[i * size + k] * a[k * size + j];
            }
            a[i*size+j]=sum;
        }

        #pragma omp parallel for private(j,k,sum) shared(a,size,i)
        for (j=i+1;j<size; j++){ //pick elements from lower triangular and transform (without diagonal, because diagonal elements of l are set to 1)
            sum=a[j*size+i];
            for (k=0; k<i; k++) {
                sum -= a[j * size + k] * a[k * size + i];
            }
            a[j*size+i]=sum/a[i*size+i];
        }
    }
}
