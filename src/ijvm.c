#include <stdio.h>  // for getc, printf
#include <stdlib.h> // malloc, free
#include "ijvm.h"
#include "util.h" // read this file for debug prints, endianness helper functions

#define STACK_CAPACITY 64 // max stack size (initally)
//#define EOF -1 // defining end of file for OP_IN case where there is no input (will output 0 in this case)

// see ijvm.h for descriptions of the below functions

void push (ijvm* m, word val){
  if (m->stack_size >= m->stack_max) {
    m->stack_max *= 2;
    m->stack = realloc(m->stack, m->stack_max * sizeof(word));
  }
  m->stack[m->stack_size] = val; // set val to memory location of top of the stack
  m->stack_size++;
}

word pop(ijvm *m){
  if(m->stack_size == 0){
    fprintf(stderr, "Stack uderflow!");
    exit(1);
  }
  m->stack_size--;
  return m->stack[m->stack_size]; // return the stack_top + 1 value 
}

word top(ijvm *m) {
  if(m->stack_size == 0){
    fprintf(stderr, "Stack empty.");
    exit(1);
  }
  return m->stack[m->stack_size - 1];
}

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


  // chapter 1 
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
  m->constant_pool_count = constant_pool_size / 4; // divided by 4 to yield words instead of bytes

  for (unsigned int i = 0 ; i < m->constant_pool_count; i++){
    fread(buf, 1, 4, file);
    m->constant_pool[i] = read_int32(buf);
  }

  fread(buf, 1, 4, file); // read text origin into buffer

  fread(buf, 1, 4, file); // read text size into buffer
  m->text_size = read_int32(buf);
  m->text = (byte *)malloc(m->text_size);

  fread(m->text, 1, m->text_size, file); // read (text_size) bytes of the text into "m->text"

 
  // chatper 2 stuff
  m->stack_max = STACK_CAPACITY;
  m->stack_size = 0;
  m->stack = malloc(m->stack_max * sizeof(word));
  m->program_counter = 0;
  m->done = false;



  //chapter 4 things
  m->lv_size = 256;
  m->locals = calloc(m->lv_size, sizeof(word));
  m->lv = 0;

  return m;
}

void destroy_ijvm(ijvm* m) 
{
  free(m->constant_pool);
  free(m->text);
  free(m->stack);
  free(m->locals);
  free(m); // free memory for struct
}

byte *get_text(ijvm* m) 
{
  return m->text; // return pointer to text
}

unsigned int get_text_size(ijvm* m) 
{
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
  return m->program_counter;
}

word tos(ijvm* m) 
{
  if (m->stack_size == 0) {
    fprintf(stderr, "Stack is empty.\n");
    exit(1);
  }
  return m->stack[m->stack_size - 1];
}

bool finished(ijvm* m) 
{
  if (m->program_counter >= m->text_size) {
    m->done = true;
  }
  return m->done;
}

word get_local_variable(ijvm* m, int i) 
{
  if (i < 0 || m->lv + 1 >= m->lv_size) {
    fprintf(stderr, "local variable not found.");
    exit(1);
  }
  return m->locals[m->lv + i];
}

void step(ijvm* m) {
  
  byte instruction = m->text[m->program_counter];
  m->program_counter++;
  // TODO: implement me
  switch (instruction) {
    case OP_BIPUSH: {
      int8_t byteVal = (int8_t) m->text[m->program_counter];
      m->program_counter++;
      push(m, byteVal);
    }
    break;
    case OP_DUP: {
      word topVal = top(m);
      push(m, topVal);
    }
    break;
    case OP_IADD: {
      word b = pop(m);
      word a = pop(m);
      word sum = a + b;
      push(m, sum);
    }
    break;
    case OP_IAND: {
      word b = pop(m);
      word a = pop(m);
      word bitwiseAnd = a & b;
      push(m, bitwiseAnd);
    }
    break;
    case OP_IOR: {
      word b = pop(m);
      word a = pop(m);
      word bitwiseOr = a | b;
      push(m, bitwiseOr);
    }
    break;
    case OP_ISUB: {
      word b = pop(m);
      word a = pop(m);
      word subVal = a - b;
      push(m, subVal);
    }
    break;
    case OP_NOP: break;
    case OP_POP: {
      pop(m);
    }
    break;
    case OP_SWAP: {
      word a = pop(m);
      word b = pop(m);
      push(m, a);
      push(m, b);
    }
    break;
    case OP_ERR: {
      fprintf(m->out, "!!!Error!!!\n");
      m->done = true;
    }
    break;
    case OP_HALT: {
      m->done = true;
    }
    break;
    case OP_IN: {
      word character = fgetc(m->in);

      if(character == 0){
        push(m, 0);
      }
      else {
        push(m, character);
      }
      }
      break;
      case OP_OUT: {
        word outputValue = pop(m);
        fprintf(m->out, "%c", outputValue);
      }
      break;
      

      // chapter 3 cases
      case OP_GOTO: {
        int16_t jumpVal = (int16_t)((m->text[m->program_counter] << 8) + m->text[m->program_counter + 1]);
        m->program_counter += 2; // move program counter val past location bytes
        m->program_counter += jumpVal - 3; // shift program counter by the jumpval - the 3 bytes (intruction and value bytes)
      }
      break;
      case OP_IFEQ: {
        int16_t jumpVal = (int16_t)((m->text[m->program_counter] << 8) + m->text[m->program_counter + 1]);
        m->program_counter += 2;
        word val = pop(m);
        if(val == 0){
          m->program_counter += jumpVal - 3;
        }
      }
      break;
      case OP_IFLT: {
        int16_t jumpVal = (int16_t)((m->text[m->program_counter] << 8) + m->text[m->program_counter + 1]);
        m->program_counter += 2;
        word val = pop(m);
        if(val < 0){
          m->program_counter += jumpVal -  3;
        }
      }
      break;
      case OP_IF_ICMPEQ: {
        int16_t jumpVal = (int16_t)((m->text[m->program_counter] << 8) + m->text[m->program_counter + 1]);
        m->program_counter += 2;

        word b = pop(m);
        word a = pop(m);
        if (a == b){
          m->program_counter += jumpVal - 3;
        }
      }
      break;
      case OP_LDC_W: {
        uint16_t index = read_int16(&m->text[m->program_counter]);
        m->program_counter += 2;
        push(m, get_constant(m, index));
      }
      break;
      case OP_ILOAD: {
        byte index = m->text[m->program_counter];
        m->program_counter++;
        push(m, m->locals[m->lv + index]);
      }
      break;
      case OP_ISTORE: {
        byte index = m->text[m->program_counter];
        m->program_counter++;
        m->locals[m->lv + index] = pop(m);
      }
      break;
      case OP_IINC: {
        byte index = m->text[m->program_counter];
        m->program_counter++;
        int8_t constant = (int8_t)m->text[m->program_counter];
        m->program_counter++;
        m->locals[m->lv + index] += constant;
      }
      break;
      case OP_WIDE: {
        byte wide_op = m->text[m->program_counter];
        m->program_counter++;
        uint16_t index = read_uint16(&m->text[m->program_counter]);
        m->program_counter += 2;

        switch (wide_op) {
          case OP_ILOAD:
            push(m, m->locals[m->lv + index]);
            break;
          case OP_ISTORE:
            m->locals[m->lv + index] = pop(m);
            break;
          case OP_IINC: {
            int8_t constant = (int8_t)m->text[m->program_counter];
            m->program_counter++;
            m->locals[m->lv + index] += constant;
            break;
          }
          default:
            fprintf(stderr, "Invalid WIDE instruction");
            m->done = true;
            break;
        }
        break;
      }
      default:{
        m->done = true;
      } 
      break;
    };



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
