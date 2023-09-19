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
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

//Helps with debugging when the output doesn't contain the entire list of cell coordinates
#define PRINT_CELL_LIST_IN_DEBUG 0
#define OUTPUT_INTERMEDIARY_IMAGES 0
#define MAIN_IMAGE_OUTPUT 1

// Declaring image arrays
unsigned char debug_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char buff1_image[BMP_WIDTH][BMP_HEIGHT];
unsigned char buff2_image[BMP_WIDTH][BMP_HEIGHT];

// they recomend a threshold around 90
// Since we divide the sum by 4 instead of 3, the threshold is changed
int threshold = 68;
int folderMode = 0;

int main(int arcg, char **argv)
{
#if _DEBUG
    printf("%s \n", argv[1]);
    printf("%s \n", argv[2]);

    clock_t startProgram, endProgram;
    double cpu_time_used_program;
    startProgram = clock();
#endif

    if (arcg < 2)
    {
#if _DEBUG
        printf("fail idiot\n");
#endif
        exit(1);
    }

    //File list for folder mode
    char file_list[100][60];
    int file_list_length = 0;
    char output_folder[] = "output/";

    //Check if the input argument is a folder or a file
    struct stat buf;
    stat(argv[1], &buf);
    if (S_ISDIR(buf.st_mode)) {
        folderMode = true;

        //Prints an error message and exits if the directory could not be opened
        if(getImagePathsFromFolder(argv[1], file_list, &file_list_length)) exit(1);
    }
    else file_list_length++;

    //Initiates the for loop for processing all the file in the older
    //If folder mode is not active, then the for loop runs once
    for (int f = 0; f < file_list_length; f++)
    {
#if _DEBUG_FOLDER
        printf("\n");
        printf(file_list[f]);
        printf("\n");
#endif

    char img_name[60] = ""; //The buffer is larger than the names we allow the input file to have
                            //This is because we add a suffix to this string later
    if (folderMode) {
        //The name length is guaranteed in folderMode
        int folder_len = strlen(argv[1]) + 1;
        int len = strlen(file_list[f]) - 4;
        memcpy(img_name, file_list[f] + folder_len, len - folder_len); //Only adding the name part
    }
    else {
        //Copies the name of the input file into img_name (no file extension)
        int len = strlen(argv[1]) - 4;
        if (len > 39) len = 39;
        memcpy(img_name, argv[1], len);
    }

#if _DEBUG
    clock_t startProcessing, endProcessing;
    double cpu_time_used_processing;
    startProcessing = clock();
#endif

#if OUTPUT_INTERMEDIARY_IMAGES
    //This code block runs only for testing purposes

    //Names the output files
    char grey_suffix[] = "_grey.bmp";
    char binary_suffix[] = "_binary.bmp";

    char grey_name[80] = "";
    char binary_name[80] = "";

    if (folderMode) {
        //Remember to output the files in the output folder
        strcat(grey_name, output_folder);
        strcat(binary_name, output_folder);
    }

    strcat(grey_name, img_name);
    strcat(grey_name, grey_suffix);
    strcat(binary_name, img_name);
    strcat(binary_name, binary_suffix);

    //loads the image file
    if (folderMode) { 
        read_bitmap(file_list[f], input_image);
    }
        else read_bitmap(argv[1], input_image);
    convert_to_gray(input_image, buff1_image);

    //Output the greyscale version
    addThirdChannel(buff1_image, debug_image);
    write_bitmap(debug_image, grey_name);

    convert_to_binary_image(threshold, buff1_image);

    //Output the binary version
    addThirdChannel(buff1_image, debug_image);
    write_bitmap(debug_image, binary_name);

    copy_bmp(buff1_image, buff2_image);
#else
    if (folderMode) {
        read_bitmap(file_list[f], input_image);
    }
    else read_bitmap(argv[1], input_image);
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
        printf("%d ", i++); //Prints the amount of times the loop has run
#endif

        if (erode(buff1_image, buff2_image)) {
            break;
        }
        count += detectCellsIterator(buff2_image, cell_list, &cell_list_length);
        copy_bmp(buff2_image, buff1_image);
    }
#if _DEBUG
    printf("\n");
    endLoop = clock();
#endif
    printf("%d \n", count); //Prints the final cell count

#if !MAIN_IMAGE_OUTPUT
    continue;
#endif

    for (short i = 0; i < cell_list_length; i++)
    {
#if (!_DEBUG || PRINT_CELL_LIST_IN_DEBUG) && !NDEBUG_FOLDER
        printf("(%d, %d)\n", cell_list[i][0], cell_list[i][1]);
#endif
        draw_red_cross(input_image, cell_list[i][0], cell_list[i][1]);
    }

    char out_name[60] = "";
    char suffix[] = "_output.bmp"; //default suffix

    //Choosing a name for the output file based on whether there is given a name/suffix for the output file/files
    if (arcg < 3) {
        strcat(img_name, suffix);
        strcat(out_name, img_name);
    }
    else if (folderMode) {
        strcat(img_name, argv[2]);
        strcat(out_name, img_name);
    }
    else
        strcat(out_name, argv[2]);

    //outputs the final processed image
    if (folderMode) {
        char folder_out_name[70] = "";
        strcat(folder_out_name, output_folder);
        char* bruh = (&folder_out_name[7]);
        sprintf(bruh, "%d_", count);
        strcat(folder_out_name, out_name);
        write_bitmap(input_image, folder_out_name);
    }
    else write_bitmap(input_image, out_name);

#if _DEBUG
    cpu_time_used_processing = endProcessing - startProcessing;
    cpu_time_used_loop = endLoop - startLoop;
    printf("Img proprocesseing time: %f ms\n", cpu_time_used_processing * 1000.0 / CLOCKS_PER_SEC);
    printf("Algorithm running time:  %f ms\n", cpu_time_used_loop * 1000.0 / CLOCKS_PER_SEC);
#endif

    }

#if _DEBUG
    endProgram = clock();
    cpu_time_used_program = endProgram - startProgram;
    printf("\nTotal program time: %f ms\n", cpu_time_used_program * 1000.0 / CLOCKS_PER_SEC);
#endif

    return 0;
}

int getImagePathsFromFolder(const char folder[], char file_list[100][60], int* file_list_length) {

    DIR *dir;
    struct dirent* item;

    //Opens the folder and readies it for use, returns 1 and exits if dir is null
    dir = opendir(folder);
    if(dir == NULL){
        printf("Can't open folder %s", folder);
        return 1;
    }

    //Loops through all items in the directory
    //adds their names to a list if the file extension is .bmp,
    //if the name isn't too short or too long, and if the list has room
    while ((item = readdir(dir)) != NULL) {

        //Checking filetype
        if (item->d_namlen > 3 &&
           item->d_namlen < 40 &&
           item->d_name[item->d_namlen - 4] == '.' &&
           item->d_name[item->d_namlen - 3] == 'b' &&
           item->d_name[item->d_namlen - 2] == 'm' &&
           item->d_name[item->d_namlen - 1] == 'p' &&
           *file_list_length < 100) {

            char temp[60] = "";
            strcat(temp, folder);
            strcat(temp, "/");
            strcat(temp, item->d_name);
            strcpy(file_list[*file_list_length], temp);
            (*file_list_length)++;
        }
    }
    closedir(dir);

    //creates the output folder if it doesn't already exist
    struct stat st;
    if (stat("output", &st) == -1) {
        mkdir("output", 0777);
    }

    return 0;
}