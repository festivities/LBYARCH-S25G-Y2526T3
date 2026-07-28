#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define RUNS 30

void imgCvtGrayDoubleToInt(int height, int width, const double *input, unsigned char *output);

static void imgCvtGrayDoubleToIntRef(int height, int width, const double *input, unsigned char *output) {
    const int count = height * width;

    for(int index = 0; index < count; ++index)
        output[index] = (unsigned char) rint(input[index] * 255.0); // rint() rounds like CVTSD2SI
}

static void *allocate(size_t bytes) {
    void *block = malloc(bytes);

    if(block == NULL) {
        fprintf(stderr, "Out of memory.\n");
        exit(EXIT_FAILURE);
    }

    return block;
}

static int checkCorrectness(int height, int width, const double *input, const unsigned char *output) {
    const int count = height * width;
    unsigned char *expected = allocate((size_t) count);
    int matched = 1;

    imgCvtGrayDoubleToIntRef(height, width, input, expected);

    for(int index = 0; index < count; ++index)
        if(output[index] != expected[index]) {
            printf("Mismatch at pixel %d: assembly gave %u, expected %u\n",
                   index, output[index], expected[index]);
            matched = 0;
            break;
        }

    free(expected);
    return matched;
}

static void printImage(int height, int width, const unsigned char *image) {
    for(int row = 0; row < height; ++row) {
        for(int col = 0; col < width; ++col)
            printf("%s%3u", col ? ", " : "", image[row * width + col]);

        printf("\n");
    }
}

static void convertUserImage(void) {
    int height, width;

    printf("Enter the height and width of the image: ");
    if(scanf("%d %d", &height, &width) != 2 || height <= 0 || width <= 0) {
        fprintf(stderr, "Invalid image size.\n");
        exit(EXIT_FAILURE);
    }

    const int count = height * width;
    double *input = allocate((size_t) count * sizeof(double));
    unsigned char *output = allocate((size_t) count);

    printf("Enter %d double float pixel values in [0.0, 1.0]:\n", count);
    for(int index = 0; index < count; ++index)
        if(scanf("%lf", &input[index]) != 1) {
            fprintf(stderr, "Invalid pixel value at index %d.\n", index);
            exit(EXIT_FAILURE);
        }

    imgCvtGrayDoubleToInt(height, width, input, output);

    printf("\nInteger pixel values:\n");
    printImage(height, width, output);

    printf("\nCorrectness check: %s\n", checkCorrectness(height, width, input, output) ? "PASS" : "FAIL");

    free(input);
    free(output);
}

static void benchmark(int height, int width) {
    const int count = height * width;
    double *input = allocate((size_t) count * sizeof(double));
    unsigned char *output = allocate((size_t) count);
    LARGE_INTEGER frequency, start, end;
    char size[16];

    for(int index = 0; index < count; ++index)
        input[index] = rand() / (double) RAND_MAX;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    for(int run = 0; run < RUNS; ++run)                 // only the asm call is timed
        imgCvtGrayDoubleToInt(height, width, input, output);

    QueryPerformanceCounter(&end);

    const double average = (end.QuadPart - start.QuadPart) * 1000.0 / (frequency.QuadPart * (double) RUNS);

    sprintf(size, "%d x %d", height, width);
    printf("  %-13s %9d   %15.6f   %s\n", size, count, average,
           checkCorrectness(height, width, input, output) ? "PASS" : "FAIL");

    free(input);
    free(output);
}

int main(void) {
    srand(2026);

    convertUserImage();

    printf("\nAverage execution time of imgCvtGrayDoubleToInt() over %d runs:\n\n", RUNS);
    printf("  %-13s %9s   %15s   %s\n", "Size", "Pixels", "Avg time (ms)", "Correctness");
    printf("  ---------------------------------------------------------------\n");

    benchmark(10, 10);
    benchmark(100, 100);
    benchmark(1000, 1000);

    return 0;
}
