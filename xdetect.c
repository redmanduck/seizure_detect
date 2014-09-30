/*
 *   BME301 Group 1 - S. Sabpisal, P. Marshall, C. Kuntz, H. Bulava, Paolo L.
 */

#include <stdio.h>
#define WINDOW_SIZE 512
#define THRES 80
#define MAX_SWITCH 40
#define BASELINE_MEAN 117

void print_r(char * bf, int blen);
int detect_seizure(unsigned char * bf);
int switch_count(char * window, int winsize , int offset, int first);
double xrms(unsigned char * M, int length);

int main(int argc, unsigned char *argv[]){
  FILE * fp = fopen(argv[1], "rb");
  FILE * fo = fopen("output.csv", "w");

  if(fp == NULL || fo == NULL){
     printf("Unable to open file\nUsage: xdetect TrainingData.bin\n");
     return 1;
  }
    
  unsigned char buffer[WINDOW_SIZE]; //use unsigned char because we want only 1 byte
  int bytes_read = 0;
  int n = 1;
  do{
     bytes_read = fread(buffer, 1, WINDOW_SIZE, fp); 
     fprintf(fo, "%d,", detect_seizure(buffer));     
     fseek(fp,(n++)*WINDOW_SIZE, SEEK_SET); //move to the next packet
  }while(bytes_read > 0);

  fclose(fp);
  fclose(fo);
}

/*
 *  Magnitude Approximation
 */

double xrms(unsigned char * M, int length){
	int i = 0, sum = 0;
	for(i = 0; i < length; i++) sum += (int)(M[i]*M[i]); 
	return (double)sum/length; // ;/length
}

/*
 *  Frequency Approximation 
 */

int switch_count(char *M, int winsize, int accum, int j){
  if(winsize == 0) return accum;
  return switch_count(M, winsize - 1, accum + ((M[j] >0 && M[j+1] <0)||( M[j] <0&&M[j+1]>0)||M[j]==0), j + 1); 
}

/*
 *  Seizure Detection
 */ 
int detect_seizure(unsigned char * onewindow){
   int i = 0;
   char reduced[WINDOW_SIZE]; 
   for(i = 0; i < WINDOW_SIZE; i++) reduced[i] = onewindow[i] - BASELINE_MEAN;
   return (xrms(reduced , WINDOW_SIZE) < (double)THRES || switch_count(reduced, WINDOW_SIZE, 0, 0) > MAX_SWITCH);     //TODO: check that THRESHOLD, it doesn't do anything
}	