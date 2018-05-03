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

clock_t start_time, finish_time;

#define U 1
#define D 2
#define L 3
#define R 4

const short goal_map[5][5] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1, 12, -1, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 0};

typedef struct {
    short x; short y;
}position;

typedef struct {
    short map[5][5];
    position blank;
    int f;
    int g;
    int h;
}STATE;

STATE state;
int depth = 0;
int min_f = 1000;
int path[100] = { 0 };

struct LA{
    short cnt;
    short action[4];
}legalAction[5][5];

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
int Conv_Manhattan(int x0, int y0, int x1, int y1){
    if((x0 <= 2 && x1 >=2) || (x0 >= 2 && x1 <= 2)){
        if((y0 == 0 &&(y1 == 2 || y1 == 3)) || (y1 == 0 && (y0 == 2 || y0 ==3)))
            return abs(x0 - x1) + abs(y0 - y1) - 1;
        else if((y0 == 0 && y1 == 4) || (y1 == 0 && y0 == 4))
            return abs(x0 - x1) + abs(y0 - y1) - 2;
        else if((y0 == 2 && y1 == 4) || (y1 == 2 && y0 == 4))
            return abs(x0 - x1) + abs(y0 - y1) - 1;
        else return abs(x0 - x1) + abs(y0 - y1);
    }
    else return abs(x0 - x1) + abs(y0 - y1);
}
int getX(int x){
    switch (x) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            return 0;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            return 1;
        case 11:
        case 12:
        case 13:
            return 2;
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
            return 3;
        case 19:
        case 20:
        case 21:
        case 22:
        case 0:
            return 4;
        default:
            return 0;
    }
}
int getY(int y){
    switch (y) {
        case 1:
        case 6:
        case 11:
        case 14:
        case 19:
            return 0;
        case 2:
        case 7:
        case 15:
        case 20:
            return 1;
        case 3:
        case 8:
        case 12:
        case 16:
        case 21:
            return 2;
        case 4:
        case 9:
        case 17:
        case 22:
            return 3;
        case 5:
        case 10:
        case 13:
        case 18:
        case 0:
            return 4;
        default:
            return 0;
    }
}
short getH(STATE a)//h2:变换曼哈顿距离
{
    short result = 0;
    int x,y;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            if(i == 2 && (j == 1 || j ==3)) continue;
            else{
                x = getX(a.map[i][j]);
                y = getY(a.map[i][j]);
                result += Conv_Manhattan(i, j, x, y);
            }
        }
    }
    return result;
}



int oppositeAction(int action)
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

void getBlankPosition(position *blank, int action)
{
    switch (action)
    {
        case U:
            blank->x--;
            break;
        case D:
            blank->x++;
            break;
        case R:
            if(blank->x == 2 && (blank->y == 0 || blank->y == 2))
                blank->y += 2;
            else
                blank->y++;
            break;
        case L:
            if(blank->x == 2 && (blank->y == 2 || blank->y == 4))
                blank->y -= 2;
            else
                blank->y --;
            break;
    }
}

void swap(short *a, short *b)
{
    short tmp = *a;
    *a = *b;
    *b = tmp;
}

int IDAstar(STATE *currentState, int steps, int h, short previousAction)  //dfs
{
    if (!memcmp(&(currentState->map), &goal_map, sizeof(short)* 25))
    {
        path[steps] = '\0';
        return 1;
    }
    if (steps + h > depth) {
        min_f = min_f > steps + h ? (steps + h) : min_f;
        return 0;
    }
    
    LA la = legalAction[currentState->blank.x][currentState->blank.y];
    for (int i = 0; i < la.cnt; i++)
    {
        STATE nextState;
        memcpy(&nextState, currentState, sizeof(STATE));
        if (oppositeAction(la.action[i]) == previousAction)    continue;
        getBlankPosition(&nextState.blank, la.action[i]);
        swap(&nextState.map[nextState.blank.x][nextState.blank.y], &nextState.map[currentState->blank.x][currentState->blank.y]);
        nextState.g = steps + 1;
        nextState.h = getH(nextState);
        nextState.f = nextState.g + nextState.h;
        path[steps] = la.action[i];
        if (IDAstar(&nextState, steps + 1, nextState.f, la.action[i]))
        {
            return 1;
        }
    }
    return 0;
}


void printInfo()
{
    
    finish_time = clock();
    printf("%.8f\n", (double)(finish_time - start_time) / CLOCKS_PER_SEC );
    int steps = 0;
    for (steps = 0; path[steps] != '\0'; steps++);
    for (int i = 0; i < steps; i++)
    {
        switch (path[i])
        {
            case U: printf("U"); break;
            case D: printf("D"); break;
            case L: printf("L"); break;
            case R: printf("R"); break;
        }
    }
    printf("\n");
}

int main()
{
    freopen("/Users/jqc/Documents/study/人工智能基础/exp1/exp1/Ah1/22数码问题/input.txt", "r", stdin);
    freopen("/Users/jqc/Documents/study/人工智能基础/exp1/exp1/Ah1/22数码问题/output_IDAh2.txt", "w", stdout);
    initial_legalActionList();
    start_time = clock();
    for (short i = 0; i < 5; ++i)
        for (short j = 0; j < 5; ++j)
        {
            int tmp;
            scanf("%d", &tmp);
            state.map[i][j] = tmp;
            if (state.map[i][j] == 0)
            {
                state.blank.x = i;    state.blank.y = j;
            }
        }
    state.h = getH(state);
    depth = state.h;
    while (1)
    {
        if (IDAstar(&state, 0, state.h, 0))
        {
            break;
        }
        depth=min_f;
        min_f = 1000;
    }
    printInfo();
    return 0;
}

