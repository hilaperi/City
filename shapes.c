#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include <math.h>
// size of picture in pixels
#define WIDTH 500
#define HEIGHT 350

typedef struct color
{
	int red, green, blue;
} COLOR;
typedef struct circle
{
	int x, y;
	int radius;
	COLOR rgb;
} CIRCLE;
typedef struct rct // rectangle
{
	int cx, cy; // center
	int width;
	int height;
	COLOR rgb;
} RCT;
typedef struct rctStrret // rectangle
{
	int cx, cy; // center
	int width;
	int height;
	COLOR rgb;
} RCTSTREET;
typedef struct nodeRCT
{
	RCT key;
	struct nodeRCT* next;
} NODERCT;
typedef struct nodeCRL
{
	CIRCLE key;
	struct nodeCRL* next;
} NODECRL;
typedef struct nodeSTR
{
	RCT key;
	struct nodeSTR* next;
} NODESTR;

RCT* ReadRcts(FILE* pf, int* pnum);
void WriteBitmap(char* fname, CIRCLE* pc, int num_circles, RCT* pr, int num_rcts, RCTSTREET* ps, int num_rctsStreets);
void AddRct(unsigned char* bmp, RCT rct);
void AddCircle(unsigned char* bmp, CIRCLE c);
NODERCT* BuildListForwardRCT(RCT rct[], int* numOfRCT);
NODESTR* BuildListForwardRCTSTREET(RCT rct[], int* numOfRCTStreet);
NODECRL* BuildListForwardCircle(CIRCLE rct[], int* numOfRCTCircle);
RCT* ReadRctsStreets(FILE* pf, int* pnum);
void AddRctStreets(unsigned char* bmp, RCTSTREET rct);
CIRCLE * addNodeCircle(RCTSTREET rct[], int *pnumStreet, int *pnumCircle);

void main()
{
	NODERCT* head = NULL;
	NODESTR* headStr = NULL;
	NODECRL* headCrl = NULL;
	CIRCLE* pc; // dynamic array of circles
	int num_circles;
	RCT* pr; // dynamic array of rectangles
	RCTSTREET* ps;
	int num_rcts;
	int num_rctsStreets;
	FILE* pf = fopen("shapes.txt", "r");// read text file

	if (pf == NULL)
	{
		printf("Can't open file %s\n", "shapes.txt");
		exit(1);
	}

	pr = ReadRcts(pf, &num_rcts);
	ps = ReadRctsStreets(pf, &num_rctsStreets);
	pc = addNodeCircle(ps, &num_rctsStreets,&num_circles);
	fclose(pf);
	head= BuildListForwardRCT(pr, &num_rcts);
	headStr = BuildListForwardRCTSTREET(ps, &num_rctsStreets);
	headCrl = BuildListForwardCircle(pc, &num_circles);
	WriteBitmap("pic.bmp", pc, num_circles, pr, num_rcts,ps,num_rctsStreets);
}

void WriteBitmap(char* fname, CIRCLE* pc, int num_circles, RCT* pr, int num_rcts, RCTSTREET* ps, int num_rctsStreets)
{
	FILE* pf = fopen(fname, "wb"); // write binary
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	unsigned char* bmp;
	int i, j, sz;
	int p = 0;
	sz = WIDTH*HEIGHT * 3;
	bmp = (unsigned char*)malloc(sz);

	// 1. define bf
	bf.bfType = 0x4d42; // magic number
	bf.bfSize = sz + sizeof(bf) + sizeof(bi); // file size in bytes
	bf.bfOffBits = sizeof(bf) + sizeof(bi); // offset before raw data
											// 2. define bi
	bi.biHeight = HEIGHT;
	bi.biWidth = WIDTH;
	bi.biSize = sizeof(bi);
	bi.biBitCount = 24; // 24 = 8*R+8*G+8*B
	bi.biCompression = BI_RGB;
	// 3. define bmp
	for (i = 0; i<HEIGHT; i++)
		for (j = 0; j < WIDTH; j++)
		{
			bmp[(i*WIDTH + j) * 3] = 80; // blue
			bmp[(i*WIDTH + j) * 3 + 1] = 208; // green
			bmp[(i*WIDTH + j) * 3 + 2] = 146; // red
		}

	// add all shapes
	for (i = 0; i < num_rcts; i++)
		AddRct(bmp, pr[i]);

	for (i = 0; i < num_rctsStreets; i++)
	AddRctStreets(bmp, ps[i]);


	for (i = 0; i <num_circles; i++)
		AddCircle(bmp, pc[i]);

	// 4. save the data to file
	fwrite(&bf, sizeof(bf), 1, pf);
	fwrite(&bi, sizeof(bi), 1, pf);
	fwrite(bmp, 1, sz, pf);

	fclose(pf);
	free(bmp);
}
void AddRct(unsigned char* bmp, RCT rct)
{
	int i, j, left, right, bottom, top;
	left = rct.cx - rct.width / 2;
	right = rct.cx + rct.width / 2;
	bottom = rct.cy - rct.height / 2;
	top = rct.cy + rct.height / 2;

	for (i = bottom; i<top; i++)
		for (j = left; j < right; j++)
		{
			bmp[(i*WIDTH + j) * 3] = rct.rgb.blue; // blue
			bmp[(i*WIDTH + j) * 3 + 1] = rct.rgb.green; // green
			bmp[(i*WIDTH + j) * 3 + 2] = rct.rgb.red; // red
		}
}
void AddRctStreets(unsigned char* bmp, RCTSTREET rct)
{
	int count = 0;
	int i, j, left, right, bottom, top;
	left = rct.cx - rct.width / 2;
	right = rct.cx + rct.width / 2;
	bottom = rct.cy - rct.height / 2;
	top = rct.cy + rct.height / 2;

	for (i = bottom; i<top; i++)
		for (j = left; j < right; j++)
		{	
			bmp[(i*WIDTH + j) * 3] = rct.rgb.blue; // blue
			bmp[(i*WIDTH + j) * 3 + 1] = rct.rgb.green; // green
			bmp[(i*WIDTH + j) * 3 + 2] = rct.rgb.red; // red
		}
}
void AddCircle(unsigned char* bmp, CIRCLE c)
{
	int i, j, left, right, bottom, top;
	double dist;		

	left = c.x - c.radius;
	right = c.x + c.radius;
	bottom = c.y - c.radius;
	top = c.y + c.radius;

	for (i = bottom; i<top; i++)
		for (j = left; j < right; j++)
		{
			dist = sqrt((i - c.y)*(i - c.y) + (j - c.x)*(j - c.x));
			if (dist < c.radius)
			{
				bmp[(i*WIDTH + j) * 3] = c.rgb.blue; // blue
				bmp[(i*WIDTH + j) * 3 + 1] = c.rgb.green; // green
				bmp[(i*WIDTH + j) * 3 + 2] = c.rgb.red; // red
			}
		}
}
RCT* ReadRcts(FILE* pf, int* pnum)
{
	RCT* pr = NULL;
	char kind[50];
	int tmp;
	*pnum = 0;
	rewind(pf); // go to the beginning of the file

	while (!feof(pf))
	{
		fscanf(pf, "%s", kind);
		if (strcmp(kind, "h") == 0) // rct
		{
			fscanf(pf, "%s", kind);
			if (strcmp(kind, "living") == 0) // rct
			{
				pr = (RCT*)realloc(pr, sizeof(RCT)*(*pnum + 1));
				fscanf(pf, "%d%d%d%d", &pr[*pnum].cx, &pr[*pnum].cy, &pr[*pnum].width, &pr[*pnum].height);
				pr[*pnum].rgb.red = 79;
				pr[*pnum].rgb.green = 118;
				pr[*pnum].rgb.blue = 249;
				(*pnum)++;
			}


			if (strcmp(kind, "industry") == 0) // rct
			{
				pr = (RCT*)realloc(pr, sizeof(RCT)*(*pnum + 1));
				fscanf(pf, "%d%d%d%d", &pr[*pnum].cx, &pr[*pnum].cy, &pr[*pnum].width, &pr[*pnum].height);
				pr[*pnum].rgb.red = 250;
				pr[*pnum].rgb.green = 192;
				pr[*pnum].rgb.blue = 144;
				(*pnum)++;
			}
			else if (strcmp(kind, "office") == 0) // rct
			{
				pr = (RCT*)realloc(pr, sizeof(RCT)*(*pnum + 1));
				fscanf(pf, "%d%d%d%d", &pr[*pnum].cx, &pr[*pnum].cy, &pr[*pnum].width, &pr[*pnum].height);
				pr[*pnum].rgb.red = 100;
				pr[*pnum].rgb.green = 100;
				pr[*pnum].rgb.blue = 100;
				(*pnum)++;
			}
			else if (strcmp(kind, "rent") == 0) // rct
			{
				pr = (RCT*)realloc(pr, sizeof(RCT)*(*pnum + 1));
				fscanf(pf, "%d%d%d%d", &pr[*pnum].cx, &pr[*pnum].cy, &pr[*pnum].width, &pr[*pnum].height);
				pr[*pnum].rgb.red = 200;
				pr[*pnum].rgb.green = 0;
				pr[*pnum].rgb.blue = 100;
				(*pnum)++;
			}
		
		}
		
	

		else 
				fscanf(pf, "%d%d%d%d", &tmp, &tmp, &tmp,
					&tmp); // or fgets
		}		
		return pr;
}
RCT* ReadRctsStreets(FILE* pf, int* pnum) {

	RCT* ps = NULL;
	char kind[50];
	int tmp;
	*pnum = 0;
	rewind(pf); // go to the beginning of the file

	while (!feof(pf))
	{
		fscanf(pf, "%s", kind);
		if (strcmp(kind, "s") == 0) // rct
		{

			ps = (RCTSTREET*)realloc(ps, sizeof(RCTSTREET)*(*pnum + 1));
			fscanf(pf, "%d%d%d%d", &ps[*pnum].cx, &ps[*pnum].cy, &ps[*pnum].width, &ps[*pnum].height);
			ps[*pnum].rgb.red = 150;
			ps[*pnum].rgb.green = 150;
			ps[*pnum].rgb.blue = 150;
			(*pnum)++;
		}

		else
			fscanf(pf, "%d%d%d%d%d", &tmp, &tmp, &tmp,
				&tmp, &tmp); // or fgets
	}
	return ps;
}
CIRCLE * addNodeCircle(RCTSTREET rct[], int *pnumStreet, int *pnumCircle) {
	CIRCLE* pc = NULL;
	*pnumCircle = 0;
	int i = 0, h = 0, j, left, right, bottom, top, leftn, rightn, bottomn, topn;
	int n = 1;

	for (h = 0; h < *pnumStreet; h++)
	{
		if (rct[h].height > rct[h].width)
		{
			left = rct[h].cx - rct[h].width / 2;
			right = rct[h].cx + rct[h].width / 2;
			bottom = rct[h].cy - rct[h].height / 2;
			top = rct[h].cy + rct[h].height / 2;
			for (n = 1; n < *pnumStreet - 1; n++)
			{
				leftn = rct[n].cx - rct[n].width / 2;
				rightn = rct[n].cx + rct[n].width / 2;
				bottomn = rct[n].cy - rct[n].height / 2;
				topn = rct[n].cy + rct[n].height / 2;
				if (rct[h].height != rct[n].height)
				{
					for (i = bottom; i < top; i++)
					{
						for (j = bottomn; j < topn; j++)
						{
							if (i == j)
							{
								pc = (CIRCLE*)realloc(pc, sizeof(CIRCLE)*(*pnumCircle + 1));
								pc[*pnumCircle].radius = 8;
								pc[*pnumCircle].rgb.blue = 0;
								pc[*pnumCircle].rgb.green = 255;
								pc[*pnumCircle].rgb.red = 255;
								pc[*pnumCircle].x = rct[h].cx;
								pc[*pnumCircle].y = j + 5;
								(*pnumCircle)++;
								pc = (CIRCLE*)realloc(pc, sizeof(CIRCLE)*(*pnumCircle + 1));
								pc[*pnumCircle].radius = 4;
								pc[*pnumCircle].rgb.blue = 200;
								pc[*pnumCircle].rgb.green = 182;
								pc[*pnumCircle].rgb.red = 175;
								pc[*pnumCircle].x = rct[h].cx;
								pc[*pnumCircle].y = j + 5;
								(*pnumCircle)++;
								j = topn;
								i = top;
							}
						}
					}
				}
			}
		}
	}
	return pc;
}
NODESTR* BuildListForwardRCTSTREET(RCT rct[], int* numOfRCTStreet) {
	NODESTR* head = NULL;
	NODESTR* tail = NULL;
	int i;
	for (i = 0; i < *numOfRCTStreet; i++)
	{
		if (head == NULL) // this is the first node
		{
			head = (RCT*)malloc(sizeof(RCT));
			head->key = rct[i];
			head->next = NULL;
			tail = head;
		}
		else// this is not the first element
		{
			tail->next = (RCT*)malloc(sizeof(RCT)); // create new element and link it to the last element
			tail = tail->next; // move tail to the last element
			tail->key = rct[i];
			tail->next = NULL;
		}
	}
	return head; // returns the address of the first node
}
NODERCT* BuildListForwardRCT(RCT rct[],int* numOfRCT)
{
	NODERCT* head = NULL;
	NODERCT* tail = NULL;
	int i;
	for ( i = 0; i < *numOfRCT; i++)
	{
		if (head == NULL) // this is the first node
		{
			head = (RCT*)malloc(sizeof(RCT));
			head->key = rct[i];
			head->next = NULL;
			tail = head;
		}
		else// this is not the first element
		{
			tail->next = (RCT*)malloc(sizeof(RCT)); // create new element and link it to the last element
			tail = tail->next; // move tail to the last element
			tail->key = rct[i];
			tail->next = NULL;
		}
	}
	return head; // returns the address of the first node
}
NODECRL* BuildListForwardCircle(CIRCLE crlc[], int* numOfRCTCircle) {
	NODECRL* head = NULL;
	NODECRL* tail = NULL;
	int i;
	for (i = 0; i < *numOfRCTCircle; i++)
	{
		if (head == NULL) // this is the first node
		{
			head = (CIRCLE*)malloc(sizeof(CIRCLE));
			head->key = crlc[i];
			head->next = NULL;
			tail = head;
		}
		else// this is not the first element
		{
			tail->next = (CIRCLE*)malloc(sizeof(CIRCLE)); // create new element and link it to the last element
			tail = tail->next; // move tail to the last element
			tail->key = crlc[i];
			tail->next = NULL;
		}
	}
	return head; // returns the address of the first node
}