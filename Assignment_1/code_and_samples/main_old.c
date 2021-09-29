//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

unsigned short xCords[500];
unsigned short yCords[500];
unsigned short index = 0;

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

// Function to convert the RGB image to greyscale by computing the average value from the 3 channels
void rgb2grey(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      for (int c = 0; c < BMP_CHANNELS; c++)
      {
      greyscale_image[x][y] += (input_image[x][y][c]/3);
      }
    }
  }
}

// Function to convert the image to a binary image by applying a threshold to either be white or black
void apply_threshold(unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH], unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      if (greyscale_image[x][y] < 90)
      {
        binary_image[x][y] = 0;
      }
      else
      {
        binary_image[x][y] = 255;
      }
    }
  }
}

//Function to erode the image by examining each pixel and its neighbors and determining if theyre white
void erode_image(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char out_image[BMP_WIDTH][BMP_HEIGTH]){
  unsigned char structuring_element[3][3] = {{0,255,0},{255,255,255},{0,255,0}};

  // Need a special case for the edges, where just the inner elements are checked? 

  // Need a way to use the structuring element for checking, perhaps using array slices

  for (int x = 1; x < BMP_WIDTH-1; x++)
  {
    for (int y = 1; y < BMP_HEIGTH-1; y++)
    {
      if (binary_image[x][y] == 255)
      {
        if (binary_image[x-1][y] != 255 || binary_image[x][y-1] != 255 || binary_image[x+1][y] != 255 || binary_image[x][y+1] != 255)
        {
          out_image[x][y] = 0;
        }
        else
        {
          out_image[x][y] = 255;
        }
      }
    }
  }
}

void detect(unsigned char in_image[BMP_WIDTH][BMP_HEIGTH], unsigned char out_image[BMP_WIDTH][BMP_HEIGTH]){




}
















void detect_cells(unsigned char out_image2[BMP_WIDTH][BMP_HEIGTH], unsigned char out_image[BMP_WIDTH][BMP_HEIGTH]){
  int exclusion = 0;
  int cellDetect = 0;
  unsigned short xCord;
  unsigned short yCord;
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      for (int z = x; z < (x+14); z++)
      {
        for (int w = y; w < (y+14); w++)
        {
          if (out_image2[z][w] == 255)
          {
            xCord = z;
            yCord = w;
            if ((z == x) || (z == x+13) || (w == y) || (w == y+13))
            {
              exclusion = 1;
            }
            else
            {
              cellDetect = 1;
            }
          }
        }
      }
      if ((exclusion == 0) && (cellDetect == 1))
      {
        xCords[index] = xCord;
        yCords[index] = yCord;
        index += 1;
        cellDetect = 0;
        for (int z = x+1; z < 12; z++)
        {
          for (int w = y+1; w < 12; w++)
          {
            out_image[z][w] = 0;
            out_image2[z][w] = 0;
          }
        }
      }
      else 
      {
        exclusion = 0;
        cellDetect = 0;
      }
    }
  }
}

// DEBUG FUNCTION TO CHECK BINARY IMAGES
void binary2out(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      for (int c = 0; c < BMP_CHANNELS; c++)
      {
      output_image[x][y][c] = binary_image[x][y];
      }
    }
  }
}

  //Declaring the arrays to store the image (unsigned char = unsigned 8 bit)
  unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
  unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
  unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH];
  unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH];
  unsigned char out_image[BMP_WIDTH][BMP_HEIGTH];
  unsigned char out_image2[BMP_WIDTH][BMP_HEIGTH];

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
  read_bitmap(argv[1], input_image);

  //Run inversion
  //invert(input_image,output_image);

  //Make image greyscale
  rgb2grey(input_image, greyscale_image);

  //Make image binary
  apply_threshold(greyscale_image, binary_image);

  erode_image(binary_image, out_image);

  for (int i = 0; i < 24; i++)
  {
    if ((i % 2) == 0)
    {
      erode_image(out_image, out_image2);
      for (int x = 0; x < BMP_WIDTH; x++)
      {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
          out_image[x][y] = out_image2[x][y];
        }
      }
      detect_cells(out_image, out_image2);
    }
    else
    {
      erode_image(out_image2, out_image);
      for (int x = 0; x < BMP_WIDTH; x++)
      {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
          out_image2[x][y] = out_image[x][y];
        }
      }
      detect_cells(out_image2, out_image);
    }
  }
  //erode_image(out_image2, out_image);  //DOESNT WORK BECAUSE out_image2 is never updated and therefore cannot be correctly used as input for erode

  // erode_image(binary_image, out_image);
  // for (int x = 0; x < BMP_WIDTH; x++)
  // {
  //   for (int y = 0; y < BMP_HEIGTH; y++)
  //   {
  //     out_image2[x][y] = out_image[x][y];
  //   }
  // }
  // erode_image(out_image, out_image2);
  // for (int x = 0; x < BMP_WIDTH; x++)
  // {
  //   for (int y = 0; y < BMP_HEIGTH; y++)
  //   {
  //     out_image[x][y] = out_image2[x][y];
  //   }
  // }
  // erode_image(out_image2, out_image);

  //detect_cells(out_image, out_image2);

  printf("%s", "Number of cells detected: ");
  printf("%u", index);
  printf("%s", " \n");

  for (int i = 0; i < index; i++)
  {
    printf("%s", "xCord: ");
    printf("%u", xCords[i]);
    printf("%s", "\n");
    printf("%s", "yCord: ");
    printf("%u", yCords[i]);
    printf("%s", "\n");
  }

  printf("%s", "Number of cells detected: ");
  printf("%u", index);
  printf("%s", " \n");

  //Make image to color again  ONLY USED TO CHECK THE BINARY IMAGES / DEBUG
  binary2out(out_image, output_image);

  //Save image to file
  write_bitmap(output_image, argv[2]);

  printf("Done!\n");
  return 0;
}
