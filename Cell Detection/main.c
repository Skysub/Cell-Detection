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

//Helps with debugging when the output doesn't contain the entire list of cell coordinates
#define PRINT_CELL_LIST_IN_DEBUG 0
#define OUTPUT_INTERMEDIARY_IMAGES 1

// Declaring image arrays
unsigned char debug_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char buff1_image[BMP_WIDTH][BMP_HEIGHT];
unsigned char buff2_image[BMP_WIDTH][BMP_HEIGHT];

// they recomend a threshold around 90
// Since we divide the sum by 4 instead of 3, the threshold is changed
int threshold = 68;

int main(int arcg, char **argv)
{
#if _DEBUG
    printf("%s \n", argv[1]);
    printf("%s \n", argv[2]);

    clock_t startProgram, endProgram, startProcessing, endProcessing;
    double cpu_time_used_program, cpu_time_used_processing;
    startProgram = clock();
    startProcessing = clock();
#endif

    if (arcg != 3)
    {
#if _DEBUG
        printf("fail idiot\n");
#endif
        exit(1);
    }

#if OUTPUT_INTERMEDIARY_IMAGES
    read_bitmap(argv[1], input_image);
    convert_to_gray(input_image, buff1_image);

    addThirdChannel(buff1_image, debug_image);
    write_bitmap(debug_image, "img_binary.bmp");

    convert_to_binary_image(threshold, buff1_image);

    addThirdChannel(buff1_image, debug_image);
    write_bitmap(debug_image, "img_grey.bmp");

    copy_bmp(buff1_image, buff2_image);
#else
    read_bitmap(argv[1], input_image);
    convert_to_gray(input_image, buff1_image);
    convert_to_binary_image(threshold, buff1_image);
    copy_bmp(buff1_image, buff2_image);
#endif


#if _DEBUG
    endProcessing = clock();

    clock_t startLoop, endLoop;
    double cpu_time_used_loop;
    startLoop = clock();

    int i = 0;
#endif

    int count = 0;
    short cell_list[MAX_CELLS][2];
    short cell_list_length = 0;
    while (true)
    {
#if _DEBUG
        i++;
        printf("%d \n", i);
#endif _DEBUG

        if (erode(buff1_image, buff2_image)){
            break;
        }
        count += detectCellsIterator(buff2_image, cell_list, &cell_list_length);
        copy_bmp(buff2_image, buff1_image);
    }
#if _DEBUG
    endLoop = clock();
#endif
    printf("%d \n", count);

    for (short i = 0; i < cell_list_length; i++)
    {
#if !_DEBUG || PRINT_CELL_LIST_IN_DEBUG
        printf("(%d, %d)\n", cell_list[i][0], cell_list[i][1]);
#endif
        draw_red_cross(input_image, cell_list[i][0], cell_list[i][1]);
    }
    write_bitmap(input_image, argv[2]);

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