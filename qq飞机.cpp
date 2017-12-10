#include <stdio.h>
#include <windows.h> // COORD的头文件 
#include <conio.h>  // kbhit()的头文件 
#include <time.h> // 随机数要用 
#define max 100000
typedef struct plane_node{
	int x;
	int y;
}plane; 
typedef struct bullet_node{
	int x;
	int y;
	int flag; // 出界判断 
	int parent; //是谁发出的子弹（敌机被打死的时候它所打出的子弹也要一起消失） 
}bullet; 
typedef struct enemy_node{
	int x;
	int y;
	int live;
}enemy;
enum {
	U = 1, D, L, R ,H, S
};// 上下左右  开火hit  静止STOP 
plane pla[6];
int gameover = 0; // 如果被击中，游戏就结束啦 
bullet bullets[max]; // 子弹数组 
bullet emy_bul[max]; // 敌人的子弹数组 
enemy emy[max][6]; //敌机数组  
int e_c = 0 ; // 敌机数量 
int d_c = 0; // 已经死去的敌军数数目 ， 二者乡间，则是实际存活的数量 
int c = 0; // 已经发射的子弹数量 
int c1 = 0; //敌人已经发射的子弹数量 
int score = 0; // 击败的敌人数目 
int status;// 状态 
int cheat_flag = 0; // 开挂标志 

int r = 3;// 控制敌人的速度 
int r1 = 8; // 子弹产生的速度 (太快了子弹太密集） 

//int f = 1; // 测试用 
//int f1 = 0;
int x_emy = 10; // 第一个敌机的位置（为什么每次都挤在一块的原因所在） 


void Pos (int x,int y) // 光标定位函数 
{
	COORD pos;
	pos.X = x ;
	pos.Y = y ;
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
} 
void HideCursor()  // 隐藏光标，不然一闪一闪看着贼难受 
{
 CONSOLE_CURSOR_INFO cursor_info = {1, 0}; 
 SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

// 放置飞机 
void myPlane(int x, int y)
{
	pla[0].x = x;
	pla[0].y = y;
	pla[1].x = x - 1;
	pla[1].y = y + 1;
	pla[2].x = x;
	pla[2].y = y + 1;
	pla[3].x = x + 2;
	pla[3].y = y + 1;
	pla[4].x = x;
	pla[4].y = y + 2;
	pla[5].x = x + 1;
	pla[5].y = y + 2;
}
//画飞机 
void drawPlane(plane pla[])
{
	int x, y;
	char *image[6] {"▲","#","■","#","^","^"};
	for (int i = 0; i < 6; i++)
	{
		x = pla[i].x;
		y = pla[i].y;
		Pos(x, y);
		printf("%s",image[i]);
	}
	HideCursor();  // 画完飞机之后隐藏光标 
} 

// 敌机 
void Enemy(int x, int y)  // ∨▼ 
{
	emy[e_c][0].x = x;
	emy[e_c][0].y = y;
	emy[e_c][0].live = 1;// 活着 
	emy[e_c][1].x = x - 1;
	emy[e_c][1].y = y - 1;
	emy[e_c][2].x = x;
	emy[e_c][2].y = y - 1;
	emy[e_c][3].x = x + 2;
	emy[e_c][3].y = y - 1;
	emy[e_c][4].x = x;
	emy[e_c][4].y = y - 2;
	emy[e_c][5].x = x + 1;
	emy[e_c][5].y = y - 2;
	e_c++;
	
}
//类似子弹的消失法子 
void clear_enemy()
{
	for (int i = 0; i < e_c; i++)
	{
		int x, y;
		for (int j = 0; j < 6; j++)
		{
			x = emy[i][j].x;
			y = emy[i][j].y;
			Pos(x, y);
			printf(" ");
		}
	}
}
// 以下是对敌军子弹的操作，和对我方子弹的操作的基本相同 
void emyBulletClear()
{
	for (int i = 0; i < c1; i++)
	{
		int x = emy_bul[i].x;
		int y = emy_bul[i].y;
		Pos(x , y);
		printf(" ");
	}
}
void emyBulletMove()
{ 
	emyBulletClear();
	for (int i = 0; i < c1; i++)
	{
		if (emy_bul[i].y > 25)  //出界判断 
			emy_bul[i].flag = 0;  // 出界标志量置0 
		if (emy_bul[i].flag == 1 )
			emy_bul[i].y = emy_bul[i].y + 1;  // 天哪这个bug。。。我把emy_bul写成 bullets[]。。。。 
 	}
}
void drawEmyBullet()
{
	for (int i = 0; i < c1; i++)
	{
		if (emy_bul[i].flag == 1)  // 未出界的才画出来 
		{
			int x = emy_bul[i].x;
			int y = emy_bul[i].y;
			Pos(x , y);
			printf("°");
		} 	
	}
} 
// 攻击敌人代码 
void attack()    // 四重循环我的天...辛苦了我的机器... 
{
	for (int i = 0; i < c; i++)
	{
		if (bullets[i].flag == 1)
		{
			for (int j = 0; j < e_c; j++)
			{
				if (emy[j][0].live == 1)
				{
					for (int k = 0; k < 6; k++)
					{		
						if (bullets[i].x == emy[j][k].x && bullets[i].y == emy[j][k].y)
						{
							bullets[i].flag = 0;
							emy[j][0].live = 0;
							score += 10; // 分数加一 
							d_c++;//敌人加一 
							for (int m = 0; m < c1; m++)   //遍历所有敌人发射的子弹，如果是母体发的就让他消失 
							{
								if (emy_bul[m].parent == j)
									emy_bul[m].flag = 0;
							} 
							break;
						}
					}
				}
				
			}
		}
		
	}
} 
// 被攻击
void attacked()
{
	for (int i = 0; i < c1; i++)
	{
		if (emy_bul[i].flag == 1) //这也是个坑，没有这步的话会莫名其妙死亡.. 
		{
			for (int j = 0; j < 6; j++)
			{
				if (emy_bul[i].x == pla[j].x && emy_bul[i].y ==  pla[j].y || emy_bul[i].x == pla[0].x + 1 && emy_bul[i].y == pla[0].y ) // 第二个判断条件是因为飞机头（那个三角形占两格）
				{
					gameover = 1; // 被打中就代表结束了 
				} 
			}
		} 
		
	} 
} 
// 敌人动 
void enemyMove()
{
	clear_enemy(); //动之前先清理哦 
	for (int i = 0; i < e_c ; i++)
	{
		if (emy[i][0].live == 1)// 死亡了的则不处理 
		{
			if (r1 % 8 == 0 && emy[i][0].y <= 25 )   // 至于是 25 26 27 要慢慢试得去 ，处理不好的话会穿墙哦 
			{
				bullet b ;     // 产生子弹 
				b.x = emy[i][0].x ;
				b.y = emy[i][0].y + 2;
				b.flag = 1;
				b.parent = i; //记录其母体 
				emy_bul[c1++] = b;
				r1 = 1;
			} 
			else 
				r1++;
			
			if (r % 3 == 0) // 三个循环执行一次 
			{
				for (int j = 0; j < 6; j++)
				{
					emy[i][j].y += 1;
				}
				r = 1;
			}
			else
				r++;
				
		}
		
	}
}
// 画敌机 
void drawEnemy()
{ 
	for (int i = 0; i < e_c ; i++)
	{
		if (emy[i][0].live == 1) // 只画还活着的 
		{
			int x, y;
			char *image[6] {"▼","#","■","#","v","v"};
			for (int j = 0; j < 6; j++)
			{
				if (emy[i][4].y > 24)
				{
					emy[i][0].live = 0; // 飞机完全出界则死亡 
					d_c++; // 死亡数量加一 
					break;
				}
				x = emy[i][j].x;
				y = emy[i][j].y;
				Pos(x, y);
				if (y > 27)  // 超出的部分不显示 
					; 
				else
					printf("%s",image[j]);
			}
		}
	}
}
 
void clear(plane pla[]) // move之前要把原来的痕迹清除 
{
	int x, y;
	for (int i = 0; i < 6; i++)
	{
		x = pla[i].x;
		y = pla[i].y;
		Pos(x, y);
		printf(" ");
	}
} 
//展示一些信息(蛤值，操作）
void showMsg()
{
	char *a[7] = {"苟","利","国","家","生","死","以"};
	char *b[7] = {"岂","因","祸","福","避","趋","之"}; 
	int start =  2;
	for (int i = 0; i < 7; i++)
	{
		Pos(63, start + i);
		printf("%s\n",a[i]);
	} 
	start =  2;
	for (int i = 0; i < 7; i++)
	{
		Pos(90, start + i);
		printf("%s\n",b[i]);
	}
	Pos(74, 5);
	printf("蛤值：%d", score); 
	Pos(63, 12); 
	printf("操作提示：");
	Pos(63,14);
	printf("w/W : 上        s/S ：下");
	Pos(63,15);
	printf("a/A : 左        d/D : 右"); 
	Pos(63,17);
	printf("j/J : 开火");
	Pos(63,18);
	printf("k/K : 开挂"); 
	if (cheat_flag == 1)
	{
		Pos(80, 18);
		printf("卢本伟牛逼！");
	}
	
	Pos(63, 20);
	printf("名人语录：");
	Pos(63,22);
	printf("1.失败是成功之母,是骗人的。");
	Pos(63,23);
	printf("2.失败乃兵家常事。");
	Pos(63,24);
	printf("3.输了这场游戏，你就输了整个世界") ;
	Pos(63,25);
	printf("4.从哪里跌倒，就不要站起来了") ;
	Pos(63,26);
	printf("5.看着我的眼睛，相信我，你不会赢的。");	 
} 
void bulletClear()  //同样要把之前的子弹清除 
{
	for (int i = 0; i < c; i++)
	{
		int x = bullets[i].x;
		int y = bullets[i].y;
		Pos(x , y);
		printf(" ");
	}
}
void bulletMove()
{
	bulletClear(); // 更新之前先清理之前的 
	for (int i = 0; i < c; i++)
	{
		if (bullets[i].y <= 1)  //出界判断 
			bullets[i].flag = 0;  // 出界标志量置0 
		if (bullets[i].flag == 1)
			bullets[i].y = bullets[i].y - 1;
	}
}
//开挂 
void cheat()
{
	for (int i = 2; i < 56; i+=2)
	{
		bullet b ;
		b.x = i;
		b.y = pla[0].y + 1;
		b.flag = 1;
		bullets[c++] = b;
	}
} 
void drawBullet()
{
	for (int i = 0; i < c; i++)
	{
		if (bullets[i].flag == 1)  // 未出界的才画出来 
		{
			int x = bullets[i].x;
			int y = bullets[i].y;
			Pos(x , y);
			printf("°");
		} 	
	}
}



void direction()
{
//	char dtn = getchar();
	char dtn = '#'; // 初始化一下，不然vs上编译不过 
	if (kbhit())//非阻塞响应 
		dtn = getch();
	switch (dtn)
	{
		case 'w' :
		case 'W' : status = U; break;
		case 's' :
		case 'S' : status = D; break;
		case 'a' :
		case 'A' : status = L; break;
		case 'd' : 
		case 'D' : status = R; break;
		case 'j' :
		case 'J' : status = H; break;	
		case 'k' :
		case 'K' : cheat(); cheat_flag = 1; break;
	} 
}

//背景 
void background()    // 边界  上 0 下27 左0  右60 
{ 
	for (int i = 0; i < 60; i +=2)
	{
		printf("■");
	}
	printf("\n");
	for (int i = 0; i < 27; i++)
	{
		printf("■");
		for (int j = 0; j < 56; j++)
			printf(" ");
		printf("■\n");
	}
	for (int i = 0; i < 60; i +=2)
	{
		printf("■");
	}
} 
// 碰撞检测 
void colide()
{
	for(int i = 0; i < 6; i++)
	{
		for (int j = 0; j < e_c; j++)
		{
			if (emy[j][0].live == 1)
			{
				for (int k = 0; k < 6; k++)
				{
					if (pla[i].x == emy[j][k].x && pla[i].y == emy[j][k].y || pla[0].x + 1 == emy[j][k].x && pla[0].y == emy[j][k].y)
					{
						gameover = 1; //相撞了就over哦 
					}
				}
			} 	
		}
	}
}
// 游戏判断 
void judge()
{
	if (gameover == 1)
	{
		
		system("cls");
		background(); 
		Pos(10,12);
		printf("亲，您挂了哦~, 您一共打落了%d架飞机哦",score / 10); 
		score = 0; // 数据恢复 
		gameover = 0; 
		Pos(0, 29 );
	}
	if (score >= 100)
	{
		system("cls");
		background();
		Pos(20, 12);
		printf("欧耶，您胜利啦！"); 
		score = 0; // 数据恢复，以免影响下一局 
		gameover = 0;
		Pos(0,29);
	}
} 
void move ()  //  
{
	while (1)
	{
		direction();
//		judge(); 放在这里不好
		showMsg();
		if (gameover == 1 || score >= 100)
		{
			Sleep(300);  // 缓一缓 
			break;
		}
			
		if (status == U && pla[0].y > 1)  // 增加了出界判断 
		{
			clear(pla); // 处理之前的痕迹 
			
			for (int i = 0; i < 6; i++)
			{
				pla[i].y = pla[i].y - 1;
			}
		}
		else if (status == D && pla[4].y < 27)
		{
			clear(pla);
			for (int i = 0; i < 6; i++)
			{
				pla[i].y = pla[i].y + 1;
			}
		}
		else if (status == R && pla[3].x <= 56)
		{
			clear(pla);
			for (int i = 0; i < 6; i++)
			{
				pla[i].x = pla[i].x + 1;
			}
		}
		else if (status == L && pla[1].x > 2)
		{
			clear(pla); 
			for (int i = 0; i < 6; i++)
			{
				pla[i].x = pla[i].x - 1;
			}
		}
		else if (status == H)
		{
			bullet b;
			b.x = pla[0].x;
			b.y = pla[0].y;
			b.flag = 1; 
			bullets[c++] = b; 
		}
		
		status = S; // 处理完上一次的之后状态恢复为静止
		
		// 对子弹的处理 
		bulletMove();
		drawBullet();
		
		//碰撞检测
		colide();
		 
		//对敌机的处理
		srand((unsigned)time(NULL));
		
		if (e_c  - d_c < 5)  // 随机产生敌人，存活的不超过5个 
		{
			int temp = rand() % 54 + 3;
			if (temp - x_emy > 4 || temp - x_emy < -4)  // 和前一个产生的敌人距离超过4 
			{
			//	printf("%d %d",x_emy, temp); 测试 
				x_emy = temp;
				if (x_emy >= 2 && x_emy <= 55)  // 保证产生的敌机不出界 
					Enemy(x_emy, 3);
			}

		}  
		
		 
//		if (f1 == 1) // 测试 
//		{
//			Enemy(rand()% 54 + 2, 4);
//			f1 = 0;
//		}
//		if (f == 1)
//		{
//			Enemy(rand()% 54 - 10, 4);
//			f = 0;
//			f1 = 1;
//		}
		//攻击敌人
		attack();
		//被攻击 
		attacked(); 
		//敌人动 
		enemyMove(); 
		drawEnemy(); 
		
		emyBulletMove(); 
		drawEmyBullet();
		
		drawPlane(pla);
		
		Sleep(100); // 控制整体速度 
	}
	
} 

//飞机出界判断 
//int isOut()   写成函数的话会一帮子打死，无法判断是哪部分出界 
//{
//	if (pla[0].y <= 1)
//		return 0;
//	if (pla[1].x <= 2)
//		return 0;
//	if (pla[3].y >= 58)
//		return 0;
//	if (pla[4].y >= 26) 
//		return 0;
//	return 1;  
//} 
// 菜单界面 
void menu()
{
	background();
	Pos(26,8);
	printf("QQ飞机\n");
	Pos(23,10);
	printf("用心创造快乐");
	Pos(4, 15);
	printf("1.和微信好友玩      2.和QQ好友玩       3.我一个人玩\n") ; 
	Pos(23,20);
	while (1)
	{
		char choose = '#';
		HideCursor();
		choose = getch();// 不用按回车 
		
		switch (choose)
		{
			case '1' : {
				system("cls");
				Pos(25,15);
				printf("嘿嘿嘿...骗你的啦...（不要打我..）\n");
				Pos(2,28);
				system("pause");
				system("cls");
				menu();
				break;
			}
			case '2' : {
				system("cls");
				Pos(25,15);
				printf("我也想有这个功能的orz...\n");
				Pos(2,28);
				system("pause");
				system("cls");
				menu();
				break;
			}
			case '3': {
				system("cls");
				background();
	
				myPlane(20,25);
				drawPlane(pla);
				move();
				judge();
				system("pause");
				system("cls");
				menu();
				break;
			}
		}
	}

	 
}
int main ()
{
	menu();
//	background();
//
//	myPlane(20,20);
//	drawPlane(pla);
//	move();
//	judge();
	
	
	return 0;
}
