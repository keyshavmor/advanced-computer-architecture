void compute_lud(float *a, int size) {

    int i,j,k;
    float sum;

    for (i=0; i<size; i++) {
        for (j=i; j<size; j++) { //computes the i-th row of U
            sum=a[i*size+j];
            for (k=0; k<i; k++) {
                sum -= a[i * size + k] * a[k * size + j];
            }
            a[i*size+j]=sum;
        }

        for (j=i+1;j<size; j++){ //computes the i-th column of L
            sum=a[j*size+i];
            for (k=0; k<i; k++) {
                sum -= a[j * size + k] * a[k * size + i];
            }
            a[j*size+i]=sum/a[i*size+i];
        }
    }
}
