#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 500

// Global array to store the elements
int* arr;

// Structure to represent the start and end indices of a thread's work
typedef struct {
    int start;
    int end;
} ThreadArgs;

// Function to merge two sorted halves of the array
void merge(int start, int mid, int end) {
    int n1 = mid - start + 1;
    int n2 = end - mid;

    // Dynamically allocate memory for left and right arrays
    int* left = malloc(n1 * sizeof(int));
    int* right = malloc(n2 * sizeof(int));

    // Copy data to temporary arrays left[] and right[]
    for (int i = 0; i < n1; i++)
        left[i] = arr[start + i];
    for (int j = 0; j < n2; j++)
        right[j] = arr[mid + 1 + j];

    // Merge the two halves back into arr[start..end]
    int i = 0, j = 0, k = start;
    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        } else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of left[], if any
    while (i < n1) {
        arr[k] = left[i];
        i++;
        k++;
    }

    // Copy the remaining elements of right[], if any
    while (j < n2) {
        arr[k] = right[j];
        j++;
        k++;
    }

    // Free the dynamically allocated memory
    free(left);
    free(right);
}

// Function to perform merge sort on a portion of the array
void* mergeSortThread(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    int start = threadArgs->start;
    int end = threadArgs->end;

    // If the array has more than one element, perform merge sort
    if (start < end) {
        // Calculate the middle index
        int mid = (start + end) / 2;

        // Create arguments for left and right halves
        ThreadArgs leftArgs = {start, mid};
        ThreadArgs rightArgs = {mid + 1, end};

        // Create threads for left and right halves
        pthread_t leftThread, rightThread;
        pthread_create(&leftThread, NULL, mergeSortThread, &leftArgs);
        pthread_create(&rightThread, NULL, mergeSortThread, &rightArgs);

        // Wait for both threads to finish
        pthread_join(leftThread, NULL);
        pthread_join(rightThread, NULL);

        // Merge the sorted halves
        merge(start, mid, end);
    }

    // Notify that the thread has completed its task
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // Validate command-line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open the input file
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the size of the array from the file
    int n;
    fscanf(file, "%d", &n);

    // Validate array size
    if (n <= 0 || n > MAX_SIZE) {
        fprintf(stderr, "Invalid array size\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Dynamically allocate memory for the array
    arr = (int*)malloc(n * sizeof(int));

    // Read array elements from the file
    for (int i = 0; i < n; i++) {
        fscanf(file, "%d", &arr[i]);
    }
    fclose(file);

    // Create a thread for the main merge sort operation
    pthread_t mainThread;
    ThreadArgs mainArgs = {0, n - 1};
    pthread_create(&mainThread, NULL, mergeSortThread, &mainArgs);

    // Wait for the main thread to finish
    pthread_join(mainThread, NULL);

    // Print the sorted array
    printf("Sorted array is:");
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    // Free the dynamically allocated memory
    free(arr);

    return 0;
}
