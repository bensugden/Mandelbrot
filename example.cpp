#include "lib/craplib.h"
#include <stdlib.h>	// for rand
#include <windows.h> // for the VK_ codes
#include <math.h>		// for sin and cos

float angle = 0;

float tx1=10,ty1=20,tx2=50,ty2=50,tx3=5,ty3=100;
int screen_x = 800;
int screen_y = 600;
double scale = 1.0f/200.f;
double centre_x = 0.0f;
double centre_y = 0.0f;
int iterations = 256;

double GetRealY(int y)
{
	return centre_y + (double(y - screen_y / 2)) * scale;
}

double GetRealX(int x)
{
	return  centre_x + (double(x - screen_x / 2)) * scale;
}

int EvalMandlebrot( double xx, double yy )
{
	int i = 0;
	double v = 0;
	double u = 0;
	double u2 = 0.0;
	double v2 = 0.0;
	for (; i < iterations; i++)
	{
		v = 2 * u * v + yy;
		u = u2 - v2 + xx;
		u2 = u * u;
		v2 = v * v;

		if ((u2 + v2) > 4.0f)
		{
			break;
		}
	}
	return i;
}

const int fix_iterations = 16;

#define SQR( a ) ( (a) * (a) )
void Game()
{
	srand( 12361531 );

	int* its[2];
	double* x_pos, *y_pos;
	double* x_posRead, *y_posRead;
	int* read;
	int* write;
	its[ 0 ] = new int[ screen_x * screen_y ];
	its[ 1 ] = new int[ screen_x * screen_y ];
	int iBuffer=0;
	int iXPosBuffer=0;
	int iYPosBuffer=0;
	double* x_posStore[2];
	x_posStore[0] = new double[ screen_x ];
	x_posStore[1] = new double[ screen_x ];
	double* y_posStore[2];
	y_posStore[0] = new double[ screen_y ];
	y_posStore[1] = new double[ screen_y ];


	bool bFirstFrame = true;
	void* texture = CreateTexture( screen_x, screen_y );
	
	write = its[ iBuffer ];
	x_pos = x_posStore[ iXPosBuffer ];
	y_pos = y_posStore[ iYPosBuffer ];

	while (StartFrame())
	{
		if (IsKeyHitSinceLastFlip(VK_ESCAPE)) break;

		int pitch;

		if (  IsKeyHitSinceLastFlip( VK_UP ) || IsKeyHitSinceLastFlip( VK_DOWN ) )
		{
			if ( IsKeyHitSinceLastFlip( VK_UP ) )
			{
				iterations += iterations / 2;
			}
			if ( IsKeyHitSinceLastFlip( VK_DOWN ) )
			{
				iterations -= iterations / 3;
			}
			
			for  ( int y = 0; y < screen_y; y++)
			{
				y_pos[ y ] = - 10000000.f;
			}
			for ( int x = 0; x < screen_x; x++ )
			{
				x_pos[ x ] = - 10000000.f;
			}
			bFirstFrame = true;
		}
		if ( bFirstFrame )
		{

			for  ( int y = 0; y < screen_y; y++)
			{
				y_pos[ y ] =  GetRealY( y );

				for ( int x = 0; x < screen_x; x++ )
				{
					x_pos[ x ] =  GetRealX( x );
					write[ x + y * screen_x ] = EvalMandlebrot( GetRealX( x ), GetRealY( y ) );
				}
			}
			bFirstFrame = false;
		}
		else
		if (IsKeyDown(VK_LBUTTON)||IsKeyDown(VK_RBUTTON))
		{			
			float mx,my;
			GetMousePos(mx,my);
			double pos_y = centre_y + (double(my -  screen_y / 2 )) * scale;
			double pos_x = centre_x + (double(mx -  screen_x / 2 )) * scale;

			if ( IsKeyDown(VK_RBUTTON) )
			{
				static double moveStartX ;
				static double moveStartY ;
				if ( IsKeyHitSinceLastFlip(VK_RBUTTON) )
				{
					moveStartX = pos_x;
					moveStartY = pos_y;
				}
				else 
				{
					centre_x += moveStartX - pos_x;
					centre_y += moveStartY - pos_y;
					moveStartX = pos_x;
					moveStartY = pos_y;
				}
			}
			else
			{
				if( IsKeyDown(VK_SHIFT) )
					scale /= 0.95;
				else
					scale *= 0.95;
				centre_x= pos_x - (double(mx -  screen_x / 2 )) * scale;
				centre_y= pos_y - (double(my -  screen_y / 2 )) * scale;
			}

			read = write;
			iBuffer ^= 1;
			write = its[ iBuffer ];

			y_posRead = y_pos;
			iYPosBuffer ^= 1;
			y_pos = y_posStore[ iYPosBuffer ];
			int t[800];
			// do best fit fill Y
			{
				int iCurrentY = 0;

				for  ( int yy = 0; yy < screen_y; yy++)
				{
					double flBestY = 10000000000;
					double flIdealY = GetRealY( yy );

					for ( int y = iCurrentY; y < screen_y; y++ )
					{
						double flRealY = y_posRead[ y ];
						double flDelta = SQR( flRealY - flIdealY );
						if (flDelta<=flBestY)
						{
							flBestY = flDelta;
							iCurrentY = y;
						}
						else
						{
							break;
						}
					}
					y_pos[ yy ] =  y_posRead[ iCurrentY ];
					t[yy]=iCurrentY;
					for ( int x = 0; x < screen_x; x++ )
					{
						write[ x + yy * screen_x ] = read[ x + iCurrentY * screen_x ];
					}
				}
			}
		
			read = write;
			iBuffer ^= 1;
			write = its[ iBuffer ];

			x_posRead = x_pos;
			iXPosBuffer ^= 1;
			x_pos = x_posStore[ iXPosBuffer ];

			// do best fit fill X
			{
				int iCurrentX = 0;

				for  ( int xx = 0; xx < screen_x; xx++)
				{
					double flIdealX = GetRealX( xx );
					double flBestX = 10000000000;
					for ( int x = iCurrentX; x < screen_x; x++ )
					{
						double flRealX = x_posRead[ x ];
						double flDelta = SQR( flRealX - flIdealX );
						if (flDelta<=flBestX)
						{
							flBestX = flDelta;
							iCurrentX = x;
						}
						else
						{
							break;
						}
					}
					x_pos[ xx ] =  x_posRead[ iCurrentX ];

					for ( int y = 0; y < screen_y; y++ )
					{
						write[ xx + y * screen_x ] = read[ iCurrentX + y * screen_x ];
					}
				}
			}
		}
		else
		{
			for ( int i = 0 ; i < fix_iterations; i++ )
			{
				// do worst fit fill Y
				{
					double flWorstY = 0.0f;
					int iWorstY = 0;
					for  ( int y = 0; y < screen_y; y++)
					{
						double flRealY = y_pos[ y ];
						double flIdealY = GetRealY( y );
						double flDelta = SQR( flRealY - flIdealY );
						if (flDelta>flWorstY)
						{
							flWorstY = flDelta;
							iWorstY = y;
						}
					}
					y_pos[ iWorstY ] =  GetRealY( iWorstY );

					for ( int x = 0; x < screen_x; x++ )
					{
						write[ x + iWorstY * screen_x ] = EvalMandlebrot( GetRealX( x ), GetRealY( iWorstY ) );
					}
				}
				// do worst fit fill X
				{
					double flWorstX = 0.0f;
					int iWorstX = 0;
					for  ( int x = 0; x < screen_x; x++)
					{
						double flRealX = x_pos[ x ];
						double flIdealX = GetRealX( x );
						double flDelta = SQR( flRealX - flIdealX );
						if (flDelta>flWorstX)
						{
							flWorstX = flDelta;
							iWorstX = x;
						}
					}
					x_pos[ iWorstX ] =  GetRealX( iWorstX );

					for ( int y = 0; y < screen_y; y++ )
					{
						write[ iWorstX + y * screen_x ] = EvalMandlebrot( GetRealX( iWorstX ), GetRealY( y ) );
					}
				}
			}
		}



		char* ctex = LockTexture( texture, pitch );
		for  ( int y = 0; y < screen_y; y++)
		{
			for ( int x = 0; x < screen_x; x++ )
			{
				int val= 255 - write[ x + y * screen_x ];

				ctex[ x * 4 + y * pitch + 0 ]= char(255);
				ctex[ x * 4 + y * pitch + 1 ]= val;
				ctex[ x * 4 + y * pitch + 2 ]= val;
				ctex[ x * 4 + y * pitch + 3 ]= val;
			}
		}
		UnlockTexture( texture );
		SetCurrentTexture( texture );
		DrawRectangle(0,0,800,600, 0xffffffff ); // colours are ALPHA RED GREEN BLUE, dont forget to set alpha to 0xff for solid!



		/*
		// draw a big rectangle
		SetCurrentTexture(mytex);
		DrawRectangle(0,0,800,600, 0xff000040); // colours are ALPHA RED GREEN BLUE, dont forget to set alpha to 0xff for solid!

		// draw a big cross across the screen
		DrawLine(0,0,800,600);
		DrawLine(800,0,0,600);

		// draw a rotating sprite in the middle of the screen
		DrawSprite(400,300,50,50,angle);
		angle+=0.01f;

		if (IsKeyHitSinceLastFlip(VK_LBUTTON))
		{
			PlaySnd(mysound);
		}
		if (IsKeyHitSinceLastFlip(VK_RBUTTON))
		{
			PlaySnd(mysound2);
		}

		// draw a er, triangle we can move with the mouse
		DrawTriangle(tx1,ty1,tx2,ty2,tx3,ty3,0x8000ff00); // transparent green! rocking!
		if (IsKeyDown(VK_LBUTTON))
		{			
			float mx,my;
			GetMousePos(mx,my);
			// find closest corner to mouse and move it
			float d1=(tx1-mx)*(tx1-mx)+(ty1-my)*(ty1-my);
			float d2=(tx2-mx)*(tx2-mx)+(ty2-my)*(ty2-my);
			float d3=(tx3-mx)*(tx3-mx)+(ty3-my)*(ty3-my);
			if (d1<d2 && d1<d3) {tx1=mx;ty1=my;}
			else if (d2<d1 && d2<d3)  {tx2=mx;ty2=my;}
			else {tx3=mx;ty3=my;}
		}

		// draw a linelist 'advanced output demo'
		Vertex vs[80];
		for (int c1=0;c1<80;c1++)
		{
			vs[c1].colour=0xff000000+rand()+(c1<<20);
			vs[c1].u=rand()/32768.f;
			vs[c1].v=rand()/32768.f;
			vs[c1].x=sinf(c1*0.4f+angle*12.f+234.f)*200+600;
			vs[c1].y=cosf(c1*-0.19f+angle*6.34f-134.f)*200+300;
		}
		DrawTriangleList(vs,80/3);
		DrawLineList(vs,80/2);

		// draw some text at the mouse cursor
		float mx,my;
		GetMousePos(mx,my);
		DrawSomeText((int)mx,(int)my,20,0xffff0000,true,"MOUSE %g,%g",mx,my);

		DrawSomeText((int)400,(int)0,20,0xffffffff,true,"click left and right mouse for sound!");
		*/

		Flip();
	}

	StopMusic();
}