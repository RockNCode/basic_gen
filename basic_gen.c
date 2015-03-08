#include <stdio.h>
#include <stdlib.h>

#define S_NUM 0
#define S_SUM 1
#define S_RES 2
#define S_MUL 3
#define S_DIV 4

#define DEBUG
#ifdef DEBUG
#define GEN_LOG printf
#else
#define GEN_LOG(...)
#endif

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
	  GEN_LOG("%c \t",values[i]);
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
    GEN_LOG("\n");
}

void get_operation(char * op)
{
    char *p =op;
    int cnt = 0;
    for(p=op; *(p+1) != '\0' ; p+=4){
      opera[cnt] = decode(p);
      GEN_LOG("at get op index =%d %c \n",cnt, opera[cnt]);
      cnt++;
    }
    GEN_LOG("\n");
}

float solve(char *str){
  float result = 0;
  int i = 0;
  int state=-1;
  int k=0;
  get_operation(str);

  for(i=0; i<9; i++){
    float is_num = (float)(opera[i] - '0');
    if(is_num >= 0.0f && is_num <= 9.0f){
      //cannot have 2 numbers in a row.
      if(state == S_NUM)
	return -1;
      GEN_LOG("number is %f\n",is_num);
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

      GEN_LOG("partial result =%f \n",result);
      continue;
    }
    //GEN_LOG("op is %c \n",opera[i]);
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
      GEN_LOG("unknown ascii\n");
      break;
    }
  }
  GEN_LOG("result is %f \n",result);

}

int main()
{
  char* test = "011010100101110001001101001010100001";
  //print_operation(test);
    solve(test);
    return 0;
}
