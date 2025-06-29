
#ifndef IJVM_STRUCT_H
#define IJVM_STRUCT_H

#include <stdio.h>  /* contains type FILE * */

#include "ijvm_types.h"
/**
 * All the state of your IJVM machine goes in this struct!
 **/

typedef int32_t word;
typedef uint8_t byte;
typedef struct IJVM {
    // do not changes these two variables
    FILE *in;   // use fgetc(ijvm->in) to get a character from in.
                // This will return EOF if no char is available.
    FILE *out;  // use for example fprintf(ijvm->out, "%c", value); to print value to out

  // your variables go here
  
  

  //chapter 1
  byte *text; // pointer to the program text (bytecode)
  unsigned int text_size; // num of bytes in text

  word *constant_pool; // pointer to constant pool (array of 32-bit values)
  unsigned int constant_pool_count; // num of 32-bit vals (constants)

  //chapter 2
  word *stack;
  unsigned int stack_size;
  unsigned int stack_max;

  unsigned int program_counter;
  
  bool done;

  //chapter 4 + 5

  word* locals;
  unsigned int lv; // top used index + 1
  unsigned int lv_max; // capacity
  unsigned int new_lv;
  unsigned int local_count; // number of local variables
  unsigned int arg_count; // number of arguments

  word* control_data;
  unsigned int control_size; // top used index + 1
  unsigned int control_max; // capacity
  
} ijvm;

#endif 
