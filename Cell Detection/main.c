//To compile (win): gcc cbmp.c sun.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp test.bmp

// A Cool attempt to draw red crosses
// More text on the way

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "cbmp.h"
#include "sun.h"

// Declaring input and output images in 3 demensions
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char buff_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];

// they recomend a threshold around 90
int threshold = 90;

int main(int arcg, char **argv)
{

    clock_t start, end;
    double cpu_time_used;
    start = clock();

    if (arcg != 3)
    {
        printf("fail idiot\n");
        exit(1);
    }

    read_bitmap(argv[1], input_image);

    convert_to_gray(input_image, output_image);

    convert_to_binary_image(threshold, output_image);

    copy_bmp(output_image, buff_image);

    int i = 0;
    int count = 0;
    while (true)
    {
        i++;
        printf("%d \n", i);
        erode(output_image, buff_image);
        copy_bmp(output_image, buff_image);
        if (erode(buff_image, output_image) == 1)
        {
            break;
        };
        count = count + detectCellsIterator(output_image, input_image);
    };

    write_bitmap(input_image, argv[2]);

    printf("%d \n", count);

    end = clock();
    cpu_time_used = end - start;
    printf("Total time: %f ms\n", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);

    return 0;
}