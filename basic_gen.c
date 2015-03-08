#include <stdio.h>
#include <stdlib.h>

#define S_NUM 0
#define S_SUM 1
#define S_RES 2
#define S_MUL 3
#define S_DIV 4

char *operators[14] = {
    "0000",
    "0001",
    "0010",
    "0011",
    "0100",
    "0101",
    "0110",
    "0111",
    "1000",
    "1001",
    "1010",
    "1011",
    "1100",
    "1101"
};

char values[14] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '+',
    '-',
    '*',
    '/'
};

int decode(char * str){
    int i;
    for(i=0 ; i < 14; i++){
        if(!strncmp(str,operators[i],4)){
	  //printf("%c \t",values[i]);
            return i;
        }
    }
}

void print_operation(char * op)
{
    char *p =op;
    for(p=op; *(p+1) != '\0' ; p+=4){
        decode(p);
    }
    printf("\n");
}

int solve(char *str){
  int result = 0;
  int num = 0;
  int it=0;
  int state=-1;
  char *p = str;
  for(p=str; *(p+1) != '\0' ; p+=4){
    int index = decode(str);
    if(index < 9){
      //printf("index is less than 9 for %d \n",index);
      //get ascii translation
      num = values[index] - '0';
      printf("number is %d",index);
      state = 0;
      //continue;
    }
    it++;
  } 

}

int main()
{
    char* test ="011010100101110001001101001010100001";
    //print_operation(test);
    solve(test);
    return 0;
}
