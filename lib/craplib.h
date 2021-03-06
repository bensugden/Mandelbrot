#pragma once

typedef unsigned long       DWORD;

#define PI ((float)3.1415926535)



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 'system' - screen is always 800 x 600
// simply call StartFrame and Flip alternately to run the game; StartFrame returns false if the user is trying to quit
bool StartFrame(DWORD clearcolor=0);
void Flip();
int	GetTimeInMS(); // ...since start of program
void Game(); // you write this :)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// input
void GetMousePos(float &x, float &y); // 0,0 is top left; 800,600 is bottom right
bool IsKeyDown(int key); // use windows VK_ codes for special keys, eg VK_LEFT; use capital chars for letter keys eg 'A', '0'
bool IsKeyHitSinceLastFlip(int key);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// output
// 'sprite output' 
void *LoadTexture(const char *fname);
void SetCurrentTexture(void *tex = 0);
void DrawRectangle(float x1, float y1, float x2, float y2, DWORD col = 0xffffffff); // textured with 
void DrawSprite(float xcentre, float ycentre, float xsize, float ysize, float angle=0, DWORD col = 0xffffffff);
// 'text' output
int DrawSomeText(int x, int y, int size, int col, bool centered, const char *pFormat, ...); 
void* CreateTexture( int width, int height );
char* LockTexture( void* texT, int& pitch );
void UnlockTexture( void* texT );


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// sound
int PlayMusic(const char *fname, float volume=1); // returns a handle which you only need if you want to change volume later with ChangeVolume
void StopMusic();

// sorry for this being 'snd' but otherwise it clashes with a windows #define (PlaySound) grr
void *LoadSnd(const char *fname, bool loop=false); // if you set loop the sample will go on looping forever, until you call StopSound
int PlaySnd(void *sound, float volume=1); // returns a handle which you only need if you are going to call StopSound or ChangeVolume()
void StopSnd(int handle);
void ChangeVolume(int handle, float newvolume=1);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// geeks only beyond this point!
// 'advanced' output - for geeks only - for drawing arbitrarily textured triangles and line lists
// 'flat colour' output - no backface culling
void DrawLine(float x1, float y1, float x2, float y2, DWORD col = 0xffffffff); // no texture
void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, DWORD col = 0xffffffff); // flat colored tri, no texture

struct Vertex 
{
	float x,y;
	DWORD colour;
	float u,v;
};
void DrawTriangleList(Vertex *verts, int numtris);
void DrawLineList(Vertex *verts, int numlines);
