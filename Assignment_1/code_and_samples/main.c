//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"
#include <math.h>

unsigned short xCords[500];
unsigned short yCords[500];
unsigned short index = 0;
double histogram[256];
//float probabilities[256];

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

// double averageGrey(unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH]){
//   int greyval;

//   for (int x = 0; x < BMP_WIDTH; x++)
//   {
//     for (int y = 0; y < BMP_HEIGTH; y++)
//     {
//       greyval += (int) greyscale_image[x][y];
//     }
//   }
//   return (double) greyval;
// }

void computeHistograms(unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH]){

  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      histogram[greyscale_image[x][y]] += 1;
    }
  }
  
  // for (int z = 0; z < 256; z++)
  // {
  //   probabilities[z] = histograms[z] / (BMP_WIDTH * BMP_HEIGTH);
  // }
}

int otsu_method(long int total_pixels) {
  double probability[256], mean[256];
  double max_between, between[256];
  int threshold;

  /*
  probability = class probability
  mean = class mean
  between = between class variance
  */

  for(int i = 0; i < 256; i++) {
      probability[i] = 0.0;
      mean[i] = 0.0;
      between[i] = 0.0;
  }

  probability[0] = histogram[0];

  for(int i = 1; i < 256; i++) {
      probability[i] = probability[i - 1] + histogram[i];
      mean[i] = mean[i - 1] + i * histogram[i];
  }

  threshold = 0;
  max_between = 0.0;

  for(int i = 0; i < 255; i++) {
    if(probability[i] != 0.0 && probability[i] != 1.0)
    {
      between[i] = pow(mean[255] * probability[i] - mean[i], 2) / (probability[i] * (1.0 - probability[i]));
    }
    else
    {
      //between[i] = 0.0;
      if(between[i] > max_between) {
        max_between = between[i];
        threshold = i;
      }
    }
  }
  return threshold;
}

// Function to convert the image to a binary image by applying a threshold to either be white or black
void apply_threshold(unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH], unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], int threshold){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      if (greyscale_image[x][y] < threshold)
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

  // Sets pixel to black if it has no white neighbors up, down, right or left
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
      else
      {
        out_image[x][y] = 0;
      }
    }
  }

  // // Sets pixel to black if it only has black neighbors up, down, right or left
  // for (int x = 1; x < BMP_WIDTH-1; x++)
  // {
  //   for (int y = 1; y < BMP_HEIGTH-1; y++)
  //   {
  //     if (out_image[x][y] == 255)
  //     {
  //       if (out_image[x-1][y] == 0 && out_image[x][y-1] == 0 && out_image[x+1][y] == 0 && out_image[x][y+1] == 0)
  //       {
  //         out_image[x][y] = 0;
  //       }
  //       else
  //       {
  //         out_image[x][y] = 255;
  //       }
  //     }
  //     else
  //     {
  //       out_image[x][y] = 0;
  //     }
  //   }
  // }
}

void detect_cells(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH]){
  unsigned char exclusion = 0;
  unsigned char cellDetected = 0;

  for (int x = 0; x < BMP_WIDTH - 13; x++){
    for (int y = 0; y < BMP_HEIGTH - 13; y++){

      // Exclusion zone check
      for (int i = 0; i <= 13; i++){
        if (binary_image[x + i][y] == 255){
          exclusion = 1;
        }
      }
      for (int i = 0; i <= 13; i++){
        if (binary_image[x + i][y + 13] == 255){
          exclusion = 1;
        }
      }
      for (int j = 1; j < 13; j++){
        if (binary_image[x][y + j] == 255){
          exclusion = 1;
        }
      }
      for (int j = 1; j < 13; j++){
        if (binary_image[x + 13][y + j] == 255){
          exclusion = 1;
        }
      }

      if (exclusion == 0){
        for (int i = x + 1; i < x + 13; i ++){
          for (int j = y + 1; j < y + 13; j++){
            // If cell is detected inside the inclusion zone 
            if(cellDetected == 1){
              binary_image[i][j] = 0;
            }
            
            // Searches for cell inside inclusion zone
            if(binary_image[i][j] == 255){
              cellDetected = 1;
              xCords[index] = x + 6;
              yCords[index] = y + 6;
              index++;
              binary_image[i][j] = 0;
            }  
          }
        }
        cellDetected = 0;
      }
      exclusion = 0;
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

// Function to draw red crosses on detected cells
void drawCross(unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){

  for (int i = 0; i < index; i++)
  {
    for (int j = xCords[i]-10; j <= xCords[i]+10; j++)
    {
      for (int k = yCords[i]-1; k <= yCords[i]+1; k++)
      {
        if (j >= 0 && j < 950 && k >= 0 && k < 950)
        {
          output_image[j][k][0] = 255;
          output_image[j][k][1] = 0;
          output_image[j][k][2] = 0;
        }
      }
    }
   for (int j = xCords[i]-1; j <= xCords[i]+1; j++)
    {
      for (int k = yCords[i]-10; k <= yCords[i]+10; k++)
      {
        if (j >= 0 && j < 950 && k >= 0 && k < 950)
        {
          output_image[j][k][0] = 255;
          output_image[j][k][1] = 0;
          output_image[j][k][2] = 0;
        }
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

  int threshold = otsu_method((BMP_WIDTH * BMP_HEIGTH));

  //Make image binary
  apply_threshold(greyscale_image, binary_image, threshold);

  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_WIDTH; y++)
    {
      out_image[x][y] = binary_image[x][y];
    }
  }

  for (int i = 0; i < 12; i++)
  {
    binary2out(out_image, output_image);
    write_bitmap(output_image, argv[2]);
    erode_image(out_image, out_image2);
    for (int x = 0; x < BMP_WIDTH; x++)
    {
      for (int y = 0; y < BMP_WIDTH; y++)
      {
        out_image[x][y] = out_image2[x][y];
      }
    } 
    detect_cells(out_image);
  }

  printf("%s", "Number of cells detected: ");
  printf("%u", index);
  printf("%s", "\n");

  printf("%s", "Otsu threshold value: ");
  printf("%u", threshold);
  printf("%s", "\n");

  //Make image to color again  ONLY USED TO CHECK THE BINARY IMAGES / DEBUG
  //binary2out(out_image, output_image);

  drawCross(input_image);

  //Save image to file
  write_bitmap(input_image, argv[2]);

  printf("Done!\n");
  return 0;
}
