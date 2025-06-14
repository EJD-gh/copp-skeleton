#include <stdio.h>  // for getc, printf
#include <stdlib.h> // malloc, free
#include "ijvm.h"
#include "util.h" // read this file for debug prints, endianness helper functions


// see ijvm.h for descriptions of the below functions

ijvm* init_ijvm(char *binary_path, FILE* input, FILE* output)
{
  // do not change these first three lines
  ijvm* m = (ijvm *) malloc(sizeof(ijvm));
  // note that malloc gives you memory, but gives no guarantees on the initial
  // values of that memory. It might be all zeroes, or be random data.
  // It is hence important that you initialize all variables in the ijvm
  // struct and do not assume these are set to zero.
  m->in = input;
  m->out = output;
  
  FILE *file = fopen(binary_path, "rb");
  if (file == NULL) { // test to see if file was opened / read
    free(m);
    return NULL;
  }

  uint8_t buf[4]; // 8 bit unsigned int for each byte, array of 4 bytes

  fread(buf, 1, 4, file); // put first 4 bytes of the file into the buffer
  uint32_t magicNum = read_uint32(buf);
  if (magicNum != MAGIC_NUMBER) {
    fprintf(stderr, "Invalid magic num\n");
    fclose(file);
    free(m);
    return NULL;
  }

  fread(buf, 1, 4, file); // read constant pool orgigin into buffer from file


  fread(buf, 1, 4, file); // read 4byte constant pool size into buffer
  uint32_t constant_pool_size = read_int32(buf);
  m->constant_pool = (word *)malloc(constant_pool_size); // allocate space for constant_pool
  m->constant_pool_count = constant_pool_size / 4; // divide by 4 to yield words instead of bytes
  

  for (unsigned int i = 0 ; i < m->constant_pool_count; i++){
    fread(buf, 1, 4, file);
    m->constant_pool[i] = read_int32(buf);
  }

  fread(buf, 1, 4, file); // read text origin into buffer

  fread(buf, 1, 4, file); // read text size into buffer
  m->text_size = read_int32(buf);
  m->text = (byte *)malloc(m->text_size);

  fread(m->text, 1, m->text_size, file); // read (text_size) bytes of the text into "m->text"

  
  // TODO: implement me

  return m;
}

void destroy_ijvm(ijvm* m) 
{
  // TODO: implement me
  free(m->constant_pool);
  free(m->text);
  free(m); // free memory for struct
}

byte *get_text(ijvm* m) 
{
  // TODO: implement me
  return m->text; // return pointer to text
}

unsigned int get_text_size(ijvm* m) 
{
  // TODO: implement me
  return m->text_size; // return size of text (bytes)
}

word get_constant(ijvm* m,int i) 
{
  // TODO: implement me
  if (i < 0 || i >= m->constant_pool_count){
    fprintf(stderr, "Invalid constant index: %d\n", i);
    exit(1);
  }
  return m->constant_pool[i];
}

unsigned int get_program_counter(ijvm* m) 
{
  // TODO: implement me
  return 0;
}

word tos(ijvm* m) 
{
  // this operation should NOT pop (remove top element from stack)
  // TODO: implement me
  return -1;
}

bool finished(ijvm* m) 
{
  // TODO: implement me
  return false;
}

word get_local_variable(ijvm* m, int i) 
{
  // TODO: implement me
  return 0;
}

void step(ijvm* m) 
{
  // TODO: implement me

}

byte get_instruction(ijvm* m) 
{ 
  return get_text(m)[get_program_counter(m)]; 
}

ijvm* init_ijvm_std(char *binary_path) 
{
  return init_ijvm(binary_path, stdin, stdout);
}

void run(ijvm* m) 
{
  while (!finished(m)) 
  {
    step(m);
  }
}


// Below: methods needed by bonus assignments, see ijvm.h
// You can leave these unimplemented if you are not doing these bonus 
// assignments.

int get_call_stack_size(ijvm* m) 
{
   // TODO: implement me if doing tail call bonus
   return 0;
}


// Checks if reference is a freed heap array. Note that this assumes that 
// 
bool is_heap_freed(ijvm* m, word reference) 
{
   // TODO: implement me if doing garbage collection bonus
   return 0;
}

// Checks if top of stack is a reference
bool is_tos_reference(ijvm* m)
{
	// TODO: implement me if doing precise garbage collection bonus
	//  using ANEWARRAY, AIALOAD and AIASTORE
	return 0;
}
