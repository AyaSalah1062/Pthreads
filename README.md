It is required to implement two popular algorithms as multi-threaded ones. They are as follows:
FIRST:Matrix Multiplication:
It is required to implement two variations of this algorithm:
1. The computation of each element of the output matrix happens in a thread.
2. The computation of each row of the output matrix happens in a thread.
SECOND:Merge Sort:
Merge sort is an O (n log n) comparison-based sorting algorithm. It is a divide-and-conquer algorithm. Conceptually, a merge sort
works as follows:
If the list is of length 0 or 1, then it is already sorted. Otherwise:
1. Divide the unsorted list into two sub-lists of about half the size.
2. Sort each sub-list recursively by re-applying the merge sort.
3. Merge the two sub-lists back into one sorted list
So you are required to implement it using pthreads. Each time the list is divided; two threads are created to do merge-sort on each
half separately. This step is repeated recursively until each sub-list has only one element.
