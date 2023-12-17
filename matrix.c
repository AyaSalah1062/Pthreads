#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// Struct to hold the arguments for the thread functions
typedef struct {
    int row;         // Row index 
    int col;         // Column index 
    int** matrix1;   // First matrix
    int** matrix2;   // Second matrix
    int** result;    // Result matrix
    int cols1;       // Number of columns in the first matrix 
    int cols2;       // Number of columns in the second matrix 
} ThreadArgs;

// Function to free the dynamically allocated memory for a matrix.
void freeMatrix(int** mat, int rows) {
    for (int i = 0; i < rows; i++) {
        free(mat[i]);
    }
    free(mat);
}

// Function to compute a single element of the result matrix (Procedure 1)
void* computeElement(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    int row = threadArgs->row;
    int col = threadArgs->col;

    int sum = 0;
    for (int k = 0; k < threadArgs->cols1; k++) {
        sum += threadArgs->matrix1[row][k] * threadArgs->matrix2[k][col];
    }
    threadArgs->result[row][col] = sum;

    pthread_exit(NULL);
}

// Function to compute each row of the result matrix (Procedure 2)
void* computeRow(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    int row = threadArgs->row;

    for (int j = 0; j < threadArgs->cols2; j++) {
        int sum = 0;
        for (int k = 0; k < threadArgs->cols1; k++) {
            sum += threadArgs->matrix1[row][k] * threadArgs->matrix2[k][j];
        }
        threadArgs->result[row][j] = sum;
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // Validate command-line arguments (the name of the executable and the input file name)
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Check if file exists or not 
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int rows1, cols1, rows2, cols2;

    // Read dimensions and allocate memory for matrix1
    fscanf(file, "%d %d", &rows1, &cols1);
    int** matrix1 = (int**)malloc(rows1 * sizeof(int*));
    for (int i = 0; i < rows1; i++) {
        matrix1[i] = (int*)malloc(cols1 * sizeof(int));
        for (int j = 0; j < cols1; j++) {
            fscanf(file, "%d", &matrix1[i][j]);
        }
    }

    // Read dimensions and allocate memory for matrix2
    fscanf(file, "%d %d", &rows2, &cols2);
    int** matrix2 = (int**)malloc(rows2 * sizeof(int*));
    for (int i = 0; i < rows2; i++) {
        matrix2[i] = (int*)malloc(cols2 * sizeof(int));
        for (int j = 0; j < cols2; j++) {
            fscanf(file, "%d", &matrix2[i][j]);
        }
    }
    fclose(file);

    // Validate matrix dimensions for multiplication
    if (cols1 != rows2) {
        fprintf(stderr, "Invalid matrix dimensions for multiplication\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the result matrix 
    int** result = (int**)malloc(rows1 * sizeof(int*));
    for (int i = 0; i < rows1; i++) {
        result[i] = (int*)malloc(cols2 * sizeof(int));
    }

    // Initialize threads and timing
    pthread_t** threads = (pthread_t**)malloc(rows1 * sizeof(pthread_t*));

    struct timeval start, end;

    // Procedure 1: Each element in a thread
    gettimeofday(&start, NULL);
    for (int i = 0; i < rows1; i++) {
        threads[i] = (pthread_t*)malloc(cols2 * sizeof(pthread_t));
        for (int j = 0; j < cols2; j++) {
            ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
            args->row = i;
            args->col = j;
            args->matrix1 = matrix1;
            args->matrix2 = matrix2;
            args->result = result;
            args->cols1 = cols1;  
            args->cols2 = cols2;
            pthread_create(&threads[i][j], NULL, computeElement, args);
        }
    }

    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            pthread_join(threads[i][j], NULL);
        }
    }
    gettimeofday(&end, NULL);
    double elapsed1 = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;

    // Procedure 2: Each row in a thread
    gettimeofday(&start, NULL);
    for (int i = 0; i < rows1; i++) {
        ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        args->row = i;
        args->matrix1 = matrix1;
        args->matrix2 = matrix2;
        args->result = result;
        args->cols1 = cols1;
        args->cols2 = cols2;
        pthread_create(&threads[i][0], NULL, computeRow, args);
    }

    for (int i = 0; i < rows1; i++) {
        pthread_join(threads[i][0], NULL);
    }
    gettimeofday(&end, NULL);
    double elapsed2 = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;

    // Print result matrix for Procedure 1
    printf("Result matrix for Procedure 1:\n");
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }
    printf("Elapsed time for Procedure 1: %f seconds\n", elapsed1);

    // Print result matrix for Procedure 2
    printf("\nResult matrix for Procedure 2:\n");
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }
    printf("Elapsed time for Procedure 2: %f seconds\n", elapsed2);

    // Free allocated memory for all matrices and threads.
    for (int i = 0; i < rows1; i++) {
        free(threads[i]);
    }
    free(threads);

    freeMatrix(matrix1, rows1);
    freeMatrix(matrix2, rows2);
    freeMatrix(result, rows1);

    // Returns 0 to indicate successful execution.
    return 0;
}
