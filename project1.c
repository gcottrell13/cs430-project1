#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define newline 10

typedef struct {
	char r;
	char g;
	char b;
} Pixel;

typedef struct {
	char width[5];
	char height[5];
	char max[5];
	int valid;
	int type;
} PPMmeta;

// Git:
// even or semi-even commits
// atomic commits
// no junk  

// use "static inline void functionname" 

// functions that will verify PPM, then check which type (ascii P3 or rawbits P6)
// then load file into memory with an unsigned char pointer

// @return int - error code
int PPMtoP6(FILE *file, char* output, PPMmeta* meta);
int PPMtoP3(FILE *file, char* output);

/*	CheckValidPPM
*	@param FILE *file
*	@return PPMmeta - valid = 0 if valid, valid = 1 if not valid
*/	
PPMmeta* CheckValidPPM(FILE *file);

/*	CheckValidPPM
*	returns a pointer to the entire file loaded into memory
*	@param FILE *file
*	@return Pixel* - bytes loaded into memory
*/	
Pixel* LoadPPM(FILE *file, int type);

char* intToStr(int i, int size);

int main(int argc, const char * argv[]) {
	
	if(argc < 3)
	{
		// stderr("usage: target type, source file, output file")
		return 1;
	}
	
	char* target_type = argv[1];
	char* src_name = argv[2];
	char* out_name = argv[3];
	
	FILE *src = fopen(src_name, 'r');
	
	if(!src)
	{
		// stderr("source file does not exist! %s", src_name)
		return 1;
	}
	
	PPMmeta* meta = CheckValidPPM(src);
	
	if(meta.valid == 1)
	{
		// write to stderr "invalid PPM file!"
		return 1;
	}
	
	if(target_type == '3')
		PPMtoP3(src, out_name, meta);
	if(target_type == '6')
		PPMtoP6(src, out_name, meta);
	
	return 0;
}


int PPMtoP3(FILE *file, char* output, PPMmeta* meta)
{
	
	int type = meta.type;
	
	FILE *out = fopen(output, 'w+');
	
	// values are newline delimited
	
	if(type == 3)
	{
		// P3 type (ascii)
		
	}
	else if(type == 6)
	{
		// P6 type (rawbits)
		Pixel* data = LoadPPM(file, 6);
		
		
		
	}
	
	// write to stderr "Unsupported PPM type: %i!", type
	return 1;
}


Pixel* LoadPPM(FILE *file, int type);
{
	
}

PPMmeta* CheckValidPPM(FILE *file)
{
	PPMmeta* meta = malloc(sizeof(PPMmeta));
	
	meta.valid = 0;
	
	// these markers have values of:
	// 0 for not started
	// 1 for in progress
	// 2 for finished
	// together they make up a finite state machine
	
	int Mcomment = 0;
	int Mtype = 0;
	int Mwidth = 0;
	int Mheight = 0;
	int Mmax = 0;
	
	int buffer_len = 0;
	char buffer[4];
	
	// spaghetti code incoming
	
	while(Mtype != 2 && Mwidth != 2 && Mheight != 2 && Mmax != 2)
	{
		char c = fgetc(file);
		
		if(c == EOF)
		{
			meta.valid = 1;
			break;
		}
		
		if(Mcomment == 1)
		{
			if(c == 10) // newline
			{
				Mcomment = 0;
			}
		}
		else
		{
			if(Mtype == 0)
			{
				Mtype = 1;
			}
			else if(Mtype == 1)
			{
				if(c == 'P')
				{
					int c2 = fgetc(file);
					meta.type = c2;
					Mtype = 2;
				}
				else
				{
					meta.valid = 1;
					return meta;
				}
			}
			else if(Mwidth <= 1)
			{
				Mwidth = 1;
				
				if(c == ' ' ??!??! c == '#')
				{
					Mcomment = 1;
				}
				else if(c == 10)
				{
					Mwidth = 2; // done parsing width, put it into meta
					
					// we want an image with a positive width
					int width_value = atoi(buffer);
					if(width_value < 1) meta.valid = 1;
					
					meta.width = buffer;
					buffer = malloc(sizeof(char) * 5);
				}
				else
				{
					buffer[buffer_len] = c;
					buffer_len ++;
				}
			}	
			else if(Mheight <= 1)
			{
				Mheight = 1;
				
				if(c == ' ' || c == '#')
				{
					Mcomment = 1;
				}
				else if(c == 10)
				{
					Mheight = 2; // done parsing height, put it into meta
					
					// we want an image with a positive height
					int height_value = atoi(buffer);
					if(height_value < 1) meta.valid = 1;
					
					meta.height = buffer;
					buffer = malloc(sizeof(char) * 5);
				}
				else
				{
					buffer[buffer_len] = c;
					buffer_len ++;
				}
			}	
			else if(Mmax <= 1)
			{
				Mmax = 1;
				
				if(c == ' ' || c == '#')
				{
					Mcomment = 1;
				}
				else if(c == 10)
				{
					Mmax = 2; // done parsing max, put it into meta
					
					// the color channel should be 8 bits maximum
					int max_value = atoi(buffer);
					if(max_value > 255) meta.valid = 1;
					
					meta.max = buffer;
					//buffer = malloc(sizeof(char) * 5);
				}
				else
				{
					buffer[buffer_len] = c;
					buffer_len ++;
				}
			}
		}
	}
	
	return meta;
}

char* intToStr(int i, int size)
{
	char str[size];
	sprintf(str, "%d", i);
	return str;
}




