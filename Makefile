CC = gcc
CFLAGS = -Wall -pthread -g
TARGET_MERGE = sort
TARGET_MATMUL = mul

all: $(TARGET_MERGE) $(TARGET_MATMUL)

$(TARGET_MERGE): 7361-sort.c
	$(CC) $(CFLAGS) -o $(TARGET_MERGE) 7361-sort.c

$(TARGET_MATMUL): 7361-matrix.c
	$(CC) $(CFLAGS) -o $(TARGET_MATMUL) 7361-matrix.c

clean:
	rm -f $(TARGET_MERGE) $(TARGET_MATMUL)
