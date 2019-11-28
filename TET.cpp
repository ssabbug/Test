#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <stdbool.h> //true=1 false=0
#include <time.h>
#include <stdlib.h>
//자리를 지정하고 empty와 블럭들을 칠해주면 버퍼없이 안깜빡이지 않을까???
//생각해보니 드럽게 느려질듯하다 그러니 전이랑 비교해서 바뀐부분만 새롭게 그려보자
#pragma warning (disable:4996)
#define RIGHT 77
#define LEFT 75
#define UP 72
#define DOWN 80      //소프트드랍
#define SPACE_BAR 64 //하드드랍
#define C 99      //홀드기능
#define X 120      //우로돌려
#define Z 122      //좌로돌려
#define ESC 27      //강제종료
#define MAIN_X 12      //게임판 가로크기
#define MAIN_Y 23      //게임판 세로크기
#define WALL 1      //벽
#define INACTIVE_BLOCK 2      //바닥에 닿은 블록
#define ACTIVE_BLOCK 3
#define EMPTY 0
#define CEILING -1      //천장
#define MAIN_X_ADJ 3 //게임판 위치조정
#define MAIN_Y_ADJ 1 //게임판 위치조정
#define STATUS_X_ADJ MAIN_X_ADJ+MAIN_X+1

void cusor(int argc, char argv) {
   CONSOLE_CURSOR_INFO cursorInfo = { 0, };
   cursorInfo.bVisible = argc;
   cursorInfo.dwSize = 1;
   SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
   return 0;
}


int rand_block(void); //다음블럭 지정
void new_block();
void block_move(); // 오왼, 솦드랍
//void block_hold();
void input_key(); //키입력 받기
int block_drop();
void title();
void main_reset();
void draw_main();   //숫자를 기호로 출력해주는 함수
void output_gameover();
void level_up(int score);
void reset();
void gotoxy();
void reset_main_cpy(void); //main_cpy를 초기화 



void gotoxy(int x, int y) {
   COORD Pos = { 2 * x, y };
   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}



char block[7][4][4][4] = {
  {
   {0,0,0,0
   ,0,1,1,0
   ,0,1,1,0
   ,0,0,0,0},

   {0,0,0,0
   ,0,1,1,0
   ,0,1,1,0
   ,0,0,0,0},

   {0,0,0,0,
   0,1,1,0,
   0,1,1,0,
   0,0,0,0},

   {0,0,0,0
   ,0,1,1,0,
   0,1,1,0,
   0,0,0,0} }   //ㅁ
   ,
   { {0,1,0,0,
     0,1,0,0,
     0,1,0,0,
     0,1,0,0},

   {0,0,0,0,
   1,1,1,1,
   0,0,0,0,
   0,0,0,0},

   {0,0,1,0,
   0,0,1,0,
   0,0,1,0,
   0,0,1,0},

   {0,0,0,0,
   0,0,0,0,
   1,1,1,1,
   0,0,0,0} }      //ㅣ

   ,{
   {0,0,0,0,
   1,1,0,0,
   0,1,1,0,
   0,0,0,0},

   {0,0,0,0,
   0,0,1,0,
   0,1,1,0,
   0,1,0,0},

   {0,0,0,0,
   1,1,0,0,
   0,1,1,0,
   0,0,0,0},

   {0,0,0,0,
   0,0,1,0,
   0,1,1,0,
   0,1,0,0} }      //Z
,
   {
   {0,0,0,0,
   0,1,1,0,
   1,1,0,0,
   0,0,0,0},

   {0,0,0,0,
   1,0,0,0,
   1,1,0,0,
   0,1,0,0},

   {0,0,0,0,
   0,1,1,0,
   1,1,0,0,
   0,0,0,0},

   {0,0,0,0,
   1,0,0,0,
   1,1,0,0,
   0,1,0,0} }      //s
   ,
   {
   {0,0,0,0
   ,0,0,1,0
   ,1,1,1,0,
   0,0,0,0},

   {0,0,0,0,
   1,1,0,0,
   0,1,0,0,
   0,1,0,0},

   {0,0,0,0,
   0,0,0,0,
   1,1,1,0,
   1,0,0,0},

   {0,0,0,0,
   0,1,0,0,
   0,1,0,0,
   0,1,1,0} }      //L
,
 { {0,0,0,0,
   1,0,0,0,
   1,1,1,0,
   0,0,0,0},

   {0,0,0,0,
   0,1,0,0,
   0,1,0,0,
   1,1,0,0},

   {0,0,0,0,
   0,0,0,0,
   1,1,1,0,
   0,0,1,0},

   {0,0,0,0,
   0,1,1,0,
   0,1,0,0,
   0,1,0,0} }      //j

 ,{ {0,0,0,0,
   0,1,0,0,
   1,1,1,0,
   0,0,0,0},

   {0,0,0,0,
   0,1,0,0,
   0,1,1,0,
   0,1,0,0},

   {0,0,0,0,
   0,0,0,0,
   1,1,1,0,
   0,1,0,0},

   {0,0,0,0,
   0,1,0,0,
   1,1,0,0,
   0,1,0,0} }      //s
};
int speed = 0;
int score;
int last_score = 0;
int main_org[MAIN_Y][MAIN_X];   //메인화면정보를 담고있음
int main_cpy[MAIN_Y][MAIN_X];   //이전메인화면정보를 담고있음(확인용)
int speed;
int level;
int level_goal;
int bx;
int by;
int cnt;
int b_type; //블록 종류를 저장
int b_rotation; //블록 회전값 저장
int b_type_next; //다음 블록값 저장
int new_block_on = 0; //새로운 블럭을 만들어야하는지 확인하는 플레그
int block_crush_on = 0; //블럭이 닿았음을 알리는 플레그

int main() {
   srand((unsigned int)time(NULL));
   cusor(false, 0.1);
   title();

   return 0;
}

int block_drop(void) {
   while (1) {
      block_move(DOWN);
      Sleep(500);
      if (_kbhit == 0) {

      }
   }
}


void title(void) {
   printf("TETRIS temporary title");      //타이틀 화면
   gotoxy(20, 16); printf("1. 게임 시작");
   gotoxy(20, 18); printf("2. 조작키 설명");
   char key = 0;
   if (_kbhit() == 0) {
      key = _getch();
      switch (key) {
      case '1':
         reset();

         break;
      case '2':
         system("cls");
         gotoxy(10, 0);
         printf("\n\n");
         printf("◁  ▷  : 좌우이동\n");
         printf("  ▽   :소프트드랍\n");
         printf("  spacebar   :하드드랍\n");
         printf("  X   :오른쪽으로 돌리기\n");
         printf("  Z   :  왼쪽으로 돌리기\n");
         printf("  ESC   :  강제종료\n");
         gotoxy(20, 18); printf("press any key to start game");
         getch();
         reset();
         draw_main();
         break;
      default:
         system("cls");
         title();
      }
   }
}
void reset() {  //게임 시작 초기값
   level = 1;
   score = 0;
   level_goal = 1000;
   cnt = 10;  //현재 단계에서 제거해야할 줄수
   speed = 100;

   system("cls");

   main_reset();

   draw_main();
   b_type_next = rand_block();
   new_block();


}
void draw_main(void) {
   int i, j;

   for (j = 1; j < MAIN_X - 1; j++) {    //천장은 계속 새로운블럭이 지나가서 지워지면 새로 그려줌
      if (main_org[3][j] == EMPTY) main_org[3][j] = CEILING;
   }
   for (i = 0; i < MAIN_Y; i++) {
      for (j = 0; j < MAIN_X; j++) {
         if (main_cpy[i][j] != main_org[i][j]) {
            gotoxy(MAIN_X_ADJ + j, MAIN_Y_ADJ + i);
            switch (main_org[i][j]) {
            case EMPTY: //빈칸모양
               printf("  ");
               break;
            case CEILING: //천장모양
               printf(". ");
               break;
            case WALL: //벽모양
               printf("▩");
               break;
            case INACTIVE_BLOCK: //굳은 블럭 모양 
               printf("□");
               break;
            case ACTIVE_BLOCK: //움직이고있는 블럭 모양 
               printf("■");
               break;
            }
         }
      }
   }
   for (i = 0; i < MAIN_Y; i++) { //게임판을 그린 후 main_cpy에 복사 
      for (j = 0; j < MAIN_X; j++) {
         main_cpy[i][j] = main_org[i][j];
      }
   }
}
void input_key(int key) {
   switch (key) {
   case RIGHT:
      block_move(RIGHT);
      break;
   case LEFT:
      block_move(LEFT);
      break;
   case X:
      block_move(X);
   case X - 32:
      break;
   case Z:
      block_move(Z);
   case Z - 32:
      break;
   case ESC:
      exit(0);
      break;
   }
}


void main_reset() {      //게임판초기화
   for (int i = 0; i < MAIN_X; i++) {
      for (int j = 1; j < MAIN_Y; j++) {
         main_org[i][j] = EMPTY;
         main_cpy[i][j] = 100;
      }
   }
   for (int i = 1; i < MAIN_Y - 1; i++) {  //좌우벽
      main_org[i][0] = WALL;
      main_org[i][MAIN_X - 1] = WALL;
   }
   for (int i = 0; i < MAIN_X; i++) {  //바닥
      main_org[MAIN_Y - 1][i] = WALL;
   }
   for (int i = 0; i < MAIN_X; i++) {   //천장
      main_org[3][i] = CEILING;
   }
}
void reset_main_cpy(void) { //main_cpy를 초기화 
   int i, j;

   for (i = 0; i < MAIN_Y; i++) {         //게임판에 게임에 사용되지 않는 숫자를 넣음 
      for (j = 0; j < MAIN_X; j++) {  //이는 main_org와 같은 숫자가 없게 하기 위함 
         main_cpy[i][j] = 100;
      }
   }
}


int rand_block(void) {
   int first_block[7];
   int second_block[7];

   for (int i = 0; i < 7; i++) {
      int random = rand() % 7;
      _sleep(10);
      first_block[i] = random;

      if (i > 0) {
         for (int j = 0; j < i; j++) {
            if (first_block[i] == first_block[j]) {
               i--;
            }
         }
      }
   }
   for (int i = 0; i < 7; i++) {
      int random = rand() % 7;
      second_block[i] = random;

      if (i > 0) {
         for (int j = 0; j < i; j++) {
            if (second_block[i] == second_block[j]) {
               i--;
            }
         }
      }
   }
   for (int i = 0; i < 7; i++)
      return first_block[i];
}
void new_block() {
   bx = MAIN_X / 2 + 1;
   by = 0;
   b_type = b_type_next;
   b_type_next = rand_block();
   b_rotation = 0;
   new_block_on = 0;  //블럭 flag 끔                                             
   for (int i = 0; i < 4; i++) { //게임판 bx, by위치에 블럭생성                         
      for (int j = 0; j < 4; j++) {
         if (block[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = ACTIVE_BLOCK;
      }
   }
   for (int i = 1; i < 3; i++) { //게임상태표시에 다음에 나올블럭을 그림                     
      for (int j = 0; j < 4; j++) {
         if (block[b_type_next][0][i][j] == 1) {
            gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
            printf("■");
         }
         else {
            gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
            printf("  ");
         }
      }
   }
}

void block_move(int key) {
   int i, j;

   switch (key) {
   case RIGHT:
      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            if(block[b_type][b_rotation][i][j]==1)
            main_org[by+i][bx + j] = EMPTY;
         }
      }

      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            if (block[b_type][b_rotation][i][j] == 1)
               main_org[by + i][bx + j + 1] = ACTIVE_BLOCK;
         }
      }
      bx++;
      break;
   case LEFT:
      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            if (block[b_type][b_rotation][i][j] == 1)
               main_org[by + i][bx + j] = EMPTY;
         }
      }

      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            if (block[b_type][b_rotation][i][j] == 1)
               main_org[by + i][bx + j - 1] = ACTIVE_BLOCK;
         }
      }
      bx--;
      break;
   case DOWN:
      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            if (block[b_type][b_rotation][i][j] == 1)
               main_org[by + i][bx + j] = EMPTY;
         }
      }

      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            if (block[b_type][b_rotation][i][j] == 1)
               main_org[by + i + 1][bx + j] = ACTIVE_BLOCK;
         }
      }
      by++;
      break;

   case X: //우로돌리기
      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            if (block[b_type][b_rotation][i][j] == 1)
               main_org[by + i][bx + j] = EMPTY;
         }
      }


      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            if (block[b_type][b_rotation][i][j] == 1)
               main_org[by + i][bx + j] = ACTIVE_BLOCK;
         }
      }
      break;
   case Z:  //좌로돌리기
      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            main_org[by][bx] = EMPTY;
         }

      }


      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            main_org[by][bx] = ACTIVE_BLOCK;
         }
      }
      break;
   }
}
