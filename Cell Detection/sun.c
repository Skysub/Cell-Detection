#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"
#include "sun.h"

// int structuring_element[3][3] = {{0, 1, 0}, {1, 1, 1}, {0, 1, 0}};

// This function converts the 3 dimension images to a 2 dimension image in gray scale. It goes through every pixel and calculate the averages value from the RGB values.
void convert_to_gray(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            //The most accurate (and slow) method
            //output_image[x][y] = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) / 3;

            //Bitshifting to divide by 4. Threshold is changed to account
            output_image[x][y] = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) >> 2;

            //Sum times 11 and then bitshift to divide by 32. If 1 would be the perfect result, then this gives the result 1.03125
            //output_image[x][y] = ((input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) * 11) >> 5;
        }
    }
}

// This function converts the image to a binary with only white and black pixels. The threshold is a varible input.
void convert_to_binary_image(int threshold, unsigned char buff_image[BMP_WIDTH][BMP_HEIGHT])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            if (buff_image[x][y] <= threshold)
                buff_image[x][y] = 0;
            else
                buff_image[x][y] = 255;
        }
    }
}

// This function uses otsu's method to calculate a threshold for the binary conversion of the image
int calculate_threshold_otsu(unsigned char buff_image[BMP_WIDTH][BMP_HEIGHT])
{
    //Creating the histogram
    int levels[256];
    for (int i = 0; i < 256; i++){
        levels[i] = 0;
    }

    //Creatres histogram of occurences of a pixel values
    for (int x = 0; x < BMP_WIDTH; x++){
        for (int y = 0; y < BMP_HEIGHT; y++){
            levels[buff_image[x][y]]++;
        }
    }

    //Creates an array where each of the levels are converted to their ratio of occurence in the image
    float totalPixels = BMP_WIDTH * BMP_HEIGHT;
    float probabilities[256];
    for (int i = 0; i < 256; i++) {
        probabilities[i] = levels[i] / totalPixels;
    }

    //Loops through all possible thresholds and calculates their inter-class variance (otsu's method)
    int largest = 0;
    int best_threshold = 0;
    for (int i = 1; i < 256; i++)
    {
        float w0 = 0, u0 = 0, w1 = 0, u1 = 0;
        for (int j = 0; j < i; j++)
        {
            w0 += probabilities[j];
            u0 += j * probabilities[j];
        }
        if (w0 < 0.01f || w0 > 0.99f) continue;
        w1 = 1 - w0;
        u0 = u0 / w0;

        for (int j = i; j < 256; j++)
        {
            u1 += j * probabilities[j];
        }
        u1 = u1 / w1;
        
        float uT = w0 * u0 + w1 * u1;

        //int sigma = w0 * (u0 * u0 + uT * uT - 2 * u0 * uT) + w1 * (u1 * u1 + uT * uT - 2 * u1 * uT);
        int sigma = (w0*w1*((u0-u1)*(u0 - u1))); //Final variance calculation

        //Checks to see if the variance is the largest seen so far, and remembers what threshold produces it
        if (sigma > largest) { 
            largest = sigma; 
            best_threshold = i;
        }
    }
    //Returns the threshold that performed the best (otsu's method)
    return best_threshold;
}

int erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT])
{
    //stop remains 1 when no pixels are eroded
    int stop = 1;
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            if (input_image[x][y] == 255)
            {
                //Edge case
                if (x == BMP_WIDTH || x == 0 || y == BMP_HEIGHT || y == 0) {
                    output_image[x][y] = 0;
                    stop = 0;
                    continue;
                }

                //main case
                if (input_image[x + 1][y] == 0 ||
                    input_image[x - 1][y] == 0 ||
                    input_image[x][y + 1] == 0 ||
                    input_image[x][y - 1] == 0)
                {
                    output_image[x][y] = 0;
                    stop = 0;
                }
                else {
                    output_image[x][y] = 255;
                }
            }
            else
                //Det her er hurtigere end hvis vi bare satte output_image[x][y] = 0
                //Det skylder nok compiler optimization
                output_image[x][y] = input_image[x][y];
        }
    }
    return stop;
}

void copy_bmp(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            output_image[x][y] = input_image[x][y];
        }
    }
}

void draw_red_cross(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], int x, int y)
{

    int length = 14;
    int x_offset = x + 7;
    int y_offset = y + 7;

    for (int i = 0; i < length; i++)
    {
        input_image[x_offset][y + i][0] = 255;
        input_image[x_offset][y + i][1] = 0;
        input_image[x_offset][y + i][2] = 0;
    }

    for (int i = 0; i < length; i++)
    {
        input_image[x + i][y_offset][0] = 255;
        input_image[x + i][y_offset][1] = 0;
        input_image[x + i][y_offset][2] = 0;
    }
}


void deleteCell(unsigned char output_image[BMP_WIDTH][BMP_HEIGHT], int row, int col)

{
    for (int x = row; x < row + 14; x++)
    {
        for (int y = col; y < col + 14; y++)
        {
            output_image[x][y] = 0;
        }
    }
}

int detectCellInstance(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], int row, int col)
{

    // a is used to output analysis of the frame
    // a = 0 means delete
    // a = 1 means increment
    // a = 2 means skip
    #define delete 0
    #define increment 1
    #define skip 2


    if ((input_image[row+13][col] != 0) || (input_image[row+13][col+13] != 0)) 
    { 
        return skip; 
    }
    
    
    //Checks the edge for cells
    for (int x = row; x < row + 14; x++) {
        if (input_image[x][col] != 0 || input_image[x][col+13] != 0) return increment;
    }

    for (int y = col+1; y < col + 13; y++){ 
        if (input_image[row][y] != 0 || input_image[row+13][y] != 0) return increment;
    }

    //Checks the rest of the frame for cells
    for (int x = row+1; x < row + 13; x++)
    {
        for (int y = col+1; y < col + 13; y++)
        {
            if (input_image[x][y] != 0) return delete;
        }
    }

    return skip;
}

int detectCellsIterator(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], short cell_list[MAX_CELLS][2], short * cell_list_length)
{
    int count = 0;

    for (int x = 0; x < BMP_WIDTH - 12; x++)
    {
        for (int y = 0; y < BMP_HEIGHT - 12; y++)
        {
            int DCI = detectCellInstance(input_image, x, y);
            if (DCI == 0)
            {
                deleteCell(input_image, x, y);
                cell_list[*cell_list_length][0] = x;
                cell_list[*cell_list_length][1] = y;
                *cell_list_length += 1;

                count++;
            }
            else if (DCI == 2)
            {
                y = y + 13;
            }
            
        }
    }
    return count;
}

//Unused function
unsigned char* addThirdChannelHeap(unsigned char input_image[BMP_WIDTH][BMP_WIDTH]) {

    unsigned char* output_image = malloc(sizeof(unsigned char) * BMP_WIDTH * BMP_WIDTH * 3);
    if (output_image == NULL) {
        printf("addThirdChannel could not allocate memory for the output image");
        exit(1);
    }

    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            output_image[x + y * BMP_WIDTH] = input_image[x][y];
            output_image[x + y * BMP_WIDTH + 1] = input_image[x][y];
            output_image[x + y * BMP_WIDTH + 2] = input_image[x][y];
        }
    }
    return output_image;
}

void addThirdChannel(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], unsigned char debug_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            debug_image[x][y][0] = input_image[x][y];
            debug_image[x][y][1] = input_image[x][y];
            debug_image[x][y][2] = input_image[x][y];
        }
    }
}