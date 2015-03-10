#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define S_NUM 0
#define S_SUM 1
#define S_RES 2
#define S_MUL 3
#define S_DIV 4

//Genetic
#define CHROMOSOME_NUM 10
#define GENERATIONS 10
#define CROSS_RATE 0.7
#define MUTATION_RATE 0.001

#define DEBUG
#ifdef DEBUG
#define GEN_LOG printf
#else
#define GEN_LOG(...)
#endif

char *chromosomepool[CHROMOSOME_NUM];

char *genes[14] = {
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

char gene_values[14] = {
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

float fitness_score[CHROMOSOME_NUM];
float fitness_prob[CHROMOSOME_NUM];

char opera[9]={0};
int soln_found = 0;
float total_prob = 0;
//function prototypes
float solve(char *str);

void msleep (unsigned int ms) {
    int microsecs;
    struct timeval tv;
    microsecs = ms * 1000;
    tv.tv_sec  = microsecs / 1000000;
    tv.tv_usec = microsecs % 1000000;
    select (0, NULL, NULL, NULL, &tv);
}

int get_rand(int max)
{
    int mrand;
    static int init =0;
    if(!init){
        srand (time(NULL));
        init =1;
    }
    mrand = rand() % max;

    return mrand;
}

// generate a random list of chromosomes.
void generate_chromosome(char* chromo){
    int i;
    int is_num=1;
    int m_rand;
    for (i=0; i<9;i++){
        //msleep(100);
        if(is_num){
            //get from the numbers pool
            m_rand = get_rand(9);
            strcpy(chromo+i*4,genes[m_rand]);
            is_num=0;
        }else{
            //get from the operator pool
            m_rand = get_rand(3) + 10;
            strcpy(chromo+i*4,genes[m_rand]);
            is_num=1;
        }
    }
}

void create_pool()
{
    int i;
    for(i=0; i < CHROMOSOME_NUM; i++){
        chromosomepool[i] = (char*)malloc(9*4*sizeof(char));
        generate_chromosome(chromosomepool[i]);
    }
}

void free_pool()
{
    int i;
    for(i=0; i < CHROMOSOME_NUM; i++){
        free(chromosomepool[i]);
    }
}


float get_fitness(float target, float result){
    float fitness;

    if(target - result == 0){
        soln_found =1;
        return 0;
    }

    fitness = 1.0f / (target - result );
    total_prob += fitness;
    return fitness;
}

void assign_fitness(float target){
    int i;
    float res;
    total_prob =0;
    for(i=0; i < CHROMOSOME_NUM; i++){
        res = solve(chromosomepool[i]);
        fitness_score[i] = get_fitness(target,res);
    }

}

void assign_prob()
{
    int i;

    for(i=0; i < CHROMOSOME_NUM; i++){
        fitness_prob[i] = fitness_score[i]/total_prob;
    }
}

void swap(int index1, int index2, int start){
    int cpy_size =9*4-start;
    char temp[cpy_size];
    strncpy(temp,chromosomepool[index1] + start,cpy_size);
    strncpy(chromosomepool[index1] + start, chromosomepool[index2] + start, cpy_size);
    strncpy(chromosomepool[index2] + start, temp, cpy_size);
}

//cross over 2 chromosomes and produce an offspring.
char* cross_over(char* chromosome1, char* chromosome2, int position){

}

void mutate(int index, int bitposition){
    
    char c_mutate = *(chromosomepool[index] + bitposition);

    if(c_mutate == '0')
        c_mutate = '1';
    else
        c_mutate = '0';

    memset(chromosomepool[index] + bitposition,c_mutate, 1 );
}

void natural_select(){
}

char decode(char * str){
    int i;
    int count=0;
    char c = 'x';
    for(i=0 ; i < 14; i++){
        if(!strncmp(str,genes[i],4)){
            GEN_LOG("%c \t",gene_values[i]);
            c = gene_values[i];
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
        //GEN_LOG("at get op index =%d %c \n",cnt, opera[cnt]);
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
            //GEN_LOG("number is %f\n",is_num);
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

            //GEN_LOG("partial result =%f \n",result);
            continue;
        }
        //GEN_LOG("op is %c \n",opera[i]);
        //cannot have 2 goperators in a row.
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
    return result;
}

int main()
{
    int i;

    create_pool();
    assign_fitness(42.0f);
    for (i=0; i<CHROMOSOME_NUM; i++){
        printf("Fitness score[%d] = %f \n",i,fitness_score[i]);
    }
    assign_prob();
    for (i=0; i<CHROMOSOME_NUM; i++){
        printf("Fitness prob[%d] = %f \n",i,fitness_prob[i]);
    }
    //Start genetic algorithm

    free_pool();

    return 0;
}
