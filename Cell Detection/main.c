//To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
//To run (linux/mac): ./main.out example.bmp example_inv.bmp

//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

//Function to convert RGB picture to grayscale with arbitrary threshold
void toGrayScale(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][1]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {

      int v = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) / 3;

      if (v > 100) {
        output_image[x][y][0] = 255;
        output_image[x][y][0] = 255;
        output_image[x][y][0] = 255;
      } else {
        output_image[x][y][0] = 0;
        output_image[x][y][0] = 0;
        output_image[x][y][0] = 0;
      }
    }
  }
}

int delCount = 0; 
void deleteCell(unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][1],int row, int col){
  delCount = delCount +1;
  for (int x = row; x < row+14; x++)
  {
    for (int y = col; y < col+14; y++)
    {
      output_image[x][y][0] = 0;
    }
  }

}

int detectCellInstance(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][1],int row, int col){
  char delete = 1;
  char detection = 0;
  
  for (int x = row; x < row+14; x++)
  {
    if (delete == 0) {break;}
    for (int y = col; y < col+14; y++)
    {
      if (x == row || x == row+13 || y==col || y==col+13)
      {
        if (input_image[x][y][0] != 0) {
          delete = 0;
          break;
        } else {
          continue;
        }

      } else 
      { 
        if (detection == 1) {continue;}
        else if (input_image[x][y][0] != 0) {detection = 1;}
      }
    }
  }

  if (detection == 0) {delete = 0;}  

  return delete;
}

void detectCellsIterator(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][1]){
   for (int x = 0; x < BMP_WIDTH-13; x++)
  {
    for (int y = 0; y < BMP_HEIGTH-13; y++)
    {
      if (detectCellInstance(input_image, x, y) == 1)
      {
        deleteCell(input_image, x, y);
      }
    }
  }
}
//Function to convert grayscale picture to RGB 
void toRGB(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][1], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      output_image[x][y][0] = input_image[x][y][0];
      output_image[x][y][1] = input_image[x][y][0];
      output_image[x][y][2] = input_image[x][y][0];
    }
  }
}

//Function to erode grayscale picture 
void erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][1], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][1]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      if (input_image[x+1][y][0] == 0 || input_image[x][y+1][0] == 0 || input_image[x-1][y][0] == 0 || input_image[x][y-1][0] == 0) 
      {
        output_image[x][y][0] = 0;
      } else
      {
        output_image[x][y][0] = 255;
      }
    }
  }
}

//Function to invert pixels of an image (negative)
void invert(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      for (int c = 0; c < BMP_CHANNELS; c++)
      {
      output_image[x][y][c] = 255 - input_image[x][y][c];
      }
    }
  }
}

  //Declaring the array to store the image (unsigned char = unsigned 8 bit)
  //GS(Grayscale) for only 1 color dimension | RGB for 3 color dimensions
  unsigned char RGB_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
  unsigned char GS_image[BMP_WIDTH][BMP_HEIGTH][1];
  unsigned char GS_image2[BMP_WIDTH][BMP_HEIGTH][1];

//Main function
int main(int argc, char** argv)
{
  //argc counts how may arguments are passed
  //argv[0] is a string with the name of the program
  //argv[1] is the first command line argument (input image)
  //argv[2] is the second command line argument (output image)

  //Checking that 2 arguments are passed
  if (argc != 3)
  {
      fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
      exit(1);
  }

  printf("Example program - 02132 - A1\n");

  //Load image from file
  read_bitmap(argv[1], RGB_image);

  toGrayScale(RGB_image, GS_image);

  erode(GS_image, GS_image2);
  detectCellsIterator(GS_image2);
      
  erode(GS_image2, GS_image);
  detectCellsIterator(GS_image);
      
  erode(GS_image, GS_image2);
  detectCellsIterator(GS_image2);
      
  erode(GS_image2, GS_image);
  detectCellsIterator(GS_image);
  
  erode(GS_image, GS_image2);
  detectCellsIterator(GS_image2);
      
  erode(GS_image2, GS_image);
  detectCellsIterator(GS_image);
  
  erode(GS_image,GS_image2);
  detectCellsIterator(GS_image2);

  erode(GS_image2,GS_image);
  detectCellsIterator(GS_image);

  erode(GS_image,GS_image2);
  detectCellsIterator(GS_image2);

  toRGB(GS_image2,RGB_image);

  //Save image to file
  write_bitmap(RGB_image, argv[2]);
  printf("delCount = %d", delCount);
  printf("Done!\n");
  return 0;
}
