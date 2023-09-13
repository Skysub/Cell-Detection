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

// Declaring image arrays
unsigned char newInput_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char final_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGHT];
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT];
unsigned char buff_image[BMP_WIDTH][BMP_HEIGHT];

// they recomend a threshold around 90
// Since we divide the sum by 4 instead of 3, the threshold is changed
int threshold = 68;

int main(int arcg, char **argv)
{
#if _DEBUG
    clock_t startProgram, endProgram;
    double cpu_time_used_program;
    startProgram = clock();
#endif

    if (arcg != 3)
    {
#if _DEBUG
        printf("fail idiot\n");
#endif
        exit(1);
    }

#if _DEBUG
    clock_t startProcessing, endProcessing;
    double cpu_time_used_processing;
    startProcessing = clock();
#endif

    read_bitmap(argv[1], newInput_image);

    convert_to_gray(newInput_image, output_image);
    
    convert_to_binary_image(threshold, output_image);

    copy_bmp(output_image, buff_image);

#if _DEBUG
    endProcessing = clock();

    clock_t startLoop, endLoop;
    double cpu_time_used_loop;
    startLoop = clock();

    int i = 0;
#endif

    int count = 0;
    while (true)
    {
#if _DEBUG
        i++;
        printf("%d \n", i);
#endif _DEBUG

        if (erode(output_image, buff_image) == 1)
        {
            break;
        };
        count += detectCellsIterator(buff_image, final_image);
        copy_bmp(buff_image, output_image);
    };
#if _DEBUG
    endLoop = clock();
#endif

    write_bitmap(final_image, argv[2]);

    printf("%d \n", count);

#if _DEBUG
    endProgram = clock();
    cpu_time_used_program = endProgram - startProgram;
    cpu_time_used_processing = endProcessing - startProcessing;
    cpu_time_used_loop = endLoop - startLoop;
    printf("Total program time: %f ms\n", cpu_time_used_program * 1000.0 / CLOCKS_PER_SEC);
    printf("Total image proprocesseing time: %f ms\n", cpu_time_used_processing * 1000.0 / CLOCKS_PER_SEC);
    printf("Total algorithm runtime: %f ms\n", cpu_time_used_loop * 1000.0 / CLOCKS_PER_SEC);
#endif


    return 0;
}