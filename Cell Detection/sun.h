#include "cbmp.h"

void convert_to_gray(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT]);
void convert_to_binary_image(int threshold, unsigned char buff_image[BMP_WIDTH][BMP_HEIGHT]);
int erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT]);
void copy_bmp(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT]);
void draw_red_cross(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], int x, int y);
int detectCellsIterator(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]);