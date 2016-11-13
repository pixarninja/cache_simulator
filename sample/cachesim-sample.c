/****************************************************************
/
/	filename: cachesim.c
/
/	description: Implements the cache simulator
/	
/	authors: Sample
/
/****************************************************************/


/***************************************
/ set up:
/ cache_array[set_value_location][associativity_number(ie blocks per set)][0] = the tag bits
/ cache_array[set_value_location][associativity_number(ie blocks per set)][1] = the dirty bit
/ cache_array[set_value_location][associativity_number(ie blocks per set)][2] = the FIFO counting bit
/ cache_array[set_value_location][associativity_number(ie blocks per set)][3] = the valid bit
/ 
/*************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

unsigned int log_base_two(float number);

float TOTAL_RUN_TIME;
float TOTAL_HIT_RATE;
float LOAD_HIT_RATE;
float STORE_HIT_RATE;
float AVERAGE_MEMORY_ACCESS_LATENCY;
float TOTAL_MEMORY_OPS;
float LOAD_MEMORY_OPS;
float STORE_MEMORY_OPS;
float TOTAL_INSTRUCTIONS;

int main(int argc, char** argv)
{
   FILE *trace_file = NULL;
   FILE *config_file = NULL;
   FILE *output_file = NULL;
   int line_size = 0;
   int associativity = 0;
   int cache_size = 0;
   int replacement_policy = 0;
   int miss_penalty = 0;
   int write_allocate = 0;
   unsigned int set_amount = 0;
   int FIFO_finder = 0;
   int FIFO_counter = 0;
   int random_number = 0;
   int x = 0;
   int y = 0;
   srand(time(NULL));
   char out_file[1024];

   char access_type;
   unsigned int address = 0;
   unsigned int instructions_between = 0;
   unsigned int set_index = 0;
   unsigned int tag_value = 0;
   int associativity_counter = 0;
   
   unsigned int initial = 0xffffffff;
   
   if(argc != 3)
   {
      printf("Usage: %s <config_file> <trace_file>\n", argv[0]);
      exit(0);
   }

   sprintf(out_file, "%s%s", argv[2], ".out"); //turns the trace file into trace file.out
   if((output_file = fopen(out_file, "w+")) == NULL)
   {
      fprintf(stderr, "Can't open output file %s\n", out_file);
      exit(1);
   }

   if((config_file = fopen(argv[1], "r")) == NULL)
   {
      fprintf(stderr, "Can't open config file %s\n", argv[1]);
      exit(1);
   }
   fscanf(config_file, "%i\n", &line_size);   //read the data in from the config file!
   fscanf(config_file, "%i\n", &associativity);
   fscanf(config_file, "%i\n", &cache_size);
   fscanf(config_file, "%i\n", &replacement_policy);     
   fscanf(config_file, "%i\n", &miss_penalty);
   fscanf(config_file, "%i", &write_allocate);
   fclose(config_file);

   cache_size = cache_size * 1024; //it's always in KB, so just multiply by 2^10

   if(associativity == 0) //if it is fully associative (WHY??) then there is only 1 set
      set_amount = 1;
   else
      set_amount = (cache_size / (associativity * line_size));
   
   unsigned int cache_array[set_amount][associativity][4]; //this is our cache!

   for(x = 0; x < set_amount; x++)           //this assigns values to our array so we don't get junk stuff...
   {
      for(y = 0; y < associativity; y++)
      {
         cache_array[x][y][3] = 0;
         cache_array[x][y][2] = 0;
         cache_array[x][y][1] = 0;
         cache_array[x][y][0] = 0;
      }
   }
   x = 0;
   y = 0;

   if((trace_file = fopen(argv[2], "r")) == NULL)
   {
      fprintf(stderr, "can't open trace file %s\n", argv[2]);
      exit(1);   
   }

   while(!feof(trace_file))
   {
      fscanf(trace_file, "%c %x %i\n", &access_type, &address, &instructions_between);
      TOTAL_INSTRUCTIONS = 1 + instructions_between + TOTAL_INSTRUCTIONS; //this is just instructions, each memory access is 1 instruction, and the instructions between are x # of instructions
      TOTAL_RUN_TIME = TOTAL_RUN_TIME + instructions_between + 1;         //assuming the instructions between all take 1 clock cycle
      
      set_index = ((initial & (set_amount*line_size - line_size))  & address) >> log_base_two((float)line_size);     //gets the set number using math logic
      tag_value = ((initial ^ (set_amount*line_size - 1)) & address) >> log_base_two((float)(set_amount*line_size)); //gets the tag number using math logic

      //if the set associativity is greater than 1 (not direct mapped) this loop will take extra time, no specification was provided on how long searching through the tag bits takes though..
      while(1)
      {
         if((cache_array[set_index][associativity_counter][0] == tag_value) && (cache_array[set_index][associativity_counter][3] != 0)) //if tag is in cache, and it is valid... then it's a hit!
         {
            TOTAL_RUN_TIME = TOTAL_RUN_TIME + 1;  //this holds the total run time, add the hit/miss penalty for each occurance
            TOTAL_HIT_RATE = TOTAL_HIT_RATE + 1;  //this holds the hit rate, just add 1 when it's a hit
            TOTAL_MEMORY_OPS = TOTAL_MEMORY_OPS + 1; //this is the total number of memory ops, just add 1 for every memory op
            if(access_type == 's')
            {
               STORE_MEMORY_OPS = STORE_MEMORY_OPS + 1; //same but for stores
               STORE_HIT_RATE = STORE_HIT_RATE + 1;
            } 
            else
            {
               LOAD_MEMORY_OPS = LOAD_MEMORY_OPS + 1;   //same but for loads
               LOAD_HIT_RATE = LOAD_HIT_RATE + 1;
            }

            if(write_allocate == 1 && (access_type == 's'))                //if it is a hit then it's already in the cache, this won't change hit/miss... just set dirty bit!
            {
               cache_array[set_index][associativity_counter][1] = 1;       //set the dirty bit
            }
                                                                           //don't need to take FIFO into account, the input order is not changed and nothing is evicted
            associativity_counter = 0;
            break;
         }

         else if(associativity_counter >= associativity)
         {
            TOTAL_RUN_TIME = TOTAL_RUN_TIME + miss_penalty;
            TOTAL_MEMORY_OPS = TOTAL_MEMORY_OPS + 1;
            if(access_type == 's')
            {
               STORE_MEMORY_OPS = STORE_MEMORY_OPS + 1;
            } 
            else
            {
               LOAD_MEMORY_OPS = LOAD_MEMORY_OPS + 1;
            }


            associativity_counter = 0;
            if(write_allocate == 0 && (access_type == 's')) //if it is a miss, but is write through, then nothing happens, don't add to cache
            {
               break;
            }

            if(replacement_policy == 1) //if the replacement_policy is FIFO
            {
               while(associativity_counter < associativity) //looks for the value in the current set with the largest fifo value, that value was put in first and will be removed.
               {
                  if((cache_array[set_index][associativity_counter][2] > FIFO_counter) && (cache_array[set_index][associativity_counter][3] != 0))
                  {
                     FIFO_finder = associativity_counter;
                     FIFO_counter = cache_array[set_index][associativity_counter][2];
                  }
                  associativity_counter++;
               }

               if((cache_array[set_index][FIFO_finder][1] == 1) && (write_allocate == 1)) 
               {
                  //update the main memory, just evicted a write change and we are using write back
                  //however... we aren't worrying about byte memory just time... so don't do anything really.
                  //this is where the time would be taken for this operation, but no specifications were given on the cycle time for when this happens 
               }

               cache_array[set_index][FIFO_finder][0] = tag_value; // set up the tag and stuff
               cache_array[set_index][FIFO_finder][3] = 1;
               if(write_allocate == 1 && (access_type == 's'))
               {
                  cache_array[set_index][FIFO_finder][1] = 1; 
               }

               cache_array[set_index][FIFO_finder][2] = 0; 
               associativity_counter = 0;
               while(associativity_counter < associativity) //this loop will incriment the fifo order to make sure it stays current
               {
                  if(cache_array[set_index][associativity_counter][3] == 1) //if it is valid...
                  {
                     cache_array[set_index][associativity_counter][2]++; //increase the FIFO order
                  }
                  associativity_counter++;
               }

               FIFO_counter = 0;
               FIFO_finder = 0;
               associativity_counter = 0;
               break;
            }

            else
            {
               random_number = random() % associativity; //makes a rAndom number!
               if((cache_array[set_index][random_number][1] == 1) && (write_allocate == 1)) 
               {
                  //update the main memory, just evicted a write change and we are using write back
                  //however... we aren't worrying about byte counts... so don't do anything really.
                  //this is where the time would be taken for this operation, but no specifications were given on the cycle time for when this happens 
               }

               cache_array[set_index][random_number][0] = tag_value; //puts the tag in the set place where the random number is
               cache_array[set_index][random_number][3] = 1;
               if(write_allocate == 1 && (access_type == 's'))
               {
                  cache_array[set_index][random_number][1] = 1; 
               }

               associativity_counter = 0;
               break;

            }
         }

         else if(cache_array[set_index][associativity_counter][3] == 0) //if there is an empty spot in our cache (valid = 0) then lets set it up!
         {
            TOTAL_RUN_TIME = TOTAL_RUN_TIME + miss_penalty; //it's a miss... add the penalty to the time.
            TOTAL_MEMORY_OPS = TOTAL_MEMORY_OPS + 1;
            if(access_type == 's')
            {
               STORE_MEMORY_OPS = STORE_MEMORY_OPS + 1;
            } 
            else
            {
               LOAD_MEMORY_OPS = LOAD_MEMORY_OPS + 1;
            }

            if(access_type == 'l')                          //if the miss was a load, then just add the tag to the set at the spot, and make it valid.
            {
               cache_array[set_index][associativity_counter][0] = tag_value;   //set up the tag
               cache_array[set_index][associativity_counter][3] = 1;           //set the valid bit
               cache_array[set_index][associativity_counter][2] = 0;           //new entry, so it is the last in.
            }
            else if((write_allocate == 1) && (access_type == 's'))  //if the miss was a store (and is write allocate/write back) then set the dirty bit, and put the tag in
            {
               cache_array[set_index][associativity_counter][1] = 1;          //dirty bit
               cache_array[set_index][associativity_counter][0] = tag_value;  //tag bits
               cache_array[set_index][associativity_counter][3] = 1;          //valid bit
               cache_array[set_index][associativity_counter][2] = 0;          //new entry, so it is the last in.
            }
            else if((write_allocate == 0) && (access_type == 's'))  //if the miss was a store (and is not write through/no write allocate) then don't put it in the cache
            {
               cache_array[set_index][associativity_counter][3] = 0;
               associativity_counter = 0;
               break;
            }
            
            if(replacement_policy == 1)  //if we do FIFO eviction type, then do this!
            {
               associativity_counter = 0;
               while(associativity_counter < associativity) //this loop incriments the fifo's to make sure they stay in order and current
               {
                  if(cache_array[set_index][associativity_counter][3] == 1) //if it is valid...
                  {
                     cache_array[set_index][associativity_counter][2]++; //increase the FIFO order
                  }
                  associativity_counter++;
               }
            }
            associativity_counter = 0;
            break;
         }
         associativity_counter++;
      }
   }

   fclose(trace_file);

   /* EVERY instruction is a memory_access, and since each memory_op is an instruction + the memory_operation, add the number of memory_ops to the total_inst count 
    * The TOTAL_INSTRUCTIONS accounts for each instruction not a memory access, and it accounts for the memory access instruction, but we also need to account for the memory access of the memory access instruction
    * that is where the memory_ops comes in at, because the memory_ops is the number of memory operations performed.  Adding these two values together, we get the memory access number.
    * divide the total run time by the number of memory accesses to get an average number of cycles per memory access.                      
    */

   AVERAGE_MEMORY_ACCESS_LATENCY = TOTAL_RUN_TIME / (TOTAL_INSTRUCTIONS + TOTAL_MEMORY_OPS); //takes the run time divided by the total number of memory accesses, this is the average memory access latency: 
                                                                                             //same as AMAL = hit time + miss rate * miss penalty
   TOTAL_HIT_RATE = TOTAL_HIT_RATE/TOTAL_MEMORY_OPS * 100;
   LOAD_HIT_RATE = LOAD_HIT_RATE/LOAD_MEMORY_OPS * 100;
   STORE_HIT_RATE = STORE_HIT_RATE/STORE_MEMORY_OPS * 100;

   fprintf(output_file, "Total Hit Rate: %.01f%%\nLoad Hit Rate: %.01f%%\nStore Hit Rate: %.01f%%\nTotal Run Time: %.00f cycles\nAverage Memory Access Latency: %.05f" //writes to the out file the stuff wanted
                      , TOTAL_HIT_RATE, LOAD_HIT_RATE, STORE_HIT_RATE, TOTAL_RUN_TIME, AVERAGE_MEMORY_ACCESS_LATENCY);
   fclose(output_file);

   exit(0);
}

/* my nifty log function! just returns a log base 2 */
unsigned int log_base_two(float number)
{
   unsigned int factor = 0;
   number = log(number)/log(2);
   factor = (unsigned int)number;   
   return factor;
}


