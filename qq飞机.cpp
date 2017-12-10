#include <stdio.h>
#include <windows.h> // COORD��ͷ�ļ� 
#include <conio.h>  // kbhit()��ͷ�ļ� 
#include <time.h> // �����Ҫ�� 
#define max 100000
typedef struct plane_node{
	int x;
	int y;
}plane; 
typedef struct bullet_node{
	int x;
	int y;
	int flag; // �����ж� 
	int parent; //��˭�������ӵ����л���������ʱ������������ӵ�ҲҪһ����ʧ�� 
}bullet; 
typedef struct enemy_node{
	int x;
	int y;
	int live;
}enemy;
enum {
	U = 1, D, L, R ,H, S
};// ��������  ����hit  ��ֹSTOP 
plane pla[6];
int gameover = 0; // ��������У���Ϸ�ͽ����� 
bullet bullets[max]; // �ӵ����� 
bullet emy_bul[max]; // ���˵��ӵ����� 
enemy emy[max][6]; //�л�����  
int e_c = 0 ; // �л����� 
int d_c = 0; // �Ѿ���ȥ�ĵо�����Ŀ �� ������䣬����ʵ�ʴ������� 
int c = 0; // �Ѿ�������ӵ����� 
int c1 = 0; //�����Ѿ�������ӵ����� 
int score = 0; // ���ܵĵ�����Ŀ 
int status;// ״̬ 
int cheat_flag = 0; // ���ұ�־ 

int r = 3;// ���Ƶ��˵��ٶ� 
int r1 = 8; // �ӵ��������ٶ� (̫�����ӵ�̫�ܼ��� 

//int f = 1; // ������ 
//int f1 = 0;
int x_emy = 10; // ��һ���л���λ�ã�Ϊʲôÿ�ζ�����һ���ԭ�����ڣ� 


void Pos (int x,int y) // ��궨λ���� 
{
	COORD pos;
	pos.X = x ;
	pos.Y = y ;
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
} 
void HideCursor()  // ���ع�꣬��Ȼһ��һ������������ 
{
 CONSOLE_CURSOR_INFO cursor_info = {1, 0}; 
 SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

// ���÷ɻ� 
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
//���ɻ� 
void drawPlane(plane pla[])
{
	int x, y;
	char *image[6] {"��","#","��","#","^","^"};
	for (int i = 0; i < 6; i++)
	{
		x = pla[i].x;
		y = pla[i].y;
		Pos(x, y);
		printf("%s",image[i]);
	}
	HideCursor();  // ����ɻ�֮�����ع�� 
} 

// �л� 
void Enemy(int x, int y)  // �Ũ� 
{
	emy[e_c][0].x = x;
	emy[e_c][0].y = y;
	emy[e_c][0].live = 1;// ���� 
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
//�����ӵ�����ʧ���� 
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
// �����ǶԵо��ӵ��Ĳ������Ͷ��ҷ��ӵ��Ĳ����Ļ�����ͬ 
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
		if (emy_bul[i].y > 25)  //�����ж� 
			emy_bul[i].flag = 0;  // �����־����0 
		if (emy_bul[i].flag == 1 )
			emy_bul[i].y = emy_bul[i].y + 1;  // �������bug�������Ұ�emy_bulд�� bullets[]�������� 
 	}
}
void drawEmyBullet()
{
	for (int i = 0; i < c1; i++)
	{
		if (emy_bul[i].flag == 1)  // δ����ĲŻ����� 
		{
			int x = emy_bul[i].x;
			int y = emy_bul[i].y;
			Pos(x , y);
			printf("��");
		} 	
	}
} 
// �������˴��� 
void attack()    // ����ѭ���ҵ���...�������ҵĻ���... 
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
							score += 10; // ������һ 
							d_c++;//���˼�һ 
							for (int m = 0; m < c1; m++)   //�������е��˷�����ӵ��������ĸ�巢�ľ�������ʧ 
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
// ������
void attacked()
{
	for (int i = 0; i < c1; i++)
	{
		if (emy_bul[i].flag == 1) //��Ҳ�Ǹ��ӣ�û���ⲽ�Ļ���Ī����������.. 
		{
			for (int j = 0; j < 6; j++)
			{
				if (emy_bul[i].x == pla[j].x && emy_bul[i].y ==  pla[j].y || emy_bul[i].x == pla[0].x + 1 && emy_bul[i].y == pla[0].y ) // �ڶ����ж���������Ϊ�ɻ�ͷ���Ǹ�������ռ����
				{
					gameover = 1; // �����оʹ�������� 
				} 
			}
		} 
		
	} 
} 
// ���˶� 
void enemyMove()
{
	clear_enemy(); //��֮ǰ������Ŷ 
	for (int i = 0; i < e_c ; i++)
	{
		if (emy[i][0].live == 1)// �����˵��򲻴��� 
		{
			if (r1 % 8 == 0 && emy[i][0].y <= 25 )   // ������ 25 26 27 Ҫ�����Ե�ȥ �������õĻ��ᴩǽŶ 
			{
				bullet b ;     // �����ӵ� 
				b.x = emy[i][0].x ;
				b.y = emy[i][0].y + 2;
				b.flag = 1;
				b.parent = i; //��¼��ĸ�� 
				emy_bul[c1++] = b;
				r1 = 1;
			} 
			else 
				r1++;
			
			if (r % 3 == 0) // ����ѭ��ִ��һ�� 
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
// ���л� 
void drawEnemy()
{ 
	for (int i = 0; i < e_c ; i++)
	{
		if (emy[i][0].live == 1) // ֻ�������ŵ� 
		{
			int x, y;
			char *image[6] {"��","#","��","#","v","v"};
			for (int j = 0; j < 6; j++)
			{
				if (emy[i][4].y > 24)
				{
					emy[i][0].live = 0; // �ɻ���ȫ���������� 
					d_c++; // ����������һ 
					break;
				}
				x = emy[i][j].x;
				y = emy[i][j].y;
				Pos(x, y);
				if (y > 27)  // �����Ĳ��ֲ���ʾ 
					; 
				else
					printf("%s",image[j]);
			}
		}
	}
}
 
void clear(plane pla[]) // move֮ǰҪ��ԭ���ĺۼ���� 
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
//չʾһЩ��Ϣ(��ֵ��������
void showMsg()
{
	char *a[7] = {"��","��","��","��","��","��","��"};
	char *b[7] = {"��","��","��","��","��","��","֮"}; 
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
	printf("��ֵ��%d", score); 
	Pos(63, 12); 
	printf("������ʾ��");
	Pos(63,14);
	printf("w/W : ��        s/S ����");
	Pos(63,15);
	printf("a/A : ��        d/D : ��"); 
	Pos(63,17);
	printf("j/J : ����");
	Pos(63,18);
	printf("k/K : ����"); 
	if (cheat_flag == 1)
	{
		Pos(80, 18);
		printf("¬��ΰţ�ƣ�");
	}
	
	Pos(63, 20);
	printf("������¼��");
	Pos(63,22);
	printf("1.ʧ���ǳɹ�֮ĸ,��ƭ�˵ġ�");
	Pos(63,23);
	printf("2.ʧ���˱��ҳ��¡�");
	Pos(63,24);
	printf("3.�����ⳡ��Ϸ�����������������") ;
	Pos(63,25);
	printf("4.������������Ͳ�Ҫվ������") ;
	Pos(63,26);
	printf("5.�����ҵ��۾��������ң��㲻��Ӯ�ġ�");	 
} 
void bulletClear()  //ͬ��Ҫ��֮ǰ���ӵ���� 
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
	bulletClear(); // ����֮ǰ������֮ǰ�� 
	for (int i = 0; i < c; i++)
	{
		if (bullets[i].y <= 1)  //�����ж� 
			bullets[i].flag = 0;  // �����־����0 
		if (bullets[i].flag == 1)
			bullets[i].y = bullets[i].y - 1;
	}
}
//���� 
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
		if (bullets[i].flag == 1)  // δ����ĲŻ����� 
		{
			int x = bullets[i].x;
			int y = bullets[i].y;
			Pos(x , y);
			printf("��");
		} 	
	}
}



void direction()
{
//	char dtn = getchar();
	char dtn = '#'; // ��ʼ��һ�£���Ȼvs�ϱ��벻�� 
	if (kbhit())//��������Ӧ 
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

//���� 
void background()    // �߽�  �� 0 ��27 ��0  ��60 
{ 
	for (int i = 0; i < 60; i +=2)
	{
		printf("��");
	}
	printf("\n");
	for (int i = 0; i < 27; i++)
	{
		printf("��");
		for (int j = 0; j < 56; j++)
			printf(" ");
		printf("��\n");
	}
	for (int i = 0; i < 60; i +=2)
	{
		printf("��");
	}
} 
// ��ײ��� 
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
						gameover = 1; //��ײ�˾�overŶ 
					}
				}
			} 	
		}
	}
}
// ��Ϸ�ж� 
void judge()
{
	if (gameover == 1)
	{
		
		system("cls");
		background(); 
		Pos(10,12);
		printf("�ף�������Ŷ~, ��һ��������%d�ܷɻ�Ŷ",score / 10); 
		score = 0; // ���ݻָ� 
		gameover = 0; 
		Pos(0, 29 );
	}
	if (score >= 100)
	{
		system("cls");
		background();
		Pos(20, 12);
		printf("ŷҮ����ʤ������"); 
		score = 0; // ���ݻָ�������Ӱ����һ�� 
		gameover = 0;
		Pos(0,29);
	}
} 
void move ()  //  
{
	while (1)
	{
		direction();
//		judge(); �������ﲻ��
		showMsg();
		if (gameover == 1 || score >= 100)
		{
			Sleep(300);  // ��һ�� 
			break;
		}
			
		if (status == U && pla[0].y > 1)  // �����˳����ж� 
		{
			clear(pla); // ����֮ǰ�ĺۼ� 
			
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
		
		status = S; // ��������һ�ε�֮��״̬�ָ�Ϊ��ֹ
		
		// ���ӵ��Ĵ��� 
		bulletMove();
		drawBullet();
		
		//��ײ���
		colide();
		 
		//�Եл��Ĵ���
		srand((unsigned)time(NULL));
		
		if (e_c  - d_c < 5)  // ����������ˣ����Ĳ�����5�� 
		{
			int temp = rand() % 54 + 3;
			if (temp - x_emy > 4 || temp - x_emy < -4)  // ��ǰһ�������ĵ��˾��볬��4 
			{
			//	printf("%d %d",x_emy, temp); ���� 
				x_emy = temp;
				if (x_emy >= 2 && x_emy <= 55)  // ��֤�����ĵл������� 
					Enemy(x_emy, 3);
			}

		}  
		
		 
//		if (f1 == 1) // ���� 
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
		//��������
		attack();
		//������ 
		attacked(); 
		//���˶� 
		enemyMove(); 
		drawEnemy(); 
		
		emyBulletMove(); 
		drawEmyBullet();
		
		drawPlane(pla);
		
		Sleep(100); // ���������ٶ� 
	}
	
} 

//�ɻ������ж� 
//int isOut()   д�ɺ����Ļ���һ���Ӵ������޷��ж����Ĳ��ֳ��� 
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
// �˵����� 
void menu()
{
	background();
	Pos(26,8);
	printf("QQ�ɻ�\n");
	Pos(23,10);
	printf("���Ĵ������");
	Pos(4, 15);
	printf("1.��΢�ź�����      2.��QQ������       3.��һ������\n") ; 
	Pos(23,20);
	while (1)
	{
		char choose = '#';
		HideCursor();
		choose = getch();// ���ð��س� 
		
		switch (choose)
		{
			case '1' : {
				system("cls");
				Pos(25,15);
				printf("�ٺٺ�...ƭ�����...����Ҫ����..��\n");
				Pos(2,28);
				system("pause");
				system("cls");
				menu();
				break;
			}
			case '2' : {
				system("cls");
				Pos(25,15);
				printf("��Ҳ����������ܵ�orz...\n");
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
