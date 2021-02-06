#include <Windows.h>
#include <SDL.h>
#include <SDL_Image.h>
#include <iostream>
#include <string>
#include "Vectors.h"
#include "Color.h"

using namespace std;

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 859;
const int MAXCHECKVALUE = 600;
float scaler = 1;

SDL_Renderer* ren;
SDL_Window* win;

bool init();
SDL_Surface* loadTextureJPG(const char* file);
SDL_Surface* loadTexturePNG(const char* file);
SDL_Surface* loadTextureBMP(const char* file);
void renderTexture(SDL_Texture* tex, int x, int y, int w, int h);
void renderTexture(SDL_Texture* tex, int x, int y);
Uint8* copySurfacePixels(SDL_Surface* surface, Uint32 pixelFormat, SDL_Renderer* renderer, int* width, int* height, int* pitch);
void drawLine(Vector2 p1, Vector2 p2, Color col);
void drawRect(Vector2 p1, Vector2 p2, Color col);
int calcPixelCount(SDL_Surface* sur, Vector2 p1, Vector2 p2);
SDL_Texture* loadTexture(const char* file);
void close();
string strToLower(string str);

int SDL_main(int argc, char* args[])
{
	if (!init())
	{
		cout << "SDL init error\n";
		system("Pause");
		return 1;
	}
	cout << "=== LEAF SQUARE CALCULATOR ===\n";
	cout << "Please, enter file address:\n";
	string fileAddr;
	cin >> fileAddr;
	int lastID = fileAddr.size()-1;
	string format = string(&fileAddr[lastID-2]);
	SDL_Surface* sur = NULL;
	if (strToLower(format) == "jpg") {
		sur = loadTextureJPG(fileAddr.c_str());
	} else {
		cout << "Incorrect format: " << format << endl;
		system("Pause");
		return 2;
	}

	if (sur == NULL)
	{
		cout << "Image loading error\n";
		system("Pause");
		return 3;
	}

	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, sur);
	float dpcm = sur->w / 21.0;

	SDL_ShowWindow(win);

	bool progRunning = true;
	SDL_Event e;
	int condition = 0;
	Vector2 startPoint;
	Vector2 endPoint;

	SDL_Texture* guider = loadTexture("guider.png");

	while (progRunning)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				progRunning = false;
				return 4;
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && condition == 2)
			{
				progRunning = false;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				condition = 1;
				startPoint.x = e.button.x;
				startPoint.y = e.button.y;
			}
			if (e.type == SDL_MOUSEBUTTONUP)
			{
				condition = 2;
				endPoint.x = e.button.x;
				endPoint.y = e.button.y;
			}
		}
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderClear(ren);

		renderTexture(tex, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		if (condition == 1)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			drawRect(startPoint, { (float)x, (float)y }, {0, 0, 0, 0});
		}
		if (condition == 2)
		{
			drawRect(startPoint, endPoint, { 0, 0, 0, 0 });
			renderTexture(guider, 0, 0);
		}
		SDL_RenderPresent(ren);
	}

	scaler = sur->w / SCREEN_WIDTH;
	startPoint.x *= sur->w / SCREEN_WIDTH;
	endPoint.x *= sur->w / SCREEN_WIDTH;
	startPoint.y *= sur->h / SCREEN_HEIGHT;
	endPoint.y *= sur->h / SCREEN_HEIGHT;

	cout << "Begin virtual square calculation...\n";
	int vSquare = calcPixelCount(sur, startPoint, endPoint);
	cout << "vSquare = " << vSquare << endl;
	float pSquare = 1.0 / dpcm;
	pSquare *= pSquare;
	cout << "pSquare = " << pSquare << " cm";
	float rSquare = vSquare * pSquare;
	cout << "\n\n=== RESULT ===\nLeaf square is " << rSquare << " cm^2\n\n";
	system("Pause");
	close();
	return 0;
}

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL initialization error: " << SDL_GetError() << std::endl;
		return false;
	}
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		std::cout << "IMG initialization error: " << SDL_GetError() << std::endl;
		return false;
	}
	win = SDL_CreateWindow("Named window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_HIDDEN);
	if (win == NULL)
	{
		std::cout << "Window creation error: " << SDL_GetError() << std::endl;
		return false;
	}
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) {
		std::cout << "Renderer creation error: " << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}

SDL_Surface* loadTextureJPG(const char* file)
{
	SDL_RWops* rwop = SDL_RWFromFile(file, "r");
	if (rwop == NULL)
	{
		std::cout << "Cannot get " << file << std::endl;
		SDL_FreeRW(rwop);
		return NULL;
	}
	SDL_Surface* tempsur = IMG_LoadJPG_RW(rwop);
	SDL_FreeRW(rwop);
	return tempsur;
}

SDL_Surface* loadTexturePNG(const char* file)
{
	SDL_RWops* rwop = SDL_RWFromFile(file, "r");
	if (rwop == NULL)
	{
		std::cout << "Cannot get " << file << std::endl;
		SDL_FreeRW(rwop);
		return NULL;
	}
	SDL_Surface* tempsur = IMG_LoadPNG_RW(rwop);
	SDL_FreeRW(rwop);
	return tempsur;
}

SDL_Surface* loadTextureBMP(const char* file)
{
	SDL_RWops* rwop = SDL_RWFromFile(file, "r");
	if (rwop == NULL)
	{
		std::cout << "Cannot get " << file << std::endl;
		SDL_FreeRW(rwop);
		return NULL;
	}
	SDL_Surface* tempsur = IMG_LoadBMP_RW(rwop);
	SDL_FreeRW(rwop);
	return tempsur;
}

void renderTexture(SDL_Texture* tex, int x, int y, int w, int h)
{
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

void renderTexture(SDL_Texture* tex, int x, int y)
{
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTexture(tex, x, y, w, h);
}

Uint8* copySurfacePixels(
	SDL_Surface* surface,  // surface to take pixels from
	Uint32 pixelFormat,    // usually SDL_GetWindowPixelFormat(window)
	SDL_Renderer* renderer,// main SDL2 renderer
	int* width,            // stores result width
	int* height,           // stores result height
	int* pitch)            // stores result pitch
{
	Uint8* pixels = 0;
	SDL_Surface* tmpSurface = 0;
	SDL_Texture* texture = 0;
	int sizeInBytes = 0;
	void* tmpPixels = 0;
	int tmpPitch = 0;

	tmpSurface = SDL_ConvertSurfaceFormat(surface, pixelFormat, 0);
	if (tmpSurface) {
		texture = SDL_CreateTexture(renderer, pixelFormat,
			SDL_TEXTUREACCESS_STREAMING,
			tmpSurface->w, tmpSurface->h);
	}

	if (texture) {
		if (width) {
			*width = tmpSurface->w;
		}
		if (height) {
			*height = tmpSurface->h;
		}
		if (pitch) {
			*pitch = tmpSurface->pitch;
		}
		sizeInBytes = tmpSurface->pitch * tmpSurface->h;
		pixels = (Uint8*)malloc(sizeInBytes);
		SDL_LockTexture(texture, 0, &tmpPixels, &tmpPitch);
		memcpy(pixels, tmpSurface->pixels, sizeInBytes);
		SDL_UnlockTexture(texture);
	}

	// Cleanup
	if (texture) {
		SDL_DestroyTexture(texture);
	}
	if (tmpSurface) {
		SDL_FreeSurface(tmpSurface);
	}

	return pixels;
}

void drawLine(Vector2 p1, Vector2 p2, Color col)
{
	SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.a);
	SDL_RenderDrawLine(ren, p1.x, p1.y, p2.x, p2.y);
}

void drawRect(Vector2 p1, Vector2 p2, Color col)
{
	drawLine(p1, { p2.x, p1.y }, col);
	drawLine(p1, { p1.x, p2.y }, col);
	drawLine(p2, { p2.x, p1.y }, col);
	drawLine(p2, { p1.x, p2.y }, col);
}

int calcPixelCount(SDL_Surface *sur, Vector2 p1, Vector2 p2)
{
	int counter = 0;
	Uint32 pf = SDL_GetWindowPixelFormat(win);
	int w = 0, h = 0, p = 0;
	Uint8* pixels = copySurfacePixels(sur, pf, ren, &w, &h, &p);

	int startX = p1.x; int xMax = p2.x;
	if (p2.x < startX) { startX = p2.x; xMax = p1.x; }
	int startY = p1.y; int yMax = p2.y;
	if (p2.y < startY) { startY = p2.y; yMax = p1.y; }

	for (int x = startX; x < xMax; x++)
	{
		for (int y = startY; y < yMax; y++)
		{
			int b = pixels[4 * (y * w + x) + 0]; // Blue
			int g = pixels[4 * (y * w + x) + 1]; // Green
			int r = pixels[4 * (y * w + x) + 2]; // Red
			int checkValue = r + g + b;
			if (checkValue < MAXCHECKVALUE)
			{
				counter++;
				SDL_RenderDrawPoint(ren, x / scaler, y / scaler);
			}
		}
	}
	SDL_RenderPresent(ren);
	SDL_Delay(3000);
	SDL_HideWindow(win);
	free(pixels);
	return counter;
}

SDL_Texture* loadTexture(const char* file)
{
	SDL_Texture* texture = IMG_LoadTexture(ren, file);
	if (!texture) {
		std::cout << "Texture loading problem: " << file << "  |  " << IMG_GetError() << std::endl;
	}
	return texture;
}

void close()
{
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	IMG_Quit();
	SDL_Quit();
}

string strToLower(string str)
{
	string nstr = "";
	for (int i = 0; i < str.size(); i++)
	{
		nstr += tolower(str[i]);
	}
	return nstr;
}