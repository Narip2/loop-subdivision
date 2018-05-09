#include "stdafx.h"
#include <gl/glut.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <map>
#define pi 3.14159265
#define maxn 30000
using namespace std;


//Define HE structure
typedef struct HE_vert
{
	GLfloat x;
	GLfloat y;
	GLfloat z;

	struct HE_edge *edge;// one of the half-edges emantating from the vertex
}HE_vert;
typedef struct HE_edge
{
	struct HE_vert *vert;// vertex at the end of the half-edge
	struct HE_edge *pair;
	struct HE_face *face;
	struct HE_edge *next;
	int flag = 0;//用于后面判断边是否用过
}HE_edge;
typedef struct HE_face
{
	struct HE_edge *edge;// one of the half-edges bordering the face
}HE_face;



//To hold the vertex
//用于存储点
//读取第一次的点和平面
int ver, pla,edge;
GLfloat bigz = 0;
int flag = 0;
int tip = 0;
GLfloat norx, nory, norz;
int cnt;
//HE_vert *v[3485];
//HE_face *f[6966];
//HE_edge *e[6966][3];


vector<HE_vert*> v;
vector<HE_face*> f;
vector<vector<HE_edge*> > e;



void readoff(string fname)
{
	map<pair<HE_vert*, HE_vert*>, HE_edge*> edges;
	pair<HE_vert*, HE_vert*> p;
	string off;
	//读取一个平面有多少个顶点
	GLfloat num;
	int points[3];
	//用于存储面的顶点
	ifstream inFile;
	inFile.open(fname);
	//Check Err

	if (inFile.fail())
	{
		cerr << "Open file failed!!" << endl;
		exit(1);
	}
	inFile >> off;
	inFile >> ver;
	inFile >> pla;
	inFile >> off;
	cnt = ver;//数组下标从这之后都是新的顶点
	for (int i = 0; i < ver; i++)
	{
		HE_vert *tpv = new HE_vert;
		v.push_back(tpv);
	}
	for (int i = 0; i < pla; i++)
	{
		vector<HE_edge*> tpe2;
		HE_face *tpf = new HE_face;
		f.push_back(tpf);
		e.push_back(tpe2);
		for (int j = 0; j < 3; j++)
		{
			HE_edge *tpe1 = new HE_edge;
			e[i].push_back(tpe1);
		}
	}
	//Define some HE structures
	//for (int i = 0; i < ver; i++)
	//{
	//	v[i] = (HE_vert*)malloc(sizeof(HE_vert));
	//}
	//for (int i = 0; i < pla; i++)
	//{
	//	f[i] = (HE_face*)malloc(sizeof(HE_face));
	//}
	//for (int i = 0; i < pla; i++)
	//{
	//	for (int j = 0; j < 3; j++)
	//	{
	//		e[i][j] = (HE_edge*)malloc(sizeof(HE_edge));
	//	}
	//}


	//Get all the vertex
	for (int i = 0; i < ver; i++)
	{
		inFile >> v[i]->x;
		inFile >> v[i]->y;
		inFile >> v[i]->z;
		//计算中心
		norx += v[i]->x / ver;
		nory += v[i]->y / ver;
		norz += v[i]->z / ver;
	}
	for (int i = 0; i < pla; i++)
	{
		//Get number of points in face(useless for 3)
		inFile >> num;
		f[i]->edge = e[i][0];
		e[i][0]->next = e[i][1];
		e[i][1]->next = e[i][2];
		e[i][2]->next = e[i][0];
		for (int j = 0; j < 3; j++)
		{
			inFile >> points[j];	
			e[i][j]->vert = v[points[j]];
			e[i][j]->face = f[i];
			v[points[j]]->edge = e[i][j]->next;
		}
		p.first = v[points[2]];
		p.second = v[points[0]];
		edges[p] = e[i][0];
		p.first = v[points[0]];
		p.second = v[points[1]];
		edges[p] = e[i][1];
		p.first = v[points[1]];
		p.second = v[points[2]];
		edges[p] = e[i][2];
	}
	for (int i = 0; i < pla; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			p.first = e[i][j]->vert;
			p.second = e[i][j]->next->next->vert;
			e[i][j]->pair = edges[p];
		}
	}
	inFile.close();
}
int y_up = 1;
GLfloat px = 0, py = 0, xx = 0, yy = 0, s = 1;
void renderWindow()
{
	glClear(GL_COLOR_BUFFER_BIT);                           //当前背景色填充窗口
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective                            // 设置透视图
	(60,                            // 透视角设置为 60 度,在Y方向上以角度为单位的视野
		1.0,    // 窗口的宽与高比
		0.01f,                                // 视野透视深度
		1000.0f                            // 视野透视深度
	);
	gluLookAt(0, 0, 4, 0, 0, 0, 0.0, 1, 0.0);
	glPushMatrix();
	glTranslatef(xx, yy, 0);
	glScalef(s, s, s);
	glRotatef(py, 1, 0, 0);
	glRotatef(px, 0, 1, 0);
	//进行细化操作
	vector<HE_edge*> contain;
	HE_edge *tpe = new HE_edge;
	for (int i = 0; i < 3; i++)
	{
		contain.push_back(tpe);
	}
	if (tip == 1)
	{
		//首先enum每个面，对每个面的边进行加点
		int temp = pla;
		for (int i = 0; i < temp; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				HE_face *tpf = new HE_face;
				f.push_back(tpf);
				vector<HE_edge*> tpe2;
				e.push_back(tpe2);
				pla++;
				for (int k = 0; k < 3; k++)
				{
					HE_edge *tpe = new HE_edge;
					e[pla - 1].push_back(tpe);
				}
				if (e[i][j]->pair->flag == 0)
				{
					HE_vert *tpv = new HE_vert;
					HE_edge *tpe = new HE_edge;
					tpe->flag = 1;
					tpv->x = 0.375*(e[i][j]->vert->x + e[i][j]->pair->vert->x) + 0.125*(e[i][j]->next->vert->x + e[i][j]->pair->next->vert->x);
					tpv->y = 0.375*(e[i][j]->vert->y + e[i][j]->pair->vert->y) + 0.125*(e[i][j]->next->vert->y + e[i][j]->pair->next->vert->y);
					tpv->z = 0.375*(e[i][j]->vert->z + e[i][j]->pair->vert->z) + 0.125*(e[i][j]->next->vert->z + e[i][j]->pair->next->vert->z);
					v.push_back(tpv);
					tpe->vert = tpv;
					e[i][j]->pair->pair = tpe;
					contain[j] = tpe;
					e[i][j]->flag = 1;
				}
				else
				{
					HE_edge *tpe = new HE_edge;
					tpe->flag = 1;
					//这一步很重要！！！！！！！！！！！！！！！！？？？？
					tpe->vert = e[i][j]->pair->vert;
					contain[j] = tpe;
					e[i][j]->flag = 1;
				}
			}
			e[pla - 1][0]->vert = e[i][0]->vert;
			e[pla - 1][1]->vert = contain[1]->vert;
			e[pla - 1][2]->vert = contain[0]->vert;
			e[pla - 2][0]->vert = e[i][1]->vert;
			e[pla - 2][1]->vert = contain[2]->vert;
			e[pla - 2][2]->vert = contain[1]->vert;
			e[pla - 3][0]->vert = e[i][2]->vert;
			e[pla - 3][1]->vert = contain[0]->vert;
			e[pla - 3][2]->vert = contain[2]->vert;
			for (int j = 0; j < 3; j++)
			{
				e[i][j] = contain[j];
			}
		}
		//对改变之后的结构重新定义相邻关系
		map<pair<HE_vert*, HE_vert*>, HE_edge*> edges;
		pair<HE_vert*, HE_vert*> p;
		for (int i = 0; i < e.size(); i++)
		{
			f[i]->edge = e[i][0];
			e[i][0]->next = e[i][1];
			e[i][1]->next = e[i][2];
			e[i][2]->next = e[i][0];
			for (int j = 0; j < 3; j++) 
			{
				//重新定义flag
				e[i][j]->flag = 0;
				e[i][j]->face = f[i];
				e[i][j]->vert->edge = e[i][j]->next;
			}
			p.first = e[i][2]->vert;
			p.second = e[i][0]->vert;
			edges[p] = e[i][0];
			p.first = e[i][0]->vert;
			p.second = e[i][1]->vert;
			edges[p] = e[i][1];
			p.first = e[i][1]->vert;
			p.second = e[i][2]->vert;
			edges[p] = e[i][2];
		}
		for (int i = 0; i < e.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				p.first = e[i][j]->vert;
				p.second = e[i][j]->next->next->vert;
				e[i][j]->pair = edges[p];
			}
		}


		//然后对原来的点的坐标进行修改
		GLfloat b;
		for (int i = 0; i < cnt; i++)
		{
			vector<HE_vert*> tpv;
			HE_edge *temp = new HE_edge;
			temp = v[i]->edge;
			do
			{
				tpv.push_back(temp->vert);
				temp = temp->pair->next;
			} while (temp != v[i]->edge);
			b = (GLfloat)1 / tpv.size()*(0.625 - (0.375 + 0.25*cos((GLfloat)2 * pi / tpv.size()))*(0.375 + 0.25*cos((GLfloat)2 * pi / tpv.size())));
			v[i]->x = (1 - tpv.size()*b)*v[i]->x;
			v[i]->y	= (1 - tpv.size()*b)*v[i]->y;
			v[i]->z = (1 - tpv.size()*b)*v[i]->z;
			for (int j = 0; j < tpv.size(); j++)
			{
				v[i]->x += b * tpv[j]->x;
				v[i]->y += b * tpv[j]->y;
				v[i]->z += b * tpv[j]->z;
			}
		}
		cnt = v.size();
		tip = 0;
	}
	if (flag == 1 || flag == 0)
	{
		HE_edge *temp;
		for (int i = 0; i < pla; i++)
		{
			glBegin(GL_POLYGON);
			glColor3f(0.5, 0.5, 0);
			temp = f[i]->edge;
			do
			{
				glVertex3f(temp->vert->x - norx, temp->vert->y - nory, temp->vert->z - norz);
				temp = temp->next;
			} while (temp != f[i]->edge);
			glEnd();
		}
	}
	if (flag == 2 || flag == 0)
	{
		glPointSize(3.5f);
		glBegin(GL_POINTS);
		glColor3f(0, 0, 1.0);
		for (int j = 0; j < v.size(); j++)
		{
			glVertex3f(v[j]->x - norx, v[j]->y - nory, v[j]->z - norz);
		}
		glEnd();
	}
	if (flag == 3 || flag == 0)
	{
		glBegin(GL_LINES);
		glColor3f(0, 0.5, 0);
		HE_edge *temp;
		for (int i = 0; i < e.size(); i++)
		{
			temp = f[i]->edge;
			do
			{
				glVertex3f(temp->pair->vert->x - norx, temp->pair->vert->y - nory, temp->pair->vert->z - norz);
				glVertex3f(temp->vert->x - norx, temp->vert->y - nory, temp->vert->z - norz);
				temp = temp->next;
			} while (temp != f[i]->edge);
		}
		glEnd();
	}
	glPopMatrix();
	glutSwapBuffers();
}


int orix, oriy;
void mouseClick(int btn, int state, int x, int y)
{
	orix = x;
	oriy = y;
}
void mouseMove(int x, int y)
{
	if (cos(pi*(py / 180)) > 0)
	{
		px += (GLfloat)(x - orix) / 5;
	}
	else
	{
		px -= (GLfloat)(x - orix) / 5;
	}
	py += (GLfloat)(y - oriy) / 5;
	orix = x;
	oriy = y;
	renderWindow();
}
void keyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		flag = 1;
	}
	else if (key == GLUT_KEY_F2)
	{
		flag = 2;
	}
	else if (key == GLUT_KEY_F3)
	{
		flag = 3;
	}
	else if (key == GLUT_KEY_F4)
	{
		flag = 0;
	}
	else if (key == GLUT_KEY_PAGE_DOWN)
	{
		s = s / 1.1;
	}
	else if (key == GLUT_KEY_PAGE_UP)
	{
		s = s * 1.1;
	}
	else if (key == GLUT_KEY_UP)
	{
		yy += 0.01;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		yy -= 0.01;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		xx -= 0.01;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		xx += 0.01;
	}
	else if (key == GLUT_KEY_F5)
	{
		tip = 1;
	}
	renderWindow();
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);                                  //初始化glut: 接收主函数的参数
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);            //显示模式：颜色&缓冲
	glutInitWindowPosition(0, 0);                           //窗口相对屏幕位置
	glutInitWindowSize(720, 720);                           //窗口大小
	readoff("E://Study//computer graphic//off//test.off");//此处是自己off文件的位置
	glutCreateWindow("Hello, OpenGL!");                     //创建窗口: 标题
	glutDisplayFunc(renderWindow);                         //显示函数
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutSpecialFunc(keyboard);
	/*glutReshapeFunc(reshape);*/
	glutMainLoop();                                         //循环
	return 0;
}
