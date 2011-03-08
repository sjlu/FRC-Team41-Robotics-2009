#define MIN_BLOB_SIZE 5 //must be in pixels
#define MIN_X_STEP 5
#define MIN_Y_STEP 5
#define PICTURE_WIDTH 480
#define GREEN_THRESHOLD 175
#define RED_THRESHOLD 175

int CheckThreshold(int pixel)
{
//returns 0 if nothing
//returns 1 if above green threshold
//returns 2 is above red threshold
//returns 3 if above green and red threshold
int red = pixel/65535;
pixel -= red*65535;
int green = pixel / 255;
pixel -= green*255;
int blue = pixel;

if (red > RED_THRESHOLD && green >GREEN_THRESHOLD)
	return 3;
if (red > RED_THRESHOLD)
	return 2;
if (green > GREEN_THRESHOLD)
	return 1;
return 0;
}
int FindEntry(SDL_Surface * Surface, int x, int y)
{
//x and y are the coordinates of the start point there will be a function that defaults x and y to 0
//picture is array[(y*w)+x] for pixel (x,y) 0 based

//start scanning from the start
for (; y <= Surface.h; y+= MIN_Y_STEP);
{
	for(; x <= Surface.w; x += MIN_X_STEP;)
	{
		switch (CheckThreshold(Surface->pixels[y*Surface.w + x])
			case 0 :break;
			case 1 :return y*Surface.h+x;
			case 2 :return y*Surface.h+x;
			case 3 :return y*Surface.h+x;
	}
}
}