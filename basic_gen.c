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

char opera[9]={0};

char decode(char * str){
    int i;
    int count=0;
    char c = 'x';
    for(i=0 ; i < 14; i++){
        if(!strncmp(str,operators[i],4)){
	  printf("%c \t",values[i]);
	  c = values[i];
	  return c;
        }
    }
}

void print_operation(char * op)
{
    char *p = op;
    for(p=op; *(p+1) != '\0' ; p+=4){
        decode(p);
    }
    printf("\n");
}

void get_operation(char * op)
{
    char *p =op;
    int cnt = 0;
    for(p=op; *(p+1) != '\0' ; p+=4){
      opera[cnt] = decode(p);
      printf("at get op index =%d %c \n",cnt, opera[cnt]);
      cnt++;
    }
    printf("\n");
}

int solve(char *str){
  int result = 0;
  int cur_num = 0;
  int i = 0;
  int state=-1;
  int k=0;
  get_operation(str);

  for(i=0; i<9; i++){
    int is_num = opera[i] - '0';
    if(is_num >= 0 && is_num <= 9){
      //cannot have 2 numbers in a row.
      if(state == S_NUM)
	return -1;
      printf("number is %d\n",is_num);
      if(i==0){
	result+=is_num;
	continue;
      }
      switch(state){
      case S_SUM:
	result+= is_num;
	break;
      case S_RES:
	result-= is_num;
	break;
      case S_MUL:
	result*= is_num;
	break;
      case S_DIV:
	result/= is_num;
	break;
      default:
	break;
      }
      state = S_NUM;

      printf("partial result =%d \n",result);
      continue;
    }
    //printf("op is %c \n",opera[i]);
    //cannot have 2 operators in a row.
    switch(opera[i]){
    case '+':
      state = S_SUM;
      break;
    case '-':
      state = S_RES;
      break;
    case '*':
      state = S_MUL;
      break;
    case '/':
      state = S_DIV;
      break;
    default:
      printf("unknown ascii\n");
      break;
    }
  }
  printf("result is %d \n",result);

}

int main()
{
    char* test ="011010100101110001001101001010100001";
    //print_operation(test);
    solve(test);
    return 0;
}
