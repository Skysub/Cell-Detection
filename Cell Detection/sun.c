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
            buff_image[x][y][0] = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) / 3;
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

int delCount = 0;
void deleteCell(unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], int row, int col)
{
    delCount = delCount + 1;
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
    char delete = 1;
    char detection = 0;

    for (int x = row; x < row + 14; x++)
    {
        if (delete == 0)
        {
            break;
        }
        for (int y = col; y < col + 14; y++)
        {
            if (x == row || x == row + 13 || y == col || y == col + 13)
            {
                if (input_image[x][y][0] != 0)
                {
                    delete = 0;
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                if (detection == 1)
                {
                    continue;
                }
                else if (input_image[x][y][0] != 0)
                {
                    detection = 1;
                }
            }
        }
    }

    if (detection == 0)
    {
        delete = 0;
    }

    return delete;
}

int detectCellsIterator(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS])
{
    int count = 0;
    for (int x = 0; x < BMP_WIDTH - 13; x++)
    {
        for (int y = 0; y < BMP_HEIGHT - 13; y++)
        {
            if (detectCellInstance(input_image, x, y) == 1)
            {
                deleteCell(input_image, x, y);
                draw_red_cross(output_image, x, y);
                count++;
            }
        }
    }
    return count;
}