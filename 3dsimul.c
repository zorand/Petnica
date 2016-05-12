/***************************************************************************/
/***********    PROGRAM :  3D SIMULACIJA BRAUNOVOG KRETANJA    *************/
/***********    IS Petnica, jul 1991.                          *************/
/***********    Autor :     Zoran Dimitrijevic,  * KRC *       *************/
/***************************************************************************/

#include <stdio.h>
#include <math.h>
#include <graphics.h>
#include <time.h>
#include <stdlib.h>
#include <dos.h>
#define box bar
#define R 100
#define pi 3.1415
#define forever while(1)
#define ABS(x) x<0 ? (-1)*(x) : (x)
#define Na (6.023*pow(10,23))
#define k (8.31/Na)
int brmol=30,brm=0,xmax=16000,ymax=8700,zmax=2500,poz=1,pr=1,Mr=16;
double brzmol=30.,T,Vsr=2.,masa,Ek;
char inkey;
struct mol
	{
	  double x,y,z,Vx,Vy,Vz;
	};
struct mol molekul[100];

	/*************************************/
	/***  redeklaracije VOID procedura ***/
	/*************************************/

void allert();
void dijagram();
void rnd();
void fizpod();
void povelicini();
void crtanje();

/************************************************************/
/** Inicijalizacija grafickog rezima VGA 640x350 u 16 boja **/
/************************************************************/

void grafika()
{
	int gd, gm;
	gd=VGA;gm=VGAMED;
	initgraph(&gd, &gm,".");
   setrgbpalette(2,20,40,255);
   setrgbpalette(3,20,55,255);
}

scan()
{
   int bio,asci,scan;
	bio=bioskey(0);
   asci=bio & 0xff;
   scan=bio>>8;
	return scan;
}

void okvir()
{
	setcolor(LIGHTBLUE);
   rectangle(0,0,639,329);
   setfillstyle(SOLID_FILL,LIGHTGRAY);
   box(0,330,639,349);
}

/***************************/
/*******  Prvi meni  *******/
/***************************/

void menu1(n0,m0)
int n0,m0;
{
   int i;
   static l;
   char *menu1[4] = {"            DESK","         Podesavanje","         Simulacija","            Kraj"};
   if (l==0)
	{
      setactivepage(1);
		okvir();
      setcolor(RED);
      for (i=0;i<4;i++)
	 outtextxy(i*160,333,menu1[i]);
      setcolor(LIGHTBLUE);
      setfillstyle(SOLID_FILL,LIGHTBLUE);
      box((2*160),330,3*160,349);
		setcolor(YELLOW);
      outtextxy(2*160,333,menu1[2]);
      setactivepage(0);
   }
   if(m0!=0)
   {
      setfillstyle(SOLID_FILL,LIGHTGRAY);
		box(((m0-1)*160),330,(m0*160),349);
      setcolor(RED);
      outtextxy(((m0-1)*160),333,menu1[m0-1]);
   }  else setcolor(RED);
   for (i=0;i<4;i++)
   {
		outtextxy(i*160,333,menu1[i]);
   }
   setfillstyle(SOLID_FILL,LIGHTBLUE);
   box (((n0-1)*160),330,(n0*160),349);
   setcolor(YELLOW);
	outtextxy(((n0-1)*160),333,menu1[n0-1]);
}

void desk()
{
   setfillstyle(SOLID_FILL,LIGHTGRAY);
   box(175,100,490,225);
   setcolor(LIGHTBLUE);
   rectangle(175,100,490,225);
   setfillstyle(SOLID_FILL,LIGHTBLUE);
   outtextxy(245,125,"Simulacija Braunovog kretanja");
	outtextxy(225,150,"Program by Zoran Dimitrijevic, K R C");
   putpixel(392,152,LIGHTBLUE);
   putpixel(391,153,LIGHTBLUE);
   outtextxy(300,170,"(c) 1991.");
   box(315,190,335,203);
   setcolor(YELLOW);
	outtextxy(320,192,"OK");
   setfillstyle(SOLID_FILL,0);
	do { inkey=scan();}
   while (inkey != 28);
   box(175,100,490,225);
}

/***************************************************************************/
/*                              SIMULACIJA                                 */
/*    Pritiskom na Esc se izlazi iz procedure simulacija.                  */
/*    Pritiskom na <d> dobija se ili se sklanja prozor sa parametrima gasa.*/
/*      3D  simulacija kretanja molekula u mom programu je zamisljena na   */
/*    sledeci nacin:  svaki molekul je na ekranu predstavljen sa kuglicom  */
/*    cija je velicina i polozaj odredjen sa x,y i z koordinatama. Postoji */
/*    96 razlicitih velicina kuglica (od 4 tacke do 100 tacaka). U za-     */
/*    visnosti od z koordinate menja se velicina kuglice. Prostor u kome   */
/*    se nalaze molekuli je predstavljen u perspektivi i na ekranu se vidi */
/*    zarubljena piramida sa jedom osnovom koja se moze menjati u toku     */
/*    izvrsenja programa. Najmanja vrednost koordinata te osnove su 641x351*/
/*    x200.                                                                */
/*      Sudar je registrovan kada je rastojanje izmedju molekula precnik R */
/*    odnosno 100 tacaka u mom koordinatnom sistemu.                       */
/*                                                                         */
/***************************************************************************/

void simulacija()
{
	int i,j,bio,asci;
   struct REGPACK regs;
   if (brmol==0)
      {
	allert();
	return;
      }
	rnd();
   do
   {
      for(i=0;i<(brmol-1);i++)
	 {
	    for(j=i+1; j<brmol; j++)
	       {
		  if (provsud(i,j)==1)
		  {
				 menjajbrz(i,j);
		  }
			 }
		 promenaxyz(i);
	 }
	 promenaxyz(brmol-1);
	 crtanje();
	 regs.r_ax=0x100;
	 intr(0x16, &regs);
	 if ((regs.r_flags & 64)==0)
	 {
		 asci=getch();
	 }
	 if (asci=='d' || asci=='D')
	 {
		 pr*=(-1);
		 asci=0;
	 }
	 if (asci==27)
	 {
		 setactivepage(0);
		 setvisualpage(0);
	    return;
	 }
     }
   forever;

}

void allert()
{
   int inkey;
	setfillstyle(SOLID_FILL,LIGHTGRAY);
   setcolor(LIGHTBLUE);
   box(235,125,405,210);
   rectangle(235,125,405,210);
   outtextxy(243,150,"Nema svrhe raditi animaciju");
   outtextxy(255,170,"sa nijednim molekulom");
   setfillstyle(SOLID_FILL,LIGHTBLUE);
	box(310,185,330,200);
   setcolor(YELLOW);
   outtextxy(316,187,"OK");
	inkey=scan();
	while (inkey!=28)
		inkey=scan();
   setfillstyle(SOLID_FILL,0);
   box(235,125,405,210);
}

/***************************************************************************/
/****     Procedura rnd postavlja slucajne polozaje i slucajne brzine   ****/
/****   za molekule koji ulaze u prostor koji se vidi na ekranu. Zbog   ****/
/****   sto se na ekranu predstavlja deo prostora u njemu nije jednaka  ****/
/****   Koncetracija u sredini ekrana je manja nego na krajevima ekrana ****/
/****   jer mnogi molekuli koji ulaze na ekran imaju takvu brzinu da    ****/
/****   vrlo brzo napustaju ekran.                                      ****/
/***************************************************************************/

void rnd()
{
   int rnd,i,j,ps;
	if (brm==0)
   {
      rnd=random(97)+4;
      molekul[brm].z = (double)(rnd*25);
		molekul[brm].x = (xmax/2-320)*molekul[brm].z/zmax-R/2+(double)(random(32767))/32767*(xmax/2-320)*molekul[brm].z/zmax;
		molekul[brm].y = ymax-(ymax/2-175)*molekul[brm].z/zmax+R/2;
		molekul[brm].Vy= (double)(random(32767))/32767;
		molekul[brm].Vz= (-1)*(double)(random(32767))/32767;
		if (random(2)==1)
		molekul[brm].Vz*=(-1)*molekul[brm].Vy*(zmax/(xmax/2-320));
		molekul[brm].Vx= pow(2.-pow(molekul[brm].Vy,2)-pow(molekul[brm].Vz,2),0.5);
		if (random(2)==1)
		molekul[brm].Vx*=(-1);
		molekul[0].z = (double)(rnd*25);
      brm=1;
	}
   while (brmol > brm)
   {
      rnd=random(97)+4;
      switch (rnd % 5)
      {
	  /*************************************************/
	  /****  molekuli ulaze sa donjeg kraja ekrana ****/
	  /*************************************************/
	  case 0 : {
		      molekul[brm].z = (double)(rnd*25);
		      molekul[brm].x = (xmax/2-320)*molekul[brm].z/zmax-R/2+(double)(random(32767))/32767*(xmax/2-320)*molekul[brm].z/zmax;
		      molekul[brm].y = ymax-(ymax/2-175)*molekul[brm].z/zmax+R/2;
		      molekul[brm].Vy= (double)(random(32767))/32767;
                      molekul[brm].Vz= (-1)*(double)(random(32767))/32767;
		      if (random(2)==1)
		      molekul[brm].Vz*=(-1)*molekul[brm].Vy*(zmax/(xmax/2-320));
		      molekul[brm].Vx= pow(2.-pow(molekul[brm].Vy,2)-pow(molekul[brm].Vz,2),0.5);
				if (random(2)==1)
		      molekul[brm].Vx*=(-1);
			 for (j=0;j<brm;j++)
				 if (provsud(brm,j)==0)
					 { brm++;break;}
		      break;
		   }

	  /***********************************************/
	  /**** molekuli ulaze sa desnog kraja ekrana ****/
	  /***********************************************/

	  case 1 : {
		      molekul[brm].z = (double)(rnd*25);
		      molekul[brm].x = xmax-(xmax/2-320)*molekul[brm].z/zmax+R/2;
		      molekul[brm].y = (ymax/2-175)*molekul[brm].z/zmax-R/2+(double)(random(32767))/32767*(ymax-2*(ymax/2-175)*molekul[brm].z/zmax);
		      molekul[brm].Vx= (-1)*(double)(random(32767))/32767;
		      molekul[brm].Vz= (-1)*(double)(random(32767))/32767;
                      if (random(2)==1)
		      molekul[brm].Vz*=(-1)*molekul[brm].Vx*(zmax/(xmax/2-320));
		      molekul[brm].Vy=pow(2.-pow(molekul[brm].Vy,2)-pow(molekul[brm].Vz,2),0.5);
		      if (random(2)==1)
				molekul[brm].Vy*=(-1);
			 for (j=0; j<brm; j++)
				  if (provsud(brm,j)==0)
				{ brm++;break;}
				break;
		   }

	  /*************************************************/
	  /**** molekuli ulaze sa  gornjeg kraja ekrana ****/
	  /*************************************************/

	  case 2 : {
		      molekul[brm].z = (double)(rnd*25);
		      molekul[brm].x = (xmax/2-320)*molekul[brm].z/zmax-R/2+(double)(random(32767))/32767*(xmax-2*(xmax/2-320)*molekul[brm].z/zmax);
		      molekul[brm].y = (zmax/2-175)*molekul[brm].z/zmax-R/2;
		      molekul[brm].Vx= (double)(random(32767))/32767;
		      molekul[brm].Vz= (-1)*(double)(random(32767))/32767;
		      if (random(2)==1)
		      molekul[brm].Vz*=(-1)*molekul[brm].Vx*zmax/(xmax/2-320);
		      if (random(2)==1)
		      molekul[brm].Vx*=(-1);
		      molekul[brm].Vy= pow(2.-pow(molekul[brm].Vz,2)-pow(molekul[brm].Vx,2),0.5);
			 for (j=0; j<brm; j++)
				 if (provsud(brm,j)==0)
					 { brm++;break;}
		      break;
			}

	  /**********************************************/
	  /**** molekuli ulaze sa levog kraja ekrana ****/
	  /**********************************************/

	  case 3 : {
				molekul[brm].z = (double)(rnd*25);
		      molekul[brm].x = (xmax/2-320)*molekul[brm].z/zmax-R/2;
		      molekul[brm].y = (ymax/2-175)*molekul[brm].z/zmax-R/2+(double)(random(32767))/32767*(ymax-2*(ymax/2-175)*molekul[brm].z/zmax);
		      molekul[brm].Vx= (double)(random(32767))/32767;
		      molekul[brm].Vz= (-1)*(double)(random(32767))/32767;
		      if (random(2)==1)
		      molekul[brm].Vz*=(-1)*molekul[brm].Vx*zmax/(xmax/2-320);
		      molekul[brm].Vy= pow(2.-pow(molekul[brm].Vz,2)-pow(molekul[brm].Vy,2),0.5);
		      if (random(2)==1)
		      molekul[brm].Vy*=(-1);
		      for (j=0; j<brm; j++)
			  if (provsud(brm,j)==0)
				{ brm++;break;}
				break;
		   }
	  /************************************************/
	  /**** molekuli ulaze odozdo i idu prema nama ****/
	  /************************************************/

	  case 4 :
		   {
		      molekul[brm].z = 0.;
				molekul[brm].x = (xmax/2-320)*molekul[brm].z/zmax+(double)(random(32767))/32767*(xmax-2*(xmax/2-320)*molekul[brm].z/zmax);
		      molekul[brm].y = (ymax/2-225)*molekul[brm].z/zmax+(double)(random(32767))/32767*(ymax-2*(ymax/2-175)*molekul[brm].z/zmax);
		      molekul[brm].Vz= (double)(random(32767))/32767;
		      molekul[brm].Vy= (double)(random(32767))/32767;
		      if (random(2)==1)
		      molekul[brm].Vy*=(-1);
		      molekul[brm].Vx= pow(2.-pow(molekul[brm].Vz,2)-pow(molekul[brm].Vy,2),0.5);
		      if (random(2)==1)
		      molekul[brm].Vx*=(-1);
		      for (j=0; j<brm; j++)
			  if (provsud(brm,j)==0)
				{ brm++;break;}
		      break;
			}
	     }
		}
}

/***************************************************************************/
/****   Ova procedura se moze znatno ubrzati i to ce biti uradjeno u    ****/
/****   kasnijim verzijama. Izvrsava se pre svakog iscrtavanja da bi    ****/
/****   se veci (i blizi) molekuli crtali preko manjih (tj. daljih).    ****/
/***************************************************************************/

int poredi(const void *a, const void *b){
	if( ((struct mol *)a)->z > ( (struct mol *)b)->z ) return 1;
	else return -1;
}

void povelicini()
{
	int i,j;
	struct mol m;
	qsort((void *)molekul, brm, sizeof(molekul[0]), poredi);
}
/*	for (i=0;i<brm-1;i++)
		for (j=i;j<brm;j++)
	 if (molekul[i].z > molekul[j].z)
		 {
			 m=molekul[i].z;
			 molekul[i].z=molekul[j].z;
			 molekul[j].z=m;
			 m=molekul[i].x;
			 molekul[i].x=molekul[j].x;
			 molekul[j].x=m;
			 m=molekul[i].y;
			 molekul[i].y=molekul[j].y;
			 molekul[j].y=m;
			 m=molekul[i].Vx;
			 molekul[i].Vx=molekul[j].Vx;
			 molekul[j].Vx=m;
			 m=molekul[i].Vy;
			 molekul[i].Vy=molekul[j].Vy;
			 molekul[j].Vy=m;
			 m=molekul[i].Vz;
			 molekul[i].Vz=molekul[j].Vz;
			 molekul[j].Vz=m;

			m=molekul[i];
			molekul[i]=molekul[j];
			molekul[j]=m;
	 }
	 */

provsud(i,j)
int i,j;
{
	double r;
	r=pow((molekul[i].x - molekul[j].x),2.)+pow((molekul[i].y - molekul[j].y),2.)+pow((poz*(molekul[i].z-molekul[j].z)),2.);
	if ( r <= pow(R,2.) )
   {
      return (1);
   }
   else
   return (0);
}

menjajbrz(i,j)
int i,j;
{
   double A,B,v1,v2,v3,Vx,Vy,Vz,x,y,z;
   x=molekul[i].x-molekul[j].x;
   y=molekul[i].y-molekul[j].y;
   z=poz*(molekul[i].z-molekul[j].z);
   A=atan(ABS(y)/(ABS(x)+0.0000001));
   if (A < pi)
      B = pi - 2*A;
   else
      B = 2*A - pi;
   Vx = ABS(molekul[i].Vx+molekul[j].Vx)/2;
   Vy = ABS(molekul[i].Vy+molekul[j].Vy)/2;
   Vz = ABS(molekul[i].Vz+molekul[j].Vz)/2;
   v1 = Vx * sin(B);
   v2 = Vx * cos(B);
   A=atan(ABS(x)/(ABS(z)+0.0000001));
   if (A < pi)
      B = pi - 2*A;
   else
      B = 2*A - pi;
	v1+= Vz * cos(B);
   v3 = Vz * sin(B);
   A=atan(ABS(z)/(ABS(y)+0.0000001));
   if (A < pi)
      B = pi - 2*A;
   else
      B = 2*A - pi;
   v2+= Vy * sin(B);
   v3 = Vz * cos(B);
   if ( x<0 )
      {
       molekul[i].Vx= -1*v1;
       molekul[j].Vx= v1;
      }
   else
      {
       molekul[i].Vx= v1;
       molekul[j].Vx= -1*v1;
      }
   if ( y<0 )
      {
       molekul[i].Vy= -1*v2;
		 molekul[j].Vy= v2;
      }
   else
      {
       molekul[i].Vy= v2;
       molekul[j].Vy= -1*v2;
      }
   if ( z<0 )
      {
       molekul[i].Vz= -1 * v3;
       molekul[j].Vz= v3;
      }
   else
      {
       molekul[i].Vz= v3;
       molekul[j].Vz= -1*v3;
      }
	promenaxyz(i);
	promenaxyz(j);
}

promenaxyz(i)
int i;
{
   int e;
   molekul[i].x += molekul[i].Vx*brzmol;
   molekul[i].y += molekul[i].Vy*brzmol;
   molekul[i].z += poz*molekul[i].Vz*brzmol;
	if (provera(molekul[i].x,molekul[i].y,molekul[i].z)==1)
   {
      for (e=i; e<brm; e++)
	 molekul[e]=molekul[e+1];
      brm--;
		rnd();
   }
}

provera(x,y,z)
double x,y,z;
{
   if (x < (xmax/2-320)*z/zmax-R/2 || x > xmax-(xmax/2-320)*z/zmax+R/2) return (1);
   if (y < (ymax/2-175)*z/zmax-R/2 || y > ymax-(ymax/2-175)*z/zmax+R/2) return (1);
   if (z < 0 || z > zmax) return (1);
	return (0);
}

/***************************************************************************/
/***   Animacija se postize preklapanjem video strana u ovoj proseduri   ***/
/***************************************************************************/
void waitsync(){
/*	asm{
		mov dx,0x3DA
l1:
		in al,dx
		and al,8
		jnz l1
l2:
		in al, dx
		and al,8
		jz l2
	}
*/
}

void crtanje()
{
	double x,y,z;
	int i;
	static int acpg=0;
	if (acpg==0)
	{
		waitsync();
		setactivepage(1);
		setvisualpage(0);
		acpg=1;
	}
	else
	{
		waitsync();
		setactivepage(0);
		setvisualpage(1);
		acpg=0;
	}
	setfillstyle(SOLID_FILL,0);
	setcolor(0);
	setviewport(1,1,638,328,1);
	box(0,0,638,329);
	povelicini();

	for(i=0;i<brmol;i++)
	{
		x=(molekul[i].x-molekul[i].z*(xmax/2-320)/zmax)*640/(xmax-2*molekul[i].z*(xmax/2-320)/zmax);
		y=(molekul[i].y-molekul[i].z*(ymax/2-175)/zmax)*640/(ymax-2*molekul[i].z*(ymax/2-175)/zmax);
		z=(molekul[i].z)/(R/2);
		setfillstyle(SOLID_FILL,LIGHTBLUE);
		setcolor(BLUE);
		fillellipse((int)x,(int)y,(int)z,(int)(z*15/20));
		setcolor(2);
		setfillstyle(SOLID_FILL,2);
		fillellipse((int)(x+z/3),(int)(y-z/3),(int)(z/3),(int)(z/3*15/20));
		setfillstyle(SOLID_FILL,3);
		setcolor(2);
		fillellipse((int)(x+z/3),(int)(y-z/3),(int)(z/5),(int)(z/5*15/20));
	}
	if (pr==1) dijagram();
}

void dijagram()
{
	int brojac;
	long int c1,c2,c3;
	double Vk;
	static int w,brmo;
	static double v,e,t,p;
	char c4[4],c5[3],c6[4];
	if (w==0) brmo=brmol;
	for(brojac=0;brojac<brmol;brojac++)
	{
		Vk=pow(pow(molekul[brojac].Vx,2)+pow(molekul[brojac].Vy,2)+pow(molekul[brojac].Vz,2),0.5)*brzmol/10;
		Vsr+=Vk;
		Ek+=(pow(Vk,2)*masa)/2.;
	}
	Vsr/=brmol;
	Ek/=brmol;
	T=(pow(Vk,2.)*masa)/(2*k);
	if ((w==0)||(brmo!=brmol))
	{
		v=Vsr;
		e=Ek;
		t=T;
		w=1;
		brmo=brmol;
	}
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	box(10,230,220,324);
	c1=(int)Vsr;
	c2=(int)(Vsr*10-(double)((int)Vsr)*10);
	c3=(int)(Vsr*100-(double)((int)Vsr)*100)-10*c2;
	itoa(c1,c4,10);
	itoa(c2,c5,10);
	itoa(c3,c6,10);
	setcolor(RED);
	outtextxy(23,235,"Vsr=");
	outtextxy(47,235,c4);
	outtextxy(57,235,".");
	outtextxy(61,235,c5);
	outtextxy(69,235,c6);
	c1=(int)(Ek*(1e+23));
	c2=(int)(Ek*(1e+23)*10-(double)((int)(Ek*(1e+23)))*10);
	c3=(int)(Ek*(1e+23)*100-(double)((int)(Ek*(1e+23)))*100)-10*c2;
	itoa(c1,c4,10);
	itoa(c2,c5,10);
	itoa(c3,c6,10);
	outtextxy(83,235," Ek=");
	outtextxy(107,235,c4);
	outtextxy(125,235,".");
	outtextxy(129,235,c5);
	outtextxy(138,235,c6);
	c1=(int)(T);
	c2=(int)(T*10)-(double)((int)T*10);
	c3=(int)(T*100)-(double)((int)T*100)-10*c2;
	itoa(c1,c4,10);
	itoa(c2,c5,10);
	itoa(c3,c6,10);
	outtextxy(143,235,"  T=");
	outtextxy(167,235,c4);
	outtextxy(185,235,".");
	outtextxy(189,235,c5);
	outtextxy(198,235,c6);
	setfillstyle(SOLID_FILL,BLUE);
	box(34,270-(int)((1.-v/Vsr)*10),54,320);
	setfillstyle(SOLID_FILL,LIGHTBLUE);
	box(100,270-(int)((1.- e / Ek)*10),120,320);
	setfillstyle(SOLID_FILL,RED);
	box(165,320-(int)(T/10),185,320);
}

podesavanje()
{
	int w=0,j;
	unsigned a;
	char br[7][7];

	itoa(brmol,br[0],10);
	itoa((int)brzmol,br[1],10);
	itoa(xmax,br[2],10);
	itoa(ymax,br[3],10);
	itoa((zmax/poz),br[4],10);
	itoa(Mr,br[5],10);
	strcpy(br[6]," OK");
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	setcolor(LIGHTBLUE);
	box(100,50,300,280);
	rectangle(100,50,300,280);
	setfillstyle(SOLID_FILL,YELLOW);
	box(120,60,280,80);
	rectangle(120,60,280,80);
	outtextxy(165,64,"PODESAVANJE:");
	outtextxy(110,100,"Broj molekula na ekranu:");
	outtextxy(110,120,"Brzina kretanja molekula:");
	outtextxy(110,140,"X-maksimum:");
	outtextxy(110,160,"Y-maksimum:");
	outtextxy(110,180,"Z-maksimum:");
	outtextxy(110,200,"Relativna molekulska masa:");
	outtextxy(110,220,"Povratak u meni:");
	while(w!=10)
	{
		for(j=0;j<7;j++)
		{
	 setcolor(LIGHTBLUE);
	 outtextxy(265,100+20*j,br[j]);
		}
		setfillstyle(SOLID_FILL,LIGHTBLUE);
		box(260,99+20*w,295,114+20*w);
		setcolor(YELLOW);
		outtextxy(265,100+20*w,br[w]);
		inkey=scan();
		if ((inkey==80) && (w<6) )
	 {
		 setfillstyle(SOLID_FILL,LIGHTGRAY);
		 box(260,99+20*w,295,114+20*w);
		 w+=1;
		 continue;
	 }
		if ((inkey==72) && (w>0) )
	 {
		 setfillstyle(SOLID_FILL,LIGHTGRAY);
		 box(260,99+20*w,295,114+20*w);
		 w-=1;
		 continue;
	 }
		if ((inkey==28) && (w==0))
	 {

		 a=editor(230,250,290,270,31,19);
		 if ((a>100)||(a<2)) {if (a!=-1) prompt(230,250,290,270);}
		 else
		 {
			 itoa((int)a,br[0],10);
			 brmol=a;
		 }
		 continue;
	 }
		if ((inkey==28) && (w==1))
	 {
		 a=editor(230,250,290,270,31,19);
		 if ((a>100)||(a<1)) {if (a!=-1) prompt(230,250,290,270);}
		 else
		 {
			 itoa((int)a,br[1],10);
			 brzmol=(double)a;
		 }
		 continue;
	 }
		if ((inkey==28) && (w==2))
	 {
		 a=editor(230,250,290,270,31,19);
		 if ((a>32500)||(a<641)) {if (a!=-1) prompt(230,250,290,270);}
		 else
		 {
			 itoa((int)a,br[2],10);
			 xmax=a;
		 }
		 continue;
	 }
		if ((inkey==28) && (w==3))
	 {
		 a=editor(230,250,290,270,31,19);
		 if ((a>32500)||(a<351)) {if (a!=-1) prompt(230,250,290,270);}
		 else
		 {
			 itoa((int)a,br[3],10);
			 ymax=a;
		 }
		 continue;
	 }
		if ((inkey==28) && (w==4))
	 {
		 a=editor(230,250,290,270,31,19);
		 if ((a>32500)||(a<200)) {if (a!=-1) prompt(230,250,290,270);}
		 else
		 {
			 itoa((int)a,br[4],10);
			 poz=a/zmax;
		 }
		 continue;
	 }
		if ((inkey==28) && (w==5))
			{
		 a=editor(230,250,290,270,31,19);
		 if ((a>10000)||(a<1)) {if (a!=-1) prompt(230,250,290,270);}
		 else
		 {
			 itoa((int)a,br[5],10);
			 Mr=a;
			 masa=Mr/Na;
		 }
		 continue;
	 }
		if (((inkey==28) && (w==6))||(inkey==1))
	 {
		 break;
	 }
	}
	setfillstyle(SOLID_FILL,0);
	box(100,20,300,280);
}

prompt(n,m,e,f)
int n,m,e,f;
{
	setfillstyle(SOLID_FILL,0);
	box(n,m,e,f);
	setcolor(LIGHTBLUE);
	rectangle(n,m,e,f);
	setcolor(YELLOW);
	outtextxy(n+5,m+3,"Los unos");
	getch();
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	box(n,m,e,f);
}
/***************************************************************************/
/*****  Unose se koordinati prozora i,m,e,f i pocetak u grafickom modu *****/
/***************************************************************************/

int editor(i,m,e,f,x,y)
int i,m,e,f,x,y;
{
	unsigned int a=0,n=0;
	int bio,asci;
	setfillstyle(SOLID_FILL,0);
	box(i,m,e,f);
	setcolor(LIGHTBLUE);
	rectangle(i,m,e,f);
	forever
	{
		setcolor(YELLOW);
		line(i+9+n*8,m+4,i+9+n*8,f-6);
		bio=bioskey(0);
		asci=bio & 0xff;
		if ((asci > 47) && (asci < 59) && (n<5))
		{
	 setcolor(0);
	 line(i+9+n*8,m+4,i+9+n*8,f-6);
	 gotoxy(x+n,y);
	 printf("%c",asci);
	 a=a*10+asci-48;
	 n++;
	 continue;
		}
		if ((asci==8) && (n>0))
		{
	 n--;
	 setfillstyle(SOLID_FILL,0);
	 box(i+9+n*8,m+4,i+17+n*8,f-6);
	 a=a/10;
		}
		if (asci==13)
		{
	 setfillstyle(SOLID_FILL,LIGHTGRAY);
	 box(i,m,e,f);
	 return(a);
		}
      if (asci==27)
      {
	 setfillstyle(SOLID_FILL,LIGHTGRAY);
	 box(i,m,e,f);
	 return(-1);
      }
   }
}

/***************************************************************************/
/*******    Iz funkcije main se menijima pozivaju druge procedure    *******/
/***************************************************************************/

main()
{
	int a;
	static int m0=0,n0=1;
	masa=Mr/Na;
	do
	{
	n0=1;
	grafika();
	okvir();
	randomize();
	settextstyle(SMALL_FONT,HORIZ_DIR,4);
	menu1(n0,m0);
	do
	{
		do {inkey=scan();}
		while ((inkey!=75) && (inkey!=77) && (inkey!=28));
		if (inkey==75 && n0>1) { m0=n0; n0=n0-1; menu1(n0,m0); }
		if (inkey==77 && n0<4) { m0=n0; n0=n0+1; menu1(n0,m0); }
		if (inkey == 28)
		{
	 switch (n0)
	 {
		case 1: { desk() ; break; }
		case 2: { podesavanje();break;}
		case 3: { simulacija();n0=5;break; }
		case 4:  n0=6;
	 }
		}
	}
	while (n0!=5 && n0!=6);
	}
	while (n0!=6);
	closegraph();
}
