
#include <curses.h>             //ncurses头文件
#include <unistd.h>             //sleep头文件
#include <stdlib.h>             
#include <pthread.h>            //多线程头文件

struct Snake{                   //用结构体来表示每个节点
	int hang;
	int lie;
	struct Snake *next;
};

struct Snake food;
struct Snake *head = NULL;             
struct Snake *tail = NULL;
int dir;
int score = 0;
#define UP 1
#define DOWN -1
#define RIGHT 2
#define LEFT -2

void initNcurses(){             //初始化ncurses界面
	initscr();
	noecho();
	keypad(stdscr,1);       //键盘输入标准
}

void initFood(){

	int x = rand()%30;
	int y = rand()%40;
	if(x == 0){
		x = x+1;
	}
	if(x == 30){
		x = x-1;
	}
	if(y == 0){
		y = y+1;
	}
	if(y == 40){
		y = y-1;
	}
	food.hang = x;
	food.lie = y;
}

int hasFood(int i,int j){
	if(food.hang == i && food.lie == j){
		return 1;
	}
	return 0;
}

int snakeNode(int i,int j){     //判断遍历到的行列是否为节点行列
	struct Snake *p;
	p = head;
	while(p != NULL){
		if(p->hang ==i&&p->lie == j){
			return 1;
		}
		p = p->next;
	}
	return 0;
}

void addNode(){                  //尾部加节点
	struct Snake *new = (struct Snake *)malloc(sizeof(struct Snake));
	switch(dir){
		case UP:
			new->hang = tail->hang-1;
			new->lie = tail->lie;
			break;
		case DOWN:
			new->hang = tail->hang+1;
			new->lie = tail->lie;
			break;
		case LEFT:
			new->hang = tail->hang;
			new->lie = tail->lie-1;
			break;
		case RIGHT:
			new->hang = tail->hang;
			new->lie = tail->lie+1;
			break;
	}
	new->next = NULL;
	tail->next = new;
	tail = new;
}

void initSnake(){                //初始化头节点
	struct Snake *p;
	dir = RIGHT;
	while(head != NULL){
		p = head;
		head = head->next;
		free(p);
	}
	initFood();
	head = (struct Snake *)malloc(sizeof(struct Snake));
	head->hang = 1;
	head->lie = 1;
	head->next = NULL;
	tail = head;

	addNode();
	addNode();
}

void gamePic(){                  //显示界面
	int hang;
	int lie;
	move(0,0);               //光标坐标

	for(hang = 0;hang<=30;hang++){
		if(hang ==0){
			for(lie = 0;lie<40;lie++){
				printw("--");
			}
			printw("\n");
		}
		else if(hang >= 0 && hang < 30){
			for(lie = 0;lie <= 40;lie++){
				if(lie == 0 || lie == 40){
					printw("|");
				}
				else if(snakeNode(hang,lie)){
					printw("[]");
				}
				else if(hasFood(hang,lie)){     
					printw("##");
				}
				else{
					printw("  ");
				}
			}
			printw("\n");
		}
		else{
			for(lie = 0;lie<40;lie++){
				printw("--");
			}
		}
	}
	printw("\nSNAKE GAME");
	printw("\nSCORE = %d\n",score);
}
void deleNode(){                                                       //从头部删除节点
	struct Snake *p;
	p = head;
	head = head->next;
	free(p);
}

int ifSnakeDie(){
	struct Snake *p;
	p = head;
	if(tail->hang == 0 || tail->lie == 0 || tail->hang == 30 || tail->lie == 40){
		return 1;
	}
	while(p->next != NULL){
		if(p->hang == tail->hang && p->lie == tail->lie){
			return 1;
		}
		p = p->next;
	}
	return 0;
}

void moveSnake(){                                                      //蛇向右移动
        if(tail->hang == food.hang && tail->lie == food.lie){
		score++;
		initFood();
	}else{
		deleNode();                                                    //头部删除节点
	}
	addNode();                                                     //尾部增加节点
	if(ifSnakeDie()){
		initSnake();
		score = 0;
	}
	gamePic();                                                     //刷新显示界面
}

void *refreshPic(){                                                    //向右自动移动线程
	while(1){
			moveSnake();
			refresh();
			usleep(100000);		                       //移动速度
	}
}

void turn(int direction){
	if(abs(dir) != abs(direction)){
		dir = direction;
	}
}

void *getDir(){                                                        //不断获取键盘输入线程
	int key;
	while(1){
		key = getch();
		switch(key){
			case KEY_DOWN:
				turn(DOWN);
				break;
			case KEY_UP:
				turn(UP);
				break;
			case KEY_LEFT:
				turn(LEFT);
				break;
			case KEY_RIGHT:
				turn(RIGHT);
				break;
		}
	}
}

int main()
{
	initNcurses();
	initSnake();
	gamePic();

	pthread_t t1;                                     //线程的定义
	pthread_t t2;
	pthread_create(&t1,NULL,refreshPic,NULL);         //线程的创建
	pthread_create(&t2,NULL,getDir,NULL);

	while(1){}
	return 0;
}
