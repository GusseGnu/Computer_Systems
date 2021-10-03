//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"
#include <math.h>
#include <time.h>

unsigned short xCords[500];
unsigned short yCords[500];
unsigned short index = 0;
double histogram[256];

// Calculate total number of pixels
long int total_pixels = BMP_WIDTH * BMP_HEIGTH;

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
  int temp;

  // Initialize histogram array
  for (int i = 0; i < 265; i++)
  {
    histogram[i] = 0.0;
  }

  // Count every entry into histogram
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    { 
      temp = greyscale_image[x][y];
      histogram[temp]++;
    }
  }
}

int otsu_method(unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH], long int total_pixels) {
  double probability[256];
  double sum_lower_bound = 0, sum_upper_bound = 0;
  double omega_lower_bound = 0, omega_upper_bound = 0; 
  double mean_lower_bound = 0, mean_upper_bound = 0;
  double variance = 0.0;
  double MAX_variance = 0.0;
  int threshold = 42;
  
  // Generate Histogram:
  computeHistograms(greyscale_image);

  // Calculates probability of entries in histogram into double values
  for (int i = 0; i < 256; i++){
    probability[i] = ((double)histogram[i] / (double)total_pixels); 
  }

  // Calculates varience
  for (int i = 1; i < 256; i++)
  { 
    printf("i: %d\n", i);
    
    // Calculates probability omega lower bound
    for(int j = 0; j < i; j++)
    {
      omega_lower_bound += probability[j];
    } 
    printf("omega_lower_bound: %f\n", omega_lower_bound);

    // Calculates probability omega upper bound
    for(int k = i; k < 256; k++) 
    {
      omega_upper_bound += probability[k];
    }
    printf("omega_upper_bound: %f\n", omega_upper_bound);

    // Calculate mean of lower bound
    for (int j = 1; j < i; j++)
    {
      sum_lower_bound += histogram[j];
    }
    mean_lower_bound = sum_lower_bound / (i-1);
    printf("mean_lower_bound: %f\n", mean_lower_bound);

    // Calculate mean of upper bound
    for (int k = i; k < 256; k++)
    {
      sum_upper_bound += histogram[k];
    }
    mean_upper_bound = sum_upper_bound / (i-1);
    printf("mean_upper_bound: %f\n", mean_upper_bound);
    

    // Calculates varience
    variance = omega_lower_bound * omega_upper_bound * pow((mean_lower_bound - mean_upper_bound), 2);
    // Checks if current variance is the greatest so far
    if (variance > MAX_variance){
      MAX_variance = variance;
      threshold = i;
    }
    printf("variance: %f\n", variance);
    printf("MAX_variance: %f\n", MAX_variance);  

    // Reset values
    omega_lower_bound = 0;
    omega_upper_bound = 0;
    sum_lower_bound = 0;
    sum_upper_bound = 0;
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
unsigned char erode_image(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char out_image[BMP_WIDTH][BMP_HEIGTH]){
  unsigned char structuring_element[3][3] = {{0,255,0},{255,255,255},{0,255,0}};
  unsigned char eroded = 0;
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
          eroded = 1;
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
  //         eroded = 1;
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
  return eroded;
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
void drawCrosses(unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){

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
  // Execution time analysis starts
  clock_t start, end;
  int cpu_time_used;
  start = clock();

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

  int threshold = otsu_method(greyscale_image, total_pixels);
  printf("threshold: %d\n", threshold);

  //Make image binary
  apply_threshold(greyscale_image, binary_image, 90);

  //Make image to color again  ONLY USED TO CHECK THE BINARY IMAGES / DEBUG
  binary2out(binary_image, output_image);

  write_bitmap(output_image, argv[2]+1);

  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_WIDTH; y++)
    {
      out_image[x][y] = binary_image[x][y];
    }
  }

  unsigned char run = 1;

  while (run == 1)
  {
    run = erode_image(out_image, out_image2);
    for (int x = 0; x < BMP_WIDTH; x++)
    {
      for (int y = 0; y < BMP_WIDTH; y++)
      {
        out_image[x][y] = out_image2[x][y];
      }
    } 
    detect_cells(out_image);
  }

  printf("Number of cells detected: %u\n", index);

  printf("Otsu threshold value: %d\n", threshold);

  //Make image to color again  ONLY USED TO CHECK THE BINARY IMAGES / DEBUG
  //binary2out(out_image, output_image);

  drawCrosses(input_image);

  //Save image to file
  write_bitmap(input_image, argv[2]);

  printf("Done!\n");

  // Execution time analysis stop
  end = clock();
  cpu_time_used = (int) (end - start);
  printf("Total time: %u ms\n", cpu_time_used * 1000 / CLOCKS_PER_SEC);
  
  return 0;
}
