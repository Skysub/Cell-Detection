#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"
#include "sun.h"

// int structuring_element[3][3] = {{0, 1, 0}, {1, 1, 1}, {0, 1, 0}};

// This function converts the 3 dimension images to a 2 dimension image in gray scale. It goes through every pixel and calculate the averages value from the RGB values.
void convert_to_gray(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char buff_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS])
{
    for (int x = 0; x <= BMP_WIDTH; x++)
    {
        for (int y = 0; y <= BMP_HEIGHT; y++)
        {
            //Bitshifting to divide by 4. Threshold is changed to account
            buff_image[x][y][0] = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) >> 2;
        }
    }
}

// This function converts the image to a binary with only white and black pixels. The threshold is a varible input.
void convert_to_binary_image(int threshold, unsigned char buff_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS])
{
    for (int x = 0; x <= BMP_WIDTH; x++)
    {
        for (int y = 0; y <= BMP_HEIGHT; y++)
        {
            if (buff_image[x][y][0] <= threshold)
                (buff_image[x][y][0] = 0);
            else
                (buff_image[x][y][0] = 255);
        }
    }
}

int erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS])
{
    //stop remains 1 when no pixels are eroded
    int stop = 1;
    for (int x = 0; x <= BMP_WIDTH; x++)
    {
        for (int y = 0; y <= BMP_HEIGHT; y++)
        {
            if (input_image[x][y][0] == 255)
            {
                if (input_image[x + 1][y][0] == 0 || input_image[x - 1][y][0] == 0 || input_image[x][y + 1][0] == 0 || input_image[x][y - 1][0] == 0)
                {
                    output_image[x][y][0] = 0;
                    stop = 0;
                }
            }
        }
    }
    return stop;
}

void copy_bmp(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS])
{
    for (int c = 0; c <= 3; c++)
    {
        for (int x = 0; x <= BMP_WIDTH; x++)
        {
            for (int y = 0; y <= BMP_HEIGHT; y++)
            {
                output_image[x][y][0] = input_image[x][y][0];
            }
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

void deleteCell(unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], int row, int col)
{
    for (int x = row; x < row + 14; x++)
    {
        for (int y = col; y < col + 14; y++)
        {
            output_image[x][y][0] = 0;
        }
    }
}

int detectCellInstance(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], int row, int col)
{

    // a is used to output analysis of the frame
    // a = 0 means delete
    // a = 1 means increment
    // a = 2 means skip
    #define delete 0
    #define increment 1
    #define skip 2
    
    char a = skip;

    for (int x = row; x < row + 14; x++)
    {
        if (a == increment)
        {
            break;
        }
        for (int y = col; y < col + 14; y++)
        {
            if (x == row || x == row + 13 || y == col || y == col + 13)
            {
                if (input_image[x][y][0] != 0)
                {
                    //Cell detected in edge. Frame of analysis to be incremented forward
                    a = increment;
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                //if cell has been detected inside the frame, then further analysis of the inside is nolonger needed
                if (a == delete)
                {
                    continue;
                }
                else if (input_image[x][y][0] != 0)
                {
                    a = delete;
                }
            }
        }
    }

    return a;
}

int detectCellsIterator(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS])
{
    int count = 0;
    for (int x = 0; x < BMP_WIDTH - 13; x++)
    {
        for (int y = 0; y < BMP_HEIGHT - 13; y++)
        {
            int DCI = detectCellInstance(input_image, x, y);
            if (DCI == 0)
            {
                deleteCell(input_image, x, y);
                draw_red_cross(output_image, x, y);
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