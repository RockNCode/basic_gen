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
#define GENERATIONS 500
#define CROSS_RATE 0.7
#define MUTATION_RATE 0.001

//#define DEBUG
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

float roulette[CHROMOSOME_NUM];

char opera[9]={0};
int soln_found = 0;
int index_of_soln = -1;
float total_score = 0;
int error = 0;
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

    return fitness;
}

void assign_fitness(float target){
    int i;
    float res;
    total_score =0;
    for(i=0; i < CHROMOSOME_NUM; i++){
        res = solve(chromosomepool[i]);
        if(!error){
            fitness_score[i] = fabs(get_fitness(target,res));
            if(soln_found){
                index_of_soln = i;
                break;
            }
        }
        else{
            // operation is not good, fitness score should be small
            // to reduce chances of survival
            fitness_score[i] = 0;
            error = 0;
        }
        total_score += fitness_score[i];
    }

}


void assign_prob()
{
    int i;
    float previous_prob = 0.0;
    for(i=0; i < CHROMOSOME_NUM; i++){
        roulette[i] = previous_prob + fitness_score[i]/total_score;
        previous_prob = roulette[i];
    }
}

void swap(int index1, int index2, int start){
    int cpy_size =9*4-start;
    char temp[cpy_size];

    strncpy(temp,chromosomepool[index1] + start,cpy_size);
    strncpy(chromosomepool[index1] + start, chromosomepool[index2] + start, cpy_size);
    strncpy(chromosomepool[index2] + start, temp, cpy_size);
}

void mutate_chromosome(int index){
    int i;
    int numberofbits = 9*4;
    for(i = 0; i < numberofbits; i++){
        char c_mutate = *(chromosomepool[index] + i);
        float rand = (float)get_rand(10000)*.0001;
        if( rand <= MUTATION_RATE ){ //probability of mutation is 1%
            GEN_LOG("mutating bit %d of index %d \n",i,index);
            if(c_mutate == '0')
                c_mutate = '1';
            else
                c_mutate = '0';

            memset(chromosomepool[index] + i,c_mutate, 1 );
        }
    }
}

char decode(char * str){
    int i;
    int count=0;
    char c = 'x';
    for(i=0 ; i < 14; i++){
        if(!strncmp(str,genes[i],4)){
            c = gene_values[i];
            return c;
        }
    }
}

void print_operation(char * op)
{
    char *p = op;
    char c;
    for(p=op; *(p+1) != '\0' ; p+=4){
        c = decode(p);
        printf(" %c \t ", c);
    }
    printf("\n");
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
    int state = -1;
    int k=0;
    get_operation(str);

    for(i=0; i<9; i++){
        float is_num = (float)(opera[i] - '0');
        if(is_num >= 0.0f && is_num <= 9.0f){
            //cannot have 2 numbers in a row.
            if(state == S_NUM){
                GEN_LOG("2 numbers in a row\n");
                error = 1;
                return -1;
            }
            //GEN_LOG("number is %f\n",is_num);
            if(i==0){
                result += is_num;
                state = S_NUM;
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
                    if(is_num == 0){
                        error = 1;
                        return -1;
                    }
                    result/= is_num;
                    break;
                default:
                    break;
            }
            state = S_NUM;

            //GEN_LOG("partial result =%f \n",result);
            continue;
        }

        if( state == -1 || state != S_NUM ){
            GEN_LOG("Cannot have an operator at the beginning/in a row \n");
            error = 1;
            return -1;
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
                error = 1;
                GEN_LOG("unknown ascii\n");
                break;
        }
    }
    GEN_LOG("result is %f \n",result);
    return result;
}

void getMates(int * mate1, int * mate2){
    //spin the wheel twice to get 2 mates
    float rand1 = (float)get_rand(1000)*.001;
    float rand2 = (float)get_rand(1000)*.001;
    int i,found = 0;

    for(i=0 ; i < CHROMOSOME_NUM ; i++){
        if(i == 0){
            if(rand1 >= 0 && rand1 <= roulette[i]){
                *mate1 = i;
                found += 1;
            }
            if(rand2 >= 0 && rand2 <= roulette[i]){
                *mate2 = i;
                found += 1;
            }
        }
        else{
            if(rand1 >= roulette[i-1] && rand1 <= roulette[i]){
                *mate1 = i;
                found += 1;
            }
            if(rand2 >= roulette[i-1] && rand2 <= roulette[i]){
                *mate2 = i;
                found += 1;
            }
        }
        if(found == 2)
            return;
    }

}

int main()
{
    int i,j;
    float target = 7.0f;
    float rand_crossover;
    create_pool();

    for(i = 0; i < GENERATIONS; i++){
        assign_fitness(target);
        if(soln_found){
            printf("solution has been found for target %f !!!! \n",target);
            printf(" chromosome = %s at generation %d \n", chromosomepool[index_of_soln],i);
            print_operation(chromosomepool[index_of_soln]);
            break;
        }
        assign_prob();

        for(j = 0; j < CHROMOSOME_NUM/2 ; j++){
            int mate1_index,mate2_index;
            float rand_crossover = (float)get_rand(1000)*.001;
            if( rand_crossover <= CROSS_RATE){
                int start_cross = get_rand(9*4);
                //spin the wheel to get 2 mates
                getMates(&mate1_index,&mate2_index);
                swap(mate1_index,mate2_index,start_cross);
                mutate_chromosome(mate1_index);
                mutate_chromosome(mate2_index);
            }
        }
    }
    if(!soln_found){
        printf("Solution not found with the given parameters\n");
    }

    free_pool();

    return 0;
}
