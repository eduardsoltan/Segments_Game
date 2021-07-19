#include <iostream>
#include <graphics.h>
#include <cstring>
#include <winbgim.h>
#include <windows.h>
#include <ctime>
#include <cmath>
#define RADIUS 5
#define DISTANTAPUNCTE 35
#define OFFSET 70

using namespace std;
struct text{
    char msg[100];
    int lungime;
} mesaj;

int tasta,x,y,z, inceput2;
char t[100];
struct coordonate {
    int x,y;
};
bool SavePressed =false;

struct Joc{
   DWORD screenWidth = GetSystemMetrics(SM_CXSCREEN);
   DWORD screenHeight = GetSystemMetrics(SM_CYSCREEN);
   int nrPunte;
   int posibleCombination;
   int playerColor[2] = {2, 10};
   int blockPoints;
   int playerTurn;
   int playerScore1 = 0;
   int playerScore2 = 0;
   int playMode;
   int numberPts1=10;
   int numberPts2=20;
   coordonate puncte[100];
   int adiacenta[100][100];
   int vizitat[100];
} stareaJoc, stareaJocPosibila;

void initalizarePunte();
void Play();
void desenareTabla(int dx);

void eliberareStructura(int x) {
    for(int i=1; i<=stareaJoc.nrPunte; i++)
        for(int j=1;j<=stareaJoc.nrPunte; j++)
           stareaJoc.adiacenta[i][j] = 0;
    for(int i=1;i<=stareaJoc.nrPunte; i++) {
        stareaJoc.vizitat[i] = 0;
        stareaJoc.puncte[i].x = 0;
        stareaJoc.puncte[i].y = 0;
    }
    stareaJoc.nrPunte = 0;

    if(x == 1) {
        stareaJoc.playerScore1 = 0;
        stareaJoc.playerScore2 = 0;
    }
}

float distance(coordonate pct1, coordonate pct2) {
    float rad = sqrt((pct2.x - pct1.x) * (pct2.x - pct1.x) + (pct2.y - pct1.y) * (pct2.y - pct1.y));
    return rad;
}

bool detectareaDistanta(coordonate pct, int j) {
    for(int i=1;i<=j;i++) {
        if(distance(pct, stareaJoc.puncte[i]) < DISTANTAPUNCTE)
            return true;
    }
    return false;
}

void sortarePuncte() {
    for(int i=1; i<= stareaJoc.nrPunte; i++)
    {
        for(int j=i; j <= stareaJoc.nrPunte; j++) {
            if(stareaJoc.puncte[i].x > stareaJoc.puncte[j].x)
                swap(stareaJoc.puncte[i], stareaJoc.puncte[j]);
        }
    }
}

bool esteInCerc(coordonate centru, coordonate pct) {
    if((pct.x - centru.x)*(pct.x - centru.x) + (pct.y - centru.y)*(pct.y - centru.y) <= (RADIUS + 1) * (RADIUS + 1))
        return true;
    return false;
}

bool canDrawLine(int x, int y) {

    if(x > y){
        int aux;
        aux = x;
        x = y;
        y = aux;
    }

    int a = stareaJoc.puncte[x].y - stareaJoc.puncte[y].y;
    int b = stareaJoc.puncte[y].x - stareaJoc.puncte[x].x;
    int c = (stareaJoc.puncte[x].x - stareaJoc.puncte[y].x) * stareaJoc.puncte[x].y + (stareaJoc.puncte[y].y - stareaJoc.puncte[x].y) * stareaJoc.puncte[x].x;

    for(int i = x + 1; i<= y - 1; i++) {
        float distance = (abs(a*stareaJoc.puncte[i].x + b*stareaJoc.puncte[i].y +c)) / (sqrt(a*a + b*b));
        if( distance <= RADIUS + 2 )
          return false;
    }
    return true;
}

void checkColiniarity() {

    for(int i = 1; i <= stareaJoc.nrPunte; i++) {
        for(int j = i + 1; j <= stareaJoc.nrPunte; j++) {
            if(!canDrawLine(i, j)) {
                stareaJoc.adiacenta[i][j] = 3;
                stareaJoc.adiacenta[j][i] = 3;
            }
        }
    }
}

int direction(coordonate p0, coordonate p1, coordonate p2) {
    return (p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y);
}

void segmentIntersection(int x, int y) {

    stareaJoc.posibleCombination = 0;
    for(int i=1;i <= stareaJoc.nrPunte; i++)
    {
        for(int j = i + 1; j <= stareaJoc.nrPunte; j++) {

           if(stareaJoc.vizitat[i] == 0 && stareaJoc.vizitat[j] == 0) {
              int d1 = direction(stareaJoc.puncte[i], stareaJoc.puncte[x], stareaJoc.puncte[y]);
              int d2 = direction(stareaJoc.puncte[j], stareaJoc.puncte[x], stareaJoc.puncte[y]);
              int d3 = direction(stareaJoc.puncte[x], stareaJoc.puncte[i], stareaJoc.puncte[j]);
              int d4 = direction(stareaJoc.puncte[y], stareaJoc.puncte[i], stareaJoc.puncte[j]);

              if(((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
                stareaJoc.adiacenta[i][j] = 4;
                stareaJoc.adiacenta[j][i] = 4;
              }
           }

           if(stareaJoc.adiacenta[i][j] == 0 && stareaJoc.vizitat[i] == 0 && stareaJoc.vizitat[j] == 0)
              stareaJoc.posibleCombination += 1;

        }
    }
}


int winnerDetermination() {

    if(stareaJoc.nrPunte % 4 == 0) {
        if(stareaJoc.blockPoints % 4 == 2)
            return 1;
        else if(stareaJoc.blockPoints % 4 == 0)
            return 2;
    }
    else if(stareaJoc.nrPunte % 4 == 1) {
        if(stareaJoc.blockPoints % 4 == 3)
            return 1;
        else if(stareaJoc.blockPoints % 4 == 1)
            return 2;
    }
    else if(stareaJoc.nrPunte % 4 == 2) {
        if(stareaJoc.blockPoints % 4 == 0)
            return 1;
        else if(stareaJoc.blockPoints % 4 == 2)
            return 2;
    }
    else if(stareaJoc.nrPunte % 4 == 3) {
        if(stareaJoc.blockPoints % 4 == 1)
            return 1;
        else if(stareaJoc.blockPoints % 4 == 3)
            return 2;
    }
}

void restoreFile() {
    FILE *fptr;
    fptr = fopen("fisier.txt", "rb");
    if(fptr == NULL) {

    }

    size_t rezultat = fread(&stareaJoc, sizeof(Joc), 1, fptr);

    desenareTabla(2);
    fclose(fptr);
}

void salvareaFisier() {
    FILE *fptr;
    fptr = fopen("fisier.txt", "wb+");

    fwrite(&stareaJoc, sizeof(Joc), 1, fptr);
    closegraph();
    fclose(fptr);
    SavePressed=true;
    //return;
}

void findPoints(int x1,int y1, int x2, int y2) {
    bool gasit1 = false, gasit2 = false;
    int xx, yy;
    coordonate pct1, pct2;
    pct1.x = x1;
    pct1.y = y1;
    pct2.x = x2;
    pct2.y = y2;

    for(int i=1; i<=stareaJoc.nrPunte; i++) {
        if(esteInCerc(stareaJoc.puncte[i], pct1)) {
            gasit1 = true;
            xx = i;
        }
        if(esteInCerc(stareaJoc.puncte[i], pct2)) {
            gasit2 = true;
            yy = i;
        }
    }
    if(xx == yy)
        return;

    if(gasit1 && gasit2) {
        if(stareaJoc.adiacenta[xx][yy] == 3) {
             setcolor(WHITE);
             strcpy(mesaj.msg , "Nu puteti trege o linie peste un punct coliniar");
             mesaj.lungime = strlen(mesaj.msg);
             outtextxy(OFFSET, stareaJoc.screenHeight - OFFSET + 10, mesaj.msg);
             setcolor(stareaJoc.playerColor[stareaJoc.playerTurn - 1]);
        }
        else if(stareaJoc.adiacenta[xx][yy] == 4 || stareaJoc.adiacenta[yy][xx] == 4) {
            setcolor(WHITE);
            strcpy(mesaj.msg, "Nu puteti trage un segment care sa se interseteze un alt segment");
            mesaj.lungime = strlen(mesaj.msg);
            outtextxy(OFFSET, stareaJoc.screenHeight - OFFSET + 10, mesaj.msg);
            setcolor(stareaJoc.playerColor[stareaJoc.playerTurn - 1]);
        }
        else if( stareaJoc.vizitat[xx] != 0 || stareaJoc.vizitat[yy] != 0) {
            setcolor(WHITE);
            strcpy(mesaj.msg, "Nu puteti folosi un punct deja folosit");
            mesaj.lungime = strlen(mesaj.msg);
            outtextxy(OFFSET, stareaJoc.screenHeight - OFFSET + 10, mesaj.msg);
            setcolor(stareaJoc.playerColor[stareaJoc.playerTurn - 1]);
        }
        else if( stareaJoc.adiacenta[xx][yy] == 0 && stareaJoc.vizitat[xx] == 0 && stareaJoc.vizitat[yy] == 0)  {
          line(stareaJoc.puncte[xx].x, stareaJoc.puncte[xx].y, stareaJoc.puncte[yy].x, stareaJoc.puncte[yy].y);
          stareaJoc.adiacenta[xx][yy] = stareaJoc.playerTurn;
          stareaJoc.adiacenta[yy][xx] = stareaJoc.playerTurn;

          if(stareaJoc.playerTurn == 1) {
            stareaJoc.playerTurn = 2;
            settextstyle(8, 0, 1);
            setlinestyle(0, 0, 1);
            setcolor(WHITE);
            setbkcolor(RED);

            outtextxy((inceput2 + inceput2 + textwidth("Player2") + 20) / 2 - textwidth("Player2") / 2, (2 *(2 * OFFSET) + textheight("Player2") + 20) / 2 - textheight("Player2") / 2, "Player2");
            setcolor(stareaJoc.playerColor[1]);
            setbkcolor(BLACK);
            setlinestyle(0, 0, 2);
            settextstyle(8, 0, 3);
          }
          else {
            stareaJoc.playerTurn = 1;
            settextstyle(8, 0, 1);
            setlinestyle(0, 0, 1);
            setcolor(WHITE);
            setbkcolor(RED);

            outtextxy((inceput2 + inceput2 + textwidth("Player1") + 20) / 2 - textwidth("Player1") / 2, (2 *(2 * OFFSET) + textheight("Player1") + 20) / 2 - textheight("Player1") / 2, "Player1");
            setbkcolor(BLACK);
            setlinestyle(0, 0, 2);
            settextstyle(8, 0, 3);

            setcolor(stareaJoc.playerColor[0]);

          }


          stareaJoc.vizitat[xx] = 1;
          stareaJoc.vizitat[yy] = 1;
          stareaJoc.blockPoints -= 2;
          segmentIntersection(xx, yy);
          if(stareaJoc.posibleCombination == 0) {
             setcolor(WHITE);
             int mijlocx = (((OFFSET + stareaJoc.screenWidth) / 2 + OFFSET) + stareaJoc.screenWidth) / 2;
             settextstyle(8, 0, 5);
             if(winnerDetermination() == 1) {
                int width = textwidth("PLAYER1 WON");
                outtextxy(mijlocx - width / 2, 7 * OFFSET, "PLAYER1 WON");
                stareaJoc.playerScore1 += 1;
             }
             else {
                int width = textwidth("PLAYER2 WON");
                outtextxy(mijlocx - width / 2, 7 * OFFSET, "PLAYER2 WON");
                stareaJoc.playerScore2 += 1;
             }


             setfillstyle(SOLID_FILL, RED);
             int width = textwidth("PLAYER1 WON");
             settextstyle(8,0,3);
             int width1 = textwidth("Back");
             int width2 = textwidth("Play again");
             int height1 = textheight("Back");
             int height2 = textheight("Play again");
             int start1 = mijlocx - width / 2;;
             int finish1 = start1 + width1 + 20;
             int start2 = finish1 + 30;
             int finish2 = start2 + width2 + 20;
             int y1 = 8 * OFFSET;
             int y2 = 8 * OFFSET + height1 + 20;
             int y3 = 8 * OFFSET + height2 + 20;
             rectangle(start1, y1, finish1, y2);
             floodfill(start1 + 1, y1 + 1, WHITE);
             setbkcolor(RED);
             outtextxy( (start1 + finish1) / 2 - width1 / 2, (y1 + y2) / 2 - height1 / 2 ,"Back");

             rectangle(start2, y1, finish2, y3);
             floodfill(start2 + 1, y1 + 1, WHITE);
             setbkcolor(RED);
             outtextxy( (start2 + finish2)/2 - width2 / 2, (y1 + y3) / 2 - height2 / 2 ,"Play again");

             while(1) {
                if(ismouseclick(WM_LBUTTONDOWN)) {
                    int x, y;
                    getmouseclick(WM_LBUTTONDOWN, x, y);
                    if( x > start2 && x < finish2 && y > y1 && y < y2) {
                        closegraph();
                        delay(200);
                        initwindow(stareaJoc.screenWidth, stareaJoc.screenHeight);
                        eliberareStructura(0);
                        initalizarePunte();
                    }
                    else if(x > start1 && x < finish1 && y > y1 && y < y3) {
                        closegraph();
                        delay(200);
                        eliberareStructura(1);
                        Play();
                    }

                }
             }

          }
        }
    }
}

void Easy() {

    delay(500);
    bool findPoint = false;
    int coordX, coordY, contor = 0;
    for(int i=1; i <= stareaJoc.nrPunte; i++) {
        for(int j = i + 1; j <= stareaJoc.nrPunte; j++) {
            if(stareaJoc.adiacenta[i][j] == 0 && stareaJoc.vizitat[i] == 0 && stareaJoc.vizitat[j] == 0 && contor < stareaJoc.nrPunte) {
                findPoint = true;
                coordX = i;
                coordY = j;
                contor++;
            }
       }
    }

    if(findPoint) {
        findPoints(stareaJoc.puncte[coordX].x, stareaJoc.puncte[coordX].y, stareaJoc.puncte[coordY].x, stareaJoc.puncte[coordY].y);
    }
}

void MEDIUM()
{   delay(500);
	stareaJocPosibila=stareaJoc;
    stareaJocPosibila.posibleCombination = 0;
    coordonate LastValidX,LastValidY ;
    int i,j,m,n;
    bool FoundSol=false;
    for( i=1;i <= stareaJocPosibila.nrPunte; i++)
    {
        if (stareaJocPosibila.vizitat[i]==0)
            for( j = i + 1; j <= stareaJocPosibila.nrPunte; j++)
            {
                stareaJocPosibila.posibleCombination=0;
                if (stareaJocPosibila.vizitat[j]==0 && stareaJoc.adiacenta[i][j] != 3 && stareaJoc.adiacenta[i][j] != 4 && stareaJoc.adiacenta[j][i] != 4)
                    for( m=1;m <= stareaJocPosibila.nrPunte; m++)
                    {
                        for( n = m + 1; n <= stareaJocPosibila.nrPunte; n++)
                        {
                            if(stareaJocPosibila.vizitat[m] == 0 && stareaJocPosibila.vizitat[n] == 0) {
                              int d1 = direction(stareaJocPosibila.puncte[m], stareaJocPosibila.puncte[i], stareaJocPosibila.puncte[j]);
                              int d2 = direction(stareaJocPosibila.puncte[n], stareaJocPosibila.puncte[i], stareaJocPosibila.puncte[j]);
                              int d3 = direction(stareaJocPosibila.puncte[i], stareaJocPosibila.puncte[m], stareaJocPosibila.puncte[n]);
                              int d4 = direction(stareaJocPosibila.puncte[j], stareaJocPosibila.puncte[m], stareaJocPosibila.puncte[n]);

                              if(((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
                                stareaJocPosibila.adiacenta[m][n] = 4;
                                stareaJocPosibila.adiacenta[n][m] = 4;
                              }
                            }

                            if(stareaJocPosibila.adiacenta[m][n] == 0 && stareaJocPosibila.vizitat[m] == 0 && stareaJocPosibila.vizitat[n] == 0){
                                  stareaJocPosibila.posibleCombination += 1;
                                  LastValidX=stareaJocPosibila.puncte[i];
                                  LastValidY=stareaJocPosibila.puncte[j];
                            }
                       }
                  }
                if(stareaJocPosibila.posibleCombination%2==0 && stareaJocPosibila.posibleCombination!=0)
                    {

                        FoundSol=true;

                        break;
                    }

            }
            if(FoundSol)
                break;
        }
        //cout<<stareaJocPosibila.posibleCombination<<"\n";
        if(i == stareaJocPosibila.nrPunte+1 && j == stareaJocPosibila.nrPunte+1)
            findPoints(LastValidX.x,LastValidX.y,LastValidY.x,LastValidY.y);
        else
            findPoints(stareaJocPosibila.puncte[i].x,stareaJocPosibila.puncte[i].y,stareaJocPosibila.puncte[j].x,stareaJocPosibila.puncte[j].y);
}
void HARD()
{
    delay(500);
	stareaJocPosibila=stareaJoc;
    stareaJocPosibila.posibleCombination = 0;
    int mutari=0;
    coordonate LastValidX,LastValidY ;
    int i,j,m,n,ii,jj,mm,nn;
    bool FoundSol=false;
	for( i=1;i <= stareaJocPosibila.nrPunte; i++)
	{
		if (stareaJocPosibila.vizitat[i]==0)
			for( j = i + 1; j <= stareaJocPosibila.nrPunte; j++)
			{
				//stareaJocPosibila.posibleCombination=0;
				if (stareaJocPosibila.vizitat[j]==0 && stareaJoc.adiacenta[i][j] != 3 && stareaJoc.adiacenta[i][j] != 4 && stareaJoc.adiacenta[j][i] != 4)
					for( m=1;m <= stareaJocPosibila.nrPunte; m++)
					{
						for( n = m + 1; n <= stareaJocPosibila.nrPunte; n++)
						{
							if(stareaJocPosibila.vizitat[m] == 0 && stareaJocPosibila.vizitat[n] == 0) {
							  int d1 = direction(stareaJocPosibila.puncte[m], stareaJocPosibila.puncte[i], stareaJocPosibila.puncte[j]);
							  int d2 = direction(stareaJocPosibila.puncte[n], stareaJocPosibila.puncte[i], stareaJocPosibila.puncte[j]);
							  int d3 = direction(stareaJocPosibila.puncte[i], stareaJocPosibila.puncte[m], stareaJocPosibila.puncte[n]);
							  int d4 = direction(stareaJocPosibila.puncte[j], stareaJocPosibila.puncte[m], stareaJocPosibila.puncte[n]);

							  if(((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
								stareaJocPosibila.adiacenta[m][n] = 4;
								stareaJocPosibila.adiacenta[n][m] = 4;
							  }
							}

							if(stareaJocPosibila.adiacenta[m][n] == 0 && stareaJocPosibila.vizitat[m] == 0 && stareaJocPosibila.vizitat[n] == 0){
								 // stareaJocPosibila.posibleCombination += 1;
								  LastValidX=stareaJocPosibila.puncte[i];
								  LastValidY=stareaJocPosibila.puncte[j];

                                        //simulez mutare AI
                                         // stareaJocPosibila.adiacenta[m][n] = stareaJocPosibila.playerTurn;
                                         // stareaJocPosibila.adiacenta[n][m] = stareaJocPosibila.playerTurn;
                                          stareaJocPosibila.vizitat[m] = 1;
                                          stareaJocPosibila.vizitat[n] = 1;
                                          mutari=0;
                                        //  stareaJocPosibila.blockPoints -= 2;
                                         // segmentIntersection(xx, yy);
                                        //simulez mutare HUMAN
                                                for( ii=1;ii <= stareaJocPosibila.nrPunte; ii++)
                                                {
                                                    if (stareaJocPosibila.vizitat[ii]==0)
                                                        for( jj = ii + 1; jj <= stareaJocPosibila.nrPunte; jj++)
                                                        {
                                                           // mutari=0;
                                                            if (stareaJocPosibila.vizitat[jj]==0 && stareaJoc.adiacenta[ii][jj] != 3 && stareaJoc.adiacenta[ii][jj] != 4 && stareaJoc.adiacenta[jj][ii] != 4)
                                                                for( mm=1;mm <= stareaJocPosibila.nrPunte; mm++)
                                                                {
                                                                    for( nn = mm + 1; nn <= stareaJocPosibila.nrPunte; nn++)
                                                                    {
                                                                        if(stareaJocPosibila.vizitat[mm] == 0 && stareaJocPosibila.vizitat[nn] == 0) {
                                                                          int d1 = direction(stareaJocPosibila.puncte[mm], stareaJocPosibila.puncte[ii], stareaJocPosibila.puncte[jj]);
                                                                          int d2 = direction(stareaJocPosibila.puncte[nn], stareaJocPosibila.puncte[ii], stareaJocPosibila.puncte[jj]);
                                                                          int d3 = direction(stareaJocPosibila.puncte[ii], stareaJocPosibila.puncte[mm], stareaJocPosibila.puncte[nn]);
                                                                          int d4 = direction(stareaJocPosibila.puncte[jj], stareaJocPosibila.puncte[mm], stareaJocPosibila.puncte[nn]);

                                                                          if(((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
                                                                            stareaJocPosibila.adiacenta[mm][nn] = 4;
                                                                            stareaJocPosibila.adiacenta[nn][mm] = 4;
                                                                          }
                                                                        }

                                                                        if(stareaJocPosibila.adiacenta[mm][nn] == 0 && stareaJocPosibila.vizitat[mm] == 0 && stareaJocPosibila.vizitat[nn] == 0){
                                                                              mutari ++;
                                                                           //   LastValidX=stareaJocPosibila.puncte[ii];
                                                                           //   LastValidY=stareaJocPosibila.puncte[jj];



                                                                        }
                                                                   }
                                                              }



                                                        }


                                                    }
                                               if(mutari==0)   //daca nu sunt mutari de efectuat de catre HUMAN => solutie buna pentru AI
                                                    {FoundSol=true;

                                                    }
                                    //am terminat de simulat mutarea facuta de HUMAN

							//stareaJocPosibila.adiacenta[m][n] = 0;
                           // stareaJocPosibila.adiacenta[n][m] = 0;
                            stareaJocPosibila.vizitat[m] = 0;
                            stareaJocPosibila.vizitat[n] = 0;
                             if (FoundSol)
                                    break;
                            }

                        //    if (FoundSol) //||(stareaJocPosibila.posibleCombination==0))
                        //        break;
                                //{ FoundSol=true;break;}

						}
                        if (FoundSol)
                              break;
					}
                    if (FoundSol)
                        break;
			}
            if (FoundSol)
                break;
	}







            findPoints(LastValidX.x,LastValidX.y,LastValidY.x,LastValidY.y);


}

void startJoc() {
    setcolor(stareaJoc.playerColor[stareaJoc.playerTurn - 1]);
    checkColiniarity();

    while(1) {
        int x1, y1, x2, y2, x, y;
        int inceput = ((OFFSET + stareaJoc.screenWidth) / 2 + OFFSET);
        int height1 = textheight("Save and exit");
        int textwidth1 = textwidth("Save and exit");
        if(ismouseclick(WM_LBUTTONUP) || (stareaJoc.playerTurn==2 && stareaJoc.playMode == 3) || (stareaJoc.playerTurn==2 && stareaJoc.playMode == 2) || (stareaJoc.playerTurn==2 && stareaJoc.playMode == 4)) {
            if( mesaj.msg != "") {
                char aux[200] = "";
                for(int i=0; i < mesaj.lungime; i++)
                    strcat(aux, " ");
                outtextxy(OFFSET, stareaJoc.screenHeight - OFFSET + 10, aux);

            }
            getmouseclick(WM_LBUTTONDOWN, x1, y1);
            getmouseclick(WM_LBUTTONUP, x2, y2);
            if(x1 > inceput + OFFSET && x1 < inceput + OFFSET + textwidth1 + 20 && y1 > 2 * OFFSET && y1 < 2 * OFFSET + height1 + 20){
                salvareaFisier();break;
            }

            if(stareaJoc.playerTurn==2 && stareaJoc.playMode == 4)
                HARD();
            else
            if(stareaJoc.playerTurn==2 && stareaJoc.playMode == 3)
               MEDIUM();
            else
            if(stareaJoc.playerTurn == 2 && stareaJoc.playMode == 2)
               Easy();
            else
               findPoints(x1, y1, x2, y2);

        }
    }
}

void desenareTabla(int dx) {
    setcolor(GREEN);
    setlinestyle(0, 0, 3);
    rectangle(OFFSET, OFFSET, (OFFSET + stareaJoc.screenWidth) / 2 + OFFSET, stareaJoc.screenHeight - OFFSET);
    setlinestyle(0, 0, 2);

    for(int i=1; i <= stareaJoc.nrPunte; i++) {
      setcolor(YELLOW);
      setfillstyle(SOLID_FILL, LIGHTCYAN);
      circle(stareaJoc.puncte[i].x, stareaJoc.puncte[i].y, RADIUS);
      floodfill(stareaJoc.puncte[i].x, stareaJoc.puncte[i].y, YELLOW);
    }

    settextstyle(8, 0, 1);
    int inceput = ((OFFSET + stareaJoc.screenWidth) / 2 + OFFSET);
    int textwidth1 = textwidth("Save and exit");
    int textheight1 = textheight("Save and exit");
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, RED);
    rectangle(inceput + OFFSET, 2 * OFFSET, inceput + OFFSET + textwidth1 + 20, 2 * OFFSET + textheight1 + 20);
    floodfill(inceput + OFFSET + 1, 2 * OFFSET + 1, WHITE);
    setbkcolor(RED);
    outtextxy( (2 * (inceput + OFFSET) + textwidth1 + 20) / 2 - textwidth1 / 2, (2 *(2 * OFFSET) + textheight1 + 20) / 2 - textheight1 / 2 ,"Save and exit");

    setbkcolor(BLACK);
    char sir[20] = "";
    if(stareaJoc.playerTurn == 1 || dx == 1)
        strcpy(sir, "Player1");
    else if(stareaJoc.playerTurn == 2)
        strcpy(sir, "Player2");
    int inceput1 = inceput + OFFSET + textwidth1 + 20 + 40;
    inceput2 = inceput1 + textwidth("Player Turn:") + 20;
    outtextxy(inceput1, (2 * OFFSET + 2 * OFFSET + textheight1 + 20) / 2 - textheight("Player Turn:") / 2, "Player Turn:");
    setbkcolor(RED);

    rectangle(inceput2 , 2 * OFFSET, inceput2 + textwidth("Player1") + 20, 2 * OFFSET + textheight("Player1") + 20);

    floodfill(inceput2 + 1, 2 * OFFSET + 1, WHITE);
    outtextxy((inceput2 + inceput2 + textwidth("Player1") + 20) / 2 - textwidth("Player1") / 2, (2 *(2 * OFFSET) + textheight("Player1") + 20) / 2 - textheight("Player1") / 2, sir);

    setbkcolor(BLACK);
    settextstyle(8, 0, 5);
    setcolor(WHITE);
    int mijlocx = (((OFFSET + stareaJoc.screenWidth) / 2 + OFFSET) + stareaJoc.screenWidth) / 2;
    int width = textwidth("SEGEMENTS");
    outtextxy(mijlocx - width / 2, 3 * OFFSET, "SEGEMENTS");

    settextstyle(8, 0, 3);
    char aux[10];
    char str1[20] = "PLAYER1 - ";
    char str2[20] = "PLAYER2 - ";
    sprintf(aux, "%d", stareaJoc.playerScore1);
    strcat(str1, aux);
    outtextxy(inceput + OFFSET , 4.5 * OFFSET, str1);
    sprintf(aux, "%d", stareaJoc.playerScore2);
    strcat(str2, aux);
    outtextxy(inceput + OFFSET, 5.5 * OFFSET, str2);
    if(dx == 1) {
      stareaJoc.playerTurn = 1;
      startJoc();
    }
    else {
        for(int i=1; i<=stareaJoc.nrPunte; i++)
        {
            for(int j= i + 1; j <= stareaJoc.nrPunte; j++)
                if(stareaJoc.adiacenta[i][j] == 1 || stareaJoc.adiacenta[i][j] == 2) {
                    setcolor(stareaJoc.playerColor[stareaJoc.adiacenta[i][j] - 1]);
                    line(stareaJoc.puncte[i].x, stareaJoc.puncte[i].y, stareaJoc.puncte[j].x, stareaJoc.puncte[j].y);
                }
        }
        setcolor(stareaJoc.playerColor[stareaJoc.playerTurn - 1]);
        startJoc();
    }
}

void initalizarePunte() {

    srand(time(NULL));
    stareaJoc.nrPunte = rand() % stareaJoc.numberPts2 + stareaJoc.numberPts1;
    stareaJoc.blockPoints = stareaJoc.nrPunte;
    int y = (stareaJoc.screenHeight - OFFSET - 15) - (OFFSET + 15);
    int x = ((OFFSET + stareaJoc.screenWidth) / 2 + OFFSET - 15) - (OFFSET + 15);
    for(int i=1;i <= stareaJoc.nrPunte; i++) {
        coordonate pct;
        pct.x = rand() % x + (OFFSET + 15);
        pct.y = rand() % y + (OFFSET + 15);
        while(detectareaDistanta(pct, i-1)) {
            pct.x = rand() % x + (OFFSET + 15);
            pct.y = rand() % y + (OFFSET + 15);
        }
        stareaJoc.puncte[i].x = pct.x;
        stareaJoc.puncte[i].y = pct.y;
     }
     sortarePuncte();
     desenareTabla(1);
}
/////////////////////////////////////////////////////////
void efectClick(int color,int buton)
{
    if(buton==1)
    {
    settextstyle(8,0,3);
    setfillstyle(SOLID_FILL, color);
    rectangle(40,40,120,80);
    floodfill(41,41,WHITE);


    setbkcolor(color);
    setcolor(BLACK);
    outtextxy(50,50,"Play");
    }
    else
    if(buton==2)
    {
    setfillstyle(SOLID_FILL, color);
    rectangle(160,100,435,140);
    floodfill(161,101,WHITE);


    setbkcolor(color);
    setcolor(BLACK);
    outtextxy(180,110,"Player vs Player");
    }
    else
    if(buton==3)
    {
    setfillstyle(SOLID_FILL, color);
    rectangle(140,150,460,190);
    floodfill(141,151,WHITE);


    setbkcolor(color);
    setcolor(BLACK);
    outtextxy(160,160,"Player vs AI (Easy)");
    }
    else
    if(buton==4)
    {
    setfillstyle(SOLID_FILL, color);
    rectangle(130,200,470,240);
    floodfill(131,201,WHITE);


    setbkcolor(color);
    setcolor(BLACK);
    outtextxy(150,210,"Player vs AI (Medium)");
    }
    else
    if(buton==5)
    {
    setfillstyle(SOLID_FILL, color);
    rectangle(140,250,460,290);
    floodfill(141,251,WHITE);


    setbkcolor(color);
    setcolor(BLACK);
    outtextxy(160,260,"Player vs AI (Hard)");
    }
}
void dupaApasare1(int buton);
void dupaApasare2(int buton)
{

    int x,y;
  char miniP[10],maxiP[10];
    stareaJoc.numberPts1=10;
    stareaJoc.numberPts2=20;

    initwindow(1000,600);
    readimagefile("backround3window.jpg",0,0,1000,600);
    settextstyle(8,0,3);
    setbkcolor(BLACK);
    outtextxy(33,20,"Settings for next game");
    //
    setfillstyle(SOLID_FILL, RED);
    rectangle(40,50,135,90);
    floodfill(41,51,WHITE);
    setbkcolor(RED);
    outtextxy(50,60,"Color");
    //
    setfillstyle(SOLID_FILL, BLUE);
    rectangle(150,50,190,90);
    floodfill(151,51,WHITE);

    setfillstyle(SOLID_FILL, YELLOW);
    rectangle(200,50,240,90);
    floodfill(201,51,WHITE);

    setfillstyle(SOLID_FILL, RED);
    rectangle(250,50,290,90);
    floodfill(251,51,WHITE);

    setfillstyle(SOLID_FILL, GREEN);
    rectangle(300,50,340,90);
    floodfill(301,51,WHITE);
    ///////
    setfillstyle(SOLID_FILL, RED);
    rectangle(40,110,135,150);
    floodfill(41,111,WHITE);
    outtextxy(50,120,"Music");
    //
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    rectangle(150,110,240,150);
    floodfill(151,111,WHITE);
    setbkcolor(LIGHTGRAY);
    setcolor(BLACK);
    outtextxy(160,120,"Song1");
    //
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    rectangle(250,110,340,150);
    floodfill(251,111,WHITE);
    setbkcolor(LIGHTGRAY);
    setcolor(BLACK);
    outtextxy(260,120,"Song2");
    //
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    rectangle(350,110,440,150);
    floodfill(351,111,WHITE);
    setbkcolor(LIGHTGRAY);
    setcolor(BLACK);
    outtextxy(360,120,"Song3");
    //
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, DARKGRAY);
    rectangle(450,110,490,150);
    floodfill(451,111,WHITE);
    setbkcolor(DARKGRAY);
    setcolor(BLACK);
    outtextxy(463,120,"X");
    //
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, RED);
    rectangle(40,170,300,210);
    floodfill(41,171,WHITE);
    setbkcolor(RED);
    outtextxy(50,180,"Number of Points");
    //
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    rectangle(310,170,350,210);
    floodfill(311,171,WHITE);
    setbkcolor(LIGHTGRAY);
    setcolor(BLACK);
    outtextxy(323,180,"<");
    //
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    rectangle(360,170,400,210);
    floodfill(361,171,WHITE);
    setbkcolor(LIGHTGRAY);
    setcolor(BLACK);
    outtextxy(373,180,">");
    //
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, LIGHTBLUE);
    rectangle(410,170,550,210);
    floodfill(411,171,WHITE);
    setbkcolor(LIGHTBLUE);
    setcolor(BLACK);
    sprintf(miniP, "%d", stareaJoc.numberPts1);
    sprintf(maxiP, "%d", stareaJoc.numberPts2);
    outtextxy(435,180,(char*)miniP);
    outtextxy(473,180,"-");
    outtextxy(495,180,(char*)maxiP);
    //
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    rectangle(560,170,600,210);
    floodfill(561,171,WHITE);
    setbkcolor(LIGHTGRAY);
    setcolor(BLACK);
    outtextxy(573,180,"<");
    //
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    rectangle(610,170,650,210);
    floodfill(611,171,WHITE);
    setbkcolor(LIGHTGRAY);
    setcolor(BLACK);
    outtextxy(623,180,">");
    //
    setcolor(WHITE);
    setbkcolor(BLACK);
    outtextxy(33,250,"Player 2 / AI: ");

    setfillstyle(SOLID_FILL, RED);
    rectangle(40,280,135,320);
    floodfill(41,281,WHITE);
    setbkcolor(RED);
    outtextxy(50,290,"Color");

    setfillstyle(SOLID_FILL, BLUE);
    rectangle(150,280,190,320);
    floodfill(151,281,WHITE);

    setfillstyle(SOLID_FILL, YELLOW);
    rectangle(200,280,240,320);
    floodfill(201,281,WHITE);

    setfillstyle(SOLID_FILL, RED);
    rectangle(250,280,290,320);
    floodfill(251,281,WHITE);

    setfillstyle(SOLID_FILL, GREEN);
    rectangle(300,280,340,320);
    floodfill(301,281,WHITE);
    //
    if(stareaJoc.playMode==1)
    {
        setcolor(YELLOW);
        setbkcolor(BLACK);
        outtextxy(30,450,"Current Game Mode: Player vs Player");
    }
    else
    if(stareaJoc.playMode==2)
    {
        setcolor(YELLOW);
        setbkcolor(BLACK);
        outtextxy(30,450,"Current Game Mode: Player vs AI (Easy)");
    }
    else
    if(stareaJoc.playMode==3)
    {
        setcolor(YELLOW);
        setbkcolor(BLACK);
        outtextxy(30,450,"Current Game Mode: Player vs AI (Medium)");
    }
    else
    if(stareaJoc.playMode==4)
    {
        setcolor(YELLOW);
        setbkcolor(BLACK);
        outtextxy(30,450,"Current Game Mode: Player vs AI (Hard)");
    }


    //////
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, GREEN);
    rectangle(800,500,890,545);
    floodfill(801,501,WHITE);
    setbkcolor(GREEN);
    outtextxy(810,510,"Start");
    //
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, RED);
    rectangle(100,500,180,545);
    floodfill(101,501,WHITE);
    setbkcolor(RED);
    outtextxy(110,510,"Back");
    //////

    int color1=15;
    int color2=15;
    while(color1!=0)
    {

    if (ismouseclick(WM_LBUTTONDOWN))
	{
	    getmouseclick(WM_LBUTTONDOWN, x, y);
	    if(x>310 && x<350 && y>170 && y<210 && stareaJoc.numberPts1>10)
        {
            stareaJoc.numberPts1-=10;
            setcolor(BLACK);
            setbkcolor(LIGHTBLUE);
            sprintf(miniP, "%d", stareaJoc.numberPts1);
            outtextxy(435,180,(char*)miniP);
        }
        else
        if(x>360 && x<400 && y>170 && y<210 && stareaJoc.numberPts1<90 && stareaJoc.numberPts1<stareaJoc.numberPts2)
        {
            stareaJoc.numberPts1+=10;
            setcolor(BLACK);
            setbkcolor(LIGHTBLUE);
            sprintf(miniP, "%d", stareaJoc.numberPts1);
            outtextxy(435,180,(char*)miniP);
        }
        else
        if(x>560 && x<600 && y>170 && y<210 && stareaJoc.numberPts2>10 && stareaJoc.numberPts1<stareaJoc.numberPts2)
        {
            stareaJoc.numberPts2-=10;
            setcolor(BLACK);
            setbkcolor(LIGHTBLUE);
            sprintf(maxiP, "%d", stareaJoc.numberPts2);
            outtextxy(495,180,(char*)maxiP);
        }
        else
        if(x>610 && x<650 && y>170 && y<210 && stareaJoc.numberPts2<60)
        {
            stareaJoc.numberPts2+=10;
            setcolor(BLACK);
            setbkcolor(LIGHTBLUE);
            sprintf(maxiP, "%d", stareaJoc.numberPts2);
            outtextxy(495,180,(char*)maxiP);
        }
        else
	    if(x>150 && x<190 && y>50 && y<90 && color2!=BLUE)
	    {
	        stareaJoc.playerColor[0] = 1;
	        color1=BLUE;
	        setcolor(WHITE);
	        rectangle(148,48,192,92);
	        rectangle(147,47,193,93);
	        //
	        setcolor(BLACK);
	        rectangle(198,48,242,92);
	        rectangle(197,47,243,93);

	        rectangle(248,48,292,92);
	        rectangle(247,47,293,93);

	        rectangle(298,48,342,92);
	        rectangle(297,47,343,93);
	        //
	        setcolor(WHITE);
	        line(152,282,188,318);
	        line(152,318,188,282);
	        //
	        setcolor(YELLOW);
	        line(202,282,238,318);
	        line(202,318,238,282);
	        setcolor(RED);
	        line(252,282,288,318);
	        line(252,318,288,282);
	        setcolor(GREEN);
	        line(302,282,338,318);
	        line(302,318,338,282);
	       // efectClick(GREEN,2);delay(100);
	        //closegraph();
	        //dupaApasare2(button1_clicked);

        }
        else
        if(x>200 && x<240 && y>50 && y<90 && color2!=YELLOW)
	    {
	        stareaJoc.playerColor[0] = 14;
	        color1=YELLOW;
	        setcolor(WHITE);
	        rectangle(198,48,242,92);
	        rectangle(197,47,243,93);
	        //
	        setcolor(BLACK);
	        rectangle(148,48,192,92);
	        rectangle(147,47,193,93);

	        rectangle(248,48,292,92);
	        rectangle(247,47,293,93);

	        rectangle(298,48,342,92);
	        rectangle(297,47,343,93);
	        //
            setcolor(BLACK);
	        line(202,282,238,318);
	        line(202,318,238,282);
	        //
            setcolor(RED);
	        line(252,282,288,318);
	        line(252,318,288,282);
	        setcolor(GREEN);
	        line(302,282,338,318);
	        line(302,318,338,282);
            setcolor(BLUE);
	        line(152,282,188,318);
	        line(152,318,188,282);
	       // efectClick(GREEN,2);delay(100);
	        //closegraph();
	        //dupaApasare2(button1_clicked);

        }
        else
        if(x>250 && x<290 && y>50 && y<90 && color2!=RED)
	    {
	        stareaJoc.playerColor[0] = 4;
	        color1=RED;
	        setcolor(WHITE);
	        rectangle(248,48,292,92);
	        rectangle(247,47,293,93);
	        //
	        setcolor(BLACK);
	        rectangle(148,48,192,92);
	        rectangle(147,47,193,93);

	        rectangle(198,48,242,92);
	        rectangle(197,47,243,93);

	        rectangle(298,48,342,92);
	        rectangle(297,47,343,93);
	        //
	        setcolor(WHITE);
	        line(252,282,288,318);
	        line(252,318,288,282);
	        //
	        setcolor(GREEN);
	        line(302,282,338,318);
	        line(302,318,338,282);
            setcolor(BLUE);
	        line(152,282,188,318);
	        line(152,318,188,282);
            setcolor(YELLOW);
	        line(202,282,238,318);
	        line(202,318,238,282);
	       // efectClick(GREEN,2);delay(100);
	        //closegraph();
	        //dupaApasare2(button1_clicked);

        }
        else
        if(x>300 && x<340 && y>50 && y<90 && color2!=GREEN)
	    {
	        stareaJoc.playerColor[0] = 2;
	        color1=GREEN;
	        setcolor(WHITE);
	        rectangle(298,48,342,92);
	        rectangle(297,47,343,93);
	        //
	        setcolor(BLACK);
	        rectangle(148,48,192,92);
	        rectangle(147,47,193,93);

	        rectangle(198,48,242,92);
	        rectangle(197,47,243,93);

	        rectangle(248,48,292,92);
	        rectangle(247,47,293,93);
            //
	        setcolor(WHITE);
	        line(302,282,338,318);
	        line(302,318,338,282);
	        //
	        setcolor(BLUE);
	        line(152,282,188,318);
	        line(152,318,188,282);
            setcolor(YELLOW);
	        line(202,282,238,318);
	        line(202,318,238,282);
	        setcolor(RED);
	        line(252,282,288,318);
	        line(252,318,288,282);
	       // efectClick(GREEN,2);delay(100);
	        //closegraph();
	        //dupaApasare2(button1_clicked);

        }
        else
        if(x>150 && x<190 && y>280 && y<320 && color1!=BLUE)
	    {
	        stareaJoc.playerColor[1] = 1;
	        color2=BLUE;
	        setcolor(WHITE);
	        rectangle(148,278,192,322);
	        rectangle(147,277,193,323);
	        //
	        setcolor(BLACK);
	        rectangle(198,278,242,322);
	        rectangle(197,277,243,323);

	        rectangle(248,278,292,322);
	        rectangle(247,277,293,323);

	        rectangle(298,278,342,322);
	        rectangle(297,277,343,323);
	        //
	        setcolor(WHITE);
	        line(152,52,188,88);
	        line(152,88,188,52);
	        //
	        setcolor(YELLOW);
	        line(202,52,238,88);
	        line(202,88,238,52);
	        setcolor(RED);
	        line(252,52,288,88);
	        line(252,88,288,52);
	        setcolor(GREEN);
	        line(302,52,338,88);
	        line(302,88,338,52);
	       // efectClick(GREEN,2);delay(100);
	        //closegraph();
	        //dupaApasare2(button1_clicked);

        }
        else
        if(x>200 && x<240 && y>280 && y<320 && color1!=YELLOW)
	    {
	        stareaJoc.playerColor[1] = 14;
	        color2=YELLOW;
	        setcolor(WHITE);
	        rectangle(198,278,242,322);
	        rectangle(197,277,243,323);
	        //
	        setcolor(BLACK);
	        rectangle(148,278,192,322);
	        rectangle(147,277,193,323);

	        rectangle(248,278,292,322);
	        rectangle(247,277,293,323);

	        rectangle(298,278,342,322);
	        rectangle(297,277,343,323);
	        //
	        setcolor(BLACK);
	        line(202,52,238,88);
	        line(202,88,238,52);
	        //
	        setcolor(RED);
	        line(252,52,288,88);
	        line(252,88,288,52);
	        setcolor(GREEN);
	        line(302,52,338,88);
	        line(302,88,338,52);
	        setcolor(BLUE);
	        line(152,52,188,88);
	        line(152,88,188,52);
	       // efectClick(GREEN,2);delay(100);
	        //closegraph();
	        //dupaApasare2(button1_clicked);

        }
        else
        if(x>250 && x<290 && y>280 && y<320 && color1!=RED)
	    {
	        stareaJoc.playerColor[1] = 4;
	        color2=RED;
	        setcolor(WHITE);
	        rectangle(248,278,292,322);
	        rectangle(247,277,293,323);
	        //
	        setcolor(BLACK);
	        rectangle(148,278,192,322);
	        rectangle(147,277,193,323);

	        rectangle(198,278,242,322);
	        rectangle(197,277,243,323);

	        rectangle(298,278,342,322);
	        rectangle(297,277,343,323);
	        //
	        setcolor(WHITE);
	        line(252,52,288,88);
	        line(252,88,288,52);
	        //
	        setcolor(GREEN);
	        line(302,52,338,88);
	        line(302,88,338,52);
	        setcolor(BLUE);
	        line(152,52,188,88);
	        line(152,88,188,52);
	        setcolor(YELLOW);
	        line(202,52,238,88);
	        line(202,88,238,52);
	       // efectClick(GREEN,2);delay(100);
	        //closegraph();
	        //dupaApasare2(button1_clicked);

        }
        else
        if(x>300 && x<340 && y>280 && y<320 && color1!=GREEN)
	    {
	        stareaJoc.playerColor[1] = 2;
	        color2=GREEN;
	        setcolor(WHITE);
	        rectangle(298,278,342,322);
	        rectangle(297,277,343,323);
	        //
	        setcolor(BLACK);
	        rectangle(148,278,192,322);
	        rectangle(147,277,193,323);

	        rectangle(198,278,242,322);
	        rectangle(197,277,243,323);

	        rectangle(248,278,292,322);
	        rectangle(247,277,293,323);
	        //
	        setcolor(WHITE);
	        line(302,52,338,88);
	        line(302,88,338,52);
	        //
	        setcolor(BLUE);
	        line(152,52,188,88);
	        line(152,88,188,52);
	        setcolor(YELLOW);
	        line(202,52,238,88);
	        line(202,88,238,52);
	        setcolor(RED);
	        line(252,52,288,88);
	        line(252,88,288,52);
	       // efectClick(GREEN,2);delay(100);
	        //closegraph();
	        //dupaApasare2(button1_clicked);

        }
        else
        if(x>150 && x<240 && y>110 && y<150)
        {
            //PlaySound(NULL,NULL,SND_FILENAME | SND_ASYNC);
            //PlaySound(TEXT("song1.wav"),NULL,SND_FILENAME | SND_ASYNC);
            //
            //color=1;
            setcolor(YELLOW);
            rectangle(148,108,242,152);
            //
            setcolor(BLACK);
            rectangle(248,108,342,152);
            rectangle(348,108,442,152);
            rectangle(448,108,492,152);
        }
        else
        if(x>250 && x<340 && y>110 && y<150)
        {
            //PlaySound(NULL,NULL,SND_FILENAME | SND_ASYNC);
            //PlaySound(TEXT("song2.wav"),NULL,SND_FILENAME | SND_ASYNC);
            //color=2;
            setcolor(YELLOW);
            rectangle(248,108,342,152);
            //
            setcolor(BLACK);
            rectangle(148,108,242,152);
            rectangle(348,108,442,152);
            rectangle(448,108,492,152);
        }
        else
        if(x>350 && x<440 && y>110 && y<150)
        {
            //PlaySound(NULL,NULL,SND_FILENAME | SND_ASYNC);
            //PlaySound(TEXT("song3.wav"),NULL,SND_FILENAME | SND_ASYNC);
            //color=3;
            setcolor(YELLOW);
            rectangle(348,108,442,152);
            //
            setcolor(BLACK);
            rectangle(148,108,242,152);
            rectangle(248,108,342,152);
            rectangle(448,108,492,152);
        }
        else
        if(x>450 && x<490 && y>110 && y<150)
        {
            //PlaySound(NULL,NULL,SND_FILENAME | SND_ASYNC);
            setcolor(YELLOW);
            rectangle(448,108,492,152);
            //
            setcolor(BLACK);
            rectangle(148,108,242,152);
            rectangle(248,108,342,152);
            rectangle(348,108,442,152);
        }
        else
        if(x>100 && x<180 && y>500 && y<545)
        {
            closegraph();
            dupaApasare1(buton);
        }
        else
        if(x>800 && x<890 && y>500 && y<545)
	    {
	        closegraph();
	        initwindow(stareaJoc.screenWidth, stareaJoc.screenHeight);
            initalizarePunte();
            break;
          //  getch(); closegraph();

        }
	}
    }

   // getch(); closegraph();
}
//void Play ()
void dupaApasare1(int button)
{int x,y;

    initwindow(600,400);
    readimagefile("Window2bkround.jpg",0,0,600,400);


    //
    setcolor(WHITE);
    settextstyle(8,0,3);
    //
    setfillstyle(SOLID_FILL, RED);
    rectangle(160,100,435,140);
    floodfill(161,101,WHITE);
    setbkcolor(RED);
    outtextxy(180,110,"Player vs Player");
    //
    setfillstyle(SOLID_FILL, RED);
    rectangle(140,150,460,190);
    floodfill(141,151,WHITE);
    setbkcolor(RED);
    outtextxy(160,160,"Player vs AI (Easy)");
    //
    setfillstyle(SOLID_FILL, RED);
    rectangle(130,200,470,240);
    floodfill(131,201,WHITE);
    setbkcolor(RED);
    outtextxy(150,210,"Player vs AI (Medium)");
    //
    setfillstyle(SOLID_FILL, RED);
    rectangle(140,250,460,290);
    floodfill(141,251,WHITE);
    setbkcolor(RED);
    outtextxy(160,260,"Player vs AI (Hard)");
    //
    setfillstyle(SOLID_FILL, CYAN);
    rectangle(70,300,140,330);
    floodfill(71,301,WHITE);
    setbkcolor(CYAN);
    outtextxy(75,305,"Back");

    int ok=0;
    int rainbow=1;
    int mouseover=0;
    int button1_clicked=0;
    int button2_clicked=0;
    int button3_clicked=0;
    while (!button1_clicked || !button2_clicked || !button3_clicked)
    {
        if(rainbow==14)
        rainbow=1;
        if(ok==0)
        {
        setcolor(rainbow);
        rectangle(0,2,599,399);
        rectangle(1,3,598,398);
        rectangle(2,4,597,397);
        rectangle(3,5,596,396);
        //
        setcolor(rainbow+1);
        rectangle(18,20,581,381);
        rectangle(19,21,580,380);
        rectangle(20,22,579,379);
        rectangle(21,23,578,378);
        //
        setcolor(rainbow+2);
        rectangle(36,48,563,363);
        rectangle(37,49,562,362);
        rectangle(38,50,561,361);
        rectangle(39,51,560,360);
        //
        setcolor(rainbow+3);
        rectangle(54,69,542,342);
        rectangle(55,70,541,341);
        rectangle(56,71,540,340);
        rectangle(57,72,539,339);
        rainbow++;
        ok=500000;
        }
        ok--;
    if (SavePressed)
    {
        break;
        SavePressed=false;
    }
    if(mousex()>160 && mousex()<435 && mousey()>100 && mousey()<140 && !ismouseclick(WM_LBUTTONDOWN) && mouseover==0)
    {
            mouseover=1;
            setcolor(WHITE);
            setfillstyle(SOLID_FILL, LIGHTCYAN);
            rectangle(160,100,435,140);
            floodfill(161,101,WHITE);
            setcolor(BLACK);
            setbkcolor(LIGHTCYAN);
            outtextxy(180,110,"Player vs Player");

    }
    //buton1
    else
    if(mousex()>140 && mousex()<460 && mousey()>150 && mousey()<190 && !ismouseclick(WM_LBUTTONDOWN) && mouseover==0)
    {
            mouseover=1;
            setcolor(WHITE);
            setfillstyle(SOLID_FILL, LIGHTCYAN);
            rectangle(140,150,460,190);
            floodfill(141,151,WHITE);
            setcolor(BLACK);
            setbkcolor(LIGHTCYAN);
            outtextxy(160,160,"Player vs AI (Easy)");

    }
    //buton2
    else
    if(mousex()>130 && mousex()<470 && mousey()>200 && mousey()<240 && !ismouseclick(WM_LBUTTONDOWN) && mouseover==0)
    {
            mouseover=1;
            setcolor(WHITE);
            setfillstyle(SOLID_FILL, LIGHTCYAN);
            rectangle(130,200,470,240);
            floodfill(131,201,WHITE);
            setcolor(BLACK);
            setbkcolor(LIGHTCYAN);
            outtextxy(150,210,"Player vs AI (Medium)");


    }
    //buton3
    else
    if(mousex()>140 && mousex()<460 && mousey()>250 && mousey()<290 && !ismouseclick(WM_LBUTTONDOWN) && mouseover==0)
    {
            mouseover=1;
            setcolor(WHITE);
            setfillstyle(SOLID_FILL, LIGHTCYAN);
            rectangle(140,250,460,290);
            floodfill(141,251,WHITE);
            setcolor(BLACK);
            setbkcolor(LIGHTCYAN);
            outtextxy(160,260,"Player vs AI (Hard)");


    }
    //buton4
    else
    if((mousex()<160 || mousex()>435 || mousey()<100 || mousey()>140) && (mousex()<140 || mousex()>460 || mousey()<150 || mousey()>190) && (mousex()<130 || mousex()>470 || mousey()<200 || mousey()>240) && (mousex()<140 || mousex()>460 || mousey()<250 || mousey()>290) && !ismouseclick(WM_LBUTTONDOWN) && mouseover==1)
    {
            mouseover=0;
            setcolor(WHITE);
            setfillstyle(SOLID_FILL, RED);
            rectangle(160,100,435,140);
            floodfill(161,101,WHITE);
            setcolor(WHITE);
            setbkcolor(RED);
            outtextxy(180,110,"Player vs Player");
    //
                mouseover=0;
            setcolor(WHITE);
            setfillstyle(SOLID_FILL, RED);
            rectangle(140,150,460,190);
            floodfill(141,151,WHITE);
            setcolor(WHITE);
            setbkcolor(RED);
            outtextxy(160,160,"Player vs AI (Easy)");
    //
                mouseover=0;
            setcolor(WHITE);
            setfillstyle(SOLID_FILL, RED);
            rectangle(130,200,470,240);
            floodfill(131,201,WHITE);
            setcolor(WHITE);
            setbkcolor(RED);
            outtextxy(150,210,"Player vs AI (Medium)");
    //
                mouseover=0;
            setcolor(WHITE);
            setfillstyle(SOLID_FILL, RED);
            rectangle(140,250,460,290);
            floodfill(141,251,WHITE);
            setcolor(WHITE);
            setbkcolor(RED);
            outtextxy(160,260,"Player vs AI (Hard)");


    }
    else
    ////
	if (ismouseclick(WM_LBUTTONDOWN))
	{
	    setcolor(WHITE);
	    getmouseclick(WM_LBUTTONDOWN, x, y);
	    if(x>70 && x<140 && y>300 && y<330)
        {
            closegraph();
            Play();
        }
        else
	    if(x>160 && x<435 && y>100 && y<140)
	    {
	        button1_clicked = 2;
	        stareaJoc.playMode = 1;
	        efectClick(GREEN,2);delay(100);
	        closegraph();
	        dupaApasare2(button1_clicked);

        }
        else
        if(x>140 && x<460 && y>150 && y<190)
            {
	        button2_clicked = 3;
	        stareaJoc.playMode = 2;
	        efectClick(GREEN,3);delay(100);
	        closegraph();
	        dupaApasare2(button2_clicked);

        }
        else
        if(x>130 && x<470 && y>200 && y<240)
        {
            button3_clicked = 4;
            stareaJoc.playMode = 3;
	        efectClick(GREEN,4);delay(100);
	        closegraph();
	        dupaApasare2(button3_clicked);
        }
        else
        if(x>140 && x<460 && y>250 && y<290)
        {
            button3_clicked = 5;
            stareaJoc.playMode = 4;
	        efectClick(GREEN,5);delay(100);
	        closegraph();
	        dupaApasare2(button3_clicked);
	        break;
        }
	}

    }
   // getch();
   // closegraph();
}


void Play ()
{ int x,y,mouseover;
    initwindow(600,400);
    readimagefile("backround2jpg.jpg",0,0,600,400);
    settextstyle(8,0,3);
    //
    //PlaySound(TEXT("eminem1.wav"),NULL,SND_FILENAME | SND_ASYNC);
    //PlaySound(NULL,NULL,SND_FILENAME | SND_ASYNC);
   //
    setfillstyle(SOLID_FILL, RED);
    rectangle(40,40,120,80);
    floodfill(41,41,WHITE);
    setbkcolor(RED);
    outtextxy(50,50,"Play");

    int width = textwidth("Restore");
    int height = textheight("Restore");
    int mijx = (40 + 40 + width + 10) / 2;
    int mijy = (100 + 100 + height + 10) / 2;
    rectangle(40, 100, 40 + width + 10, 100 + height + 10);
    floodfill(41, 101, WHITE);
    setbkcolor(RED);
    outtextxy( mijx - width / 2, mijy - height / 2, "Restore");
//    settextstyle(8,0,30);
//    setbkcolor(BLACK);
//    outtextxy(120,100,"Segments");
//    //
//    circle(110,190,4);
//    circle(535,190,4);
//    line(113,190,530,190);
//    //
//    circle(150,250,4);
//    circle(270,350,4);
//    line(152,251,267,348);
//    //
//    circle(500,230,4);
//    circle(340,285,4);
//    line(497,231,342,283);



    mouseover=0;
    int buttonPlay_clicked=0;
    while (!buttonPlay_clicked)
    {

        //delay(500);

    if(mousex()>40 && mousex()<120 && mousey()>40 && mousey()<80 && !ismouseclick(WM_LBUTTONDOWN) && mouseover==0)
    {
            mouseover=1;
            setcolor(WHITE);
            settextstyle(8,0,3);
            setfillstyle(SOLID_FILL, LIGHTRED);
            rectangle(40,40,120,80);
            floodfill(41,41,WHITE);
            setcolor(BLACK);
            setbkcolor(LIGHTRED);
            outtextxy(50,50,"Play");

    }
    else
    if((mousex()<40 || mousex()>120 || mousey()<40 || mousey()>80) && !ismouseclick(WM_LBUTTONDOWN) && mouseover==1)
    {
            mouseover=0;
            setcolor(WHITE);
            setfillstyle(SOLID_FILL, RED);
            rectangle(40,40,120,80);
            floodfill(41,41,WHITE);
            setcolor(WHITE);
            setbkcolor(RED);
            outtextxy(50,50,"Play");

    }
    else
	 if (ismouseclick(WM_LBUTTONDOWN))
	 {
	    getmouseclick(WM_LBUTTONDOWN, x, y);
	    if(x>40 && x<120 && y>40 && y<80)
	    {
            setcolor(WHITE);
	        buttonPlay_clicked = 1;
	        efectClick(YELLOW,1);delay(100);
	        closegraph();
	        dupaApasare1(buttonPlay_clicked);
	        break;

        }
        else if(x > 40 && x < 40 + width + 10 && y > 100 && y < 100 + height + 10) {
            closegraph();
            initwindow(stareaJoc.screenWidth, stareaJoc.screenHeight);
            restoreFile();
        }
	 }
    }

   // getch();
   // closegraph();
}

int main()
{
    Play();
    return 0;
}

