#include <stdio.h>
#define WINDOW_SIZE 512
#define THRES 40000
#define MAX_SWITCH 28
#define BASELINE_MEAN 116

void print_r(char * bf, int blen);
int detect_seizure(unsigned char * bf);
int switch_count(char * window, int winsize);
double xrms(unsigned char * M, int len);

int main(int argc, unsigned char *argv[]){
  printf("Opening %s..\n" , argv[1]);
  FILE * fp = fopen(argv[1], "rb");
  FILE * fo = fopen("output.csv", "w");
  if(fp == NULL){
     printf("Unable to open file\n\n");
     return 1;
  }
    
  unsigned char buffer[WINDOW_SIZE]; //use unsigned char because we want only 1 byte
  int bytes_read = 0;
  int total_bytes_read = 0;
  int n = 1;
  do{
     //read WINDOW_SIZE bytes and detect seizure then throw 
     //the data away 
     bytes_read = fread(buffer, 1, WINDOW_SIZE, fp); 
     //print_r(buffer, WINDOW_SIZE);

     int result = detect_seizure(buffer);
     fprintf(fo, "%d,", result);     
     //printf("===== Window # %d =====\n", n);
     //print_r(buffer, WINDOW_SIZE);
     total_bytes_read += bytes_read;
     fseek(fp,(n++)*WINDOW_SIZE, SEEK_SET); //move to the next packet
  }while(bytes_read > 0);


  printf("Total Bytes: %d", total_bytes_read);
  fclose(fp);
  fclose(fo);
}

double xrms(unsigned char * M, int length){
  int i = 0;
  int sum = 0;
  for(i = 0; i < length; i++){
    int point = M[i];
    printf("%d (0x%x),  ", point, point);
    sum += point;
    printf("<-[%d]  ", sum);

  }
  printf("SUM=%d",sum); //got sum/len = ~23 in matlab
  return (double)sum/length; // ;/length
}

int switch_count(char * M, int winsize){
   int k = 0;
   int j =0;
   for(j = 0; j < winsize; j++){
     if(M[j] > 0 && M[j+1] < 0){
        k++;
     }else if(M[j] < 0 && M[j+1] > 0){
        k++;
     }else if(M[j] == 0){
        k++;
     }
   }
   return k;
}
/*
 *
 *  seizure detection
 */
int detect_seizure(unsigned char * onewindow){
   int i = 0;
   char reduced[WINDOW_SIZE]; 
   unsigned char squared[WINDOW_SIZE];
   for(i = 0; i < WINDOW_SIZE; i++){
     reduced[i] = onewindow[i] - BASELINE_MEAN;
     squared[i] = reduced[i] * reduced[i];
   }
  // print_r(squared, WINDOW_SIZE); 
   int num_switch = switch_count(reduced, WINDOW_SIZE);
   double xr = xrms(squared, WINDOW_SIZE);
   printf("rms: %.3f (%d), num_switch: %d (%d)\n", xr, THRES/WINDOW_SIZE, num_switch, MAX_SWITCH);
   if( xr < THRES/WINDOW_SIZE || num_switch > MAX_SWITCH){ // || num_switch > MAX_SWITCH
      return 0;
   }
   printf("Seizure found");
   return 1;
}

/*
 *
 * function for debugging
 * 
 * */
void print_r(char *buffer, int blen){
  int i = 0;
  for(i = 0; i< blen; i++){
    printf("(%d,%x,%d), ", i,buffer[i],buffer[i] );
  }
}
