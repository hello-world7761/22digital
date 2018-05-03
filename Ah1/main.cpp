//
//  main.cpp
//  Ah1
//
//  Created by 姜庆彩 on 2018/5/1.
//  Copyright © 2018年 姜庆彩. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
using namespace std;

#define U 1
#define D 2
#define L 3
#define R 4

clock_t start_time, finish_time;
double t;

typedef struct indexNode {
    long stateIndex;
    struct indexNode *next;
}indexNode;

typedef struct {
    indexNode *chain;
    long count;
}hashTableNode;

typedef struct {
    short x; short y;
} position;


typedef struct {
    short map[5][5];
    short action;
    short steps;
    int f;
    int g;
    int h;
    long long parent;
    long long sIndex;
    position current; //0的位置
} state;

typedef struct {
    long pstate;
} pState;


struct {
    short cnt;
    short action[4];
}legalAction[5][5];


short steps = 0;
long long heapSize = -1; //当前堆中最后元素下标
state stateList[10000000];
pState heap[10000000];
position goal[25];
const short goal_map[5][5] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1, 12, -1, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 0};

void initial_goal()
{
    for (short i = 0; i <= 4; i++)
        for (short j = 0; j <= 4; j++)
        {
            if (goal_map[i][j] != -1)
                goal[goal_map[i][j]] = { i, j};
        }
}

void initial_legalActionList(){//每个位置可以移动的方向
    for(int i = 0; i < 5; ++i){
        for(int j = 0; j <5; ++j){
            legalAction[i][j].cnt = 0;
            if((((j == 0)||(j ==2)||(j==4)) && i > 0) || (i ==1 && (j==1||j==3)) || (i==4 &&(j==1||j==3)))
                legalAction[i][j].action[legalAction[i][j].cnt++] = U;
            if((((j == 0)||(j ==2)||(j==4)) && i < 4) || (i ==0 && (j==1||j==3)) || (i==3 &&(j==1||j==3)))
                legalAction[i][j].action[legalAction[i][j].cnt++] = D;
            if(j==2 || j==4 || (j==1 && (i==0 || i==1|| i==3||i==4)) || (j==3 && (i==0 || i==1|| i==3||i==4)))
                legalAction[i][j].action[legalAction[i][j].cnt++] = L;
            if(j==0 || j==2 || (j==1 && (i==0 || i==1|| i==3||i==4)) || (j==3 && (i==0 || i==1|| i==3||i==4)))
                legalAction[i][j].action[legalAction[i][j].cnt++] = R;
        }
    }
}
void swap(long*a, long*b)
{
    long tmp = *a;
    *a = *b;
    *b = tmp;
}


void heapAdjust(long long i)
{
    long min = i;
    while (min <= heapSize)
    {
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        long tmp = min;
        if (l <= heapSize && stateList[heap[l].pstate].f < stateList[heap[min].pstate].f)
            min = l;
        if (r <= heapSize && stateList[heap[r].pstate].f < stateList[heap[min].pstate].f)
            min = r;
        if (min != tmp)
        {
            swap(&heap[min].pstate, &heap[tmp].pstate);
            i = min;
        }
        else break;
    }
}

void heap_insert(long long i)
{
    heap[++heapSize].pstate = i;
    long long j = heapSize;
    while (j > 0 && stateList[heap[(j - 1) / 2].pstate].f > stateList[heap[j].pstate].f)
    {
        swap(&heap[(j - 1) / 2].pstate, &heap[j].pstate);
        j = (j - 1) / 2;
    }
}

state extract_min()
{
    state *tmp = &stateList[heap[0].pstate];
    heap[0] = heap[heapSize--];
    heapAdjust(0);
    return *tmp;
}

short getH(state a)//h1:错位棋子数
{
    short result = 0;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++)
            if(a.map[i][j] != goal_map[i][j])
                result ++;
    }
    return result;
}

void swap_short(short *a, short *b)
{
    short tmp = *a;
    *a = *b;
    *b = tmp;
}

void handle(long long *index, state *tmp)
{
    state *tmp1 = &stateList[*index];
    for (short i = 0; i <= 4; i++)
        for (short j = 0; j <= 4; j++)
        {
            tmp1->map[i][j] = tmp->map[i][j];
        }
    swap_short(&(tmp1->map[tmp1->current.x][tmp1->current.y]), &(tmp1->map[tmp->current.x][tmp->current.y]));
    tmp1->parent = tmp->sIndex;
    tmp1->g = tmp->g + 1; tmp1->h = getH(*tmp1);  tmp1->f = tmp1->g + tmp1->h;  tmp1->sIndex = *index;
    tmp1->steps = tmp->steps;
    heap_insert(*index);
}

int getOppositeAction(int action)
{
    switch (action)
    {
        case U: return D;
        case D: return U;
        case L: return R;
        case R: return L;
        default: return 0;
    }
}
void printActionList(int parent, int lastAction)
{
    finish_time = clock();
    printf("%.8f\n", (double)(finish_time - start_time) / CLOCKS_PER_SEC );
    short *action = new short[steps];
    action[steps - 1] = getOppositeAction(lastAction);
    int i = 1; long long tmp = parent;
    while (tmp != 0)
    {
        action[steps - i - 1] = getOppositeAction(stateList[tmp].action);
        tmp = stateList[tmp].parent;
        i++;
    }
    for (int i = 1; i < steps; i++)
    {
        switch (action[i])
        {
            case U: printf("U"); break;
            case D: printf("D"); break;
            case L: printf("L"); break;
            case R: printf("R"); break;        }
    }
    printf("\n");
    delete[]action;
}

int main() {
    
    freopen("/Users/jqc/Documents/study/人工智能基础/exp1/exp1/Ah1/22数码问题/input.txt", "r", stdin);
    initial_legalActionList();
    initial_goal();
    start_time = clock();
    long long count = 0;
    for(int i = 0; i < 5; ++i){
        for(int j = 0; j < 5; ++j){
            int tmp;
            scanf("%d", &tmp);
            stateList[count].map[i][j] = tmp;
            if (stateList[count].map[i][j] == 0)
            {
                stateList[count].current.x = i; stateList[count].current.y = j;
            }
            if (i == 4 && j == 4)
            {
                stateList[count].g = 0; stateList[count].h = getH(stateList[count]); stateList[count].f = stateList[count].g + stateList[count].h; stateList[count].steps = 0;
                stateList[count].action = 0; stateList[count].parent = NULL; stateList[count].sIndex = count;
                heap_insert(count);
                count++;
            }
        }
    }
    while (1)
    {
        state tmp = extract_min();
        tmp.steps++;
        if (tmp.h == 0)
        {
            freopen("/Users/jqc/Documents/study/人工智能基础/exp1/exp1/Ah1/22数码问题/output_Ah1.txt ", "w", stdout);
            steps = tmp.steps;
            printActionList(tmp.parent, tmp.action);
            return 0;
        }
        for (short i = 0; i < legalAction[tmp.current.x][tmp.current.y].cnt; i++)
        {
            switch (legalAction[tmp.current.x][tmp.current.y].action[i])
            {
                case U:
                    if (tmp.action != U)
                    {
                        stateList[count].action = D;
                        stateList[count].current.x = tmp.current.x -1; stateList[count].current.y = tmp.current.y;
                        handle(&count, &tmp); count++;
                    }
                    break;
                case D:
                    if (tmp.action != D)
                    {
                        stateList[count].action = U;
                        stateList[count].current.x = tmp.current.x + 1; stateList[count].current.y = tmp.current.y;
                        handle(&count, &tmp); count++; //printf("%d\n", count);
                    }
                    break;
                case L:
                    if (tmp.action != L)
                    {
                        stateList[count].action = R;
                        if(tmp.current.x == 2 && (tmp.current.y==2 || tmp.current.y==4)){
                            stateList[count].current.x = tmp.current.x; stateList[count].current.y = tmp.current.y - 2;
                        }// 跨越-1
                        else{
                            stateList[count].current.x = tmp.current.x; stateList[count].current.y = tmp.current.y - 1;
                        }
                        handle(&count, &tmp); count++;
                    }
                    break;
                case R:
                    if (tmp.action != R)
                    {
                        stateList[count].action = L;
                        if(tmp.current.x == 2 && (tmp.current.y==0 || tmp.current.y==2)){
                            stateList[count].current.x = tmp.current.x; stateList[count].current.y = tmp.current.y + 2;
                        }// 跨越-1
                        else{
                            stateList[count].current.x = tmp.current.x; stateList[count].current.y = tmp.current.y + 1;
                        }
                        handle(&count, &tmp); count++;
                    }
                    break;
            }
        }
    }
    return 0;
}


