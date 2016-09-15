#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Pixel;

typedef struct {
	int width;
	int height;
	int max;
	int valid;
	int type;
} PPMmeta;

// functions that will verify PPM, then check which type (ascii P3 or rawbits P6)
// then load file into memory with an unsigned char pointer

// @return int - error code
int PPMtoT(FILE *file, char* output, int out_type, PPMmeta meta);

/*	CheckValidPPM
*	@param FILE *file
*	@return PPMmeta - valid = 0 if valid, valid = 1 if not valid
*/	
PPMmeta CheckValidPPM(FILE *file);

/*	CheckValidPPM
*	returns a pointer to the entire file loaded into memory
*	@param FILE *file
*	@param int type
*	@param int size
*	@return Pixel* - bytes loaded into memory
*/	
Pixel* LoadPPM(FILE *file, int type, int size);

char* intToStr(int i, int size);

int main(int argc, const char * argv[]) {
	
	if(argc < 3)
	{
		// stderr("usage: target type, source file, output file")
		fprintf(stderr, "usage: target type, source file, output file\n");
		return 1;
	}
	
	const char* target_type = argv[1];
	char* src_name = argv[2];
	char* out_name = argv[3];
	
	FILE *src = fopen(src_name, "r");
	
	if(!src)
	{
		// stderr("source file does not exist! %s", src_name)
		fprintf(stderr, "Source file does not exist!\n");
		return 1;
	}
	
	PPMmeta meta = CheckValidPPM(src);
	
	if(meta.valid == 1)
	{
		// write to stderr "invalid PPM file!"
		fprintf(stderr, "Invalid PPM file!\n");
		return 1;
	}
	
	if(*target_type == '3')
		PPMtoT(src, out_name, 3, meta);
	if(*target_type == '6')
		PPMtoT(src, out_name, 6, meta);
	
	fclose(src);
	
	return 0;
}


int PPMtoT(FILE *file, char* output, int out_type, PPMmeta meta)
{
	FILE *out = fopen(output, "w+");
	
	// values are newline delimited
	
	if(out_type == 3)
	{
		// input P3 type (ascii)
		Pixel* data = LoadPPM(file, meta.type, meta.width * meta.height);
		
		fprintf(out, "P%d\n", out_type);
		fprintf(out, "%d %d\n", meta.width, meta.height);
		fprintf(out, "%d\n", meta.max);
	
		int c;
		char newline = 10;
		for(c = 0; c < meta.width * meta.height; c++)
		{
			Pixel p = data[c];
			char* r = intToStr(p.r, 4);
			char* g = intToStr(p.g, 4);
			char* b = intToStr(p.b, 4);
			fprintf(out, "%s\n", r);
			fprintf(out, "%s\n", g);
			fprintf(out, "%s\n", b);
		}
	}
	else if(out_type == 6)
	{
		// input P6 type (rawbits)
		Pixel* data = LoadPPM(file, meta.type, meta.width * meta.height);
		
		
		fprintf(out, "P%d\n", out_type);
		fprintf(out, "%d %d\n", meta.width, meta.height);
		fprintf(out, "%d\n", meta.max);
		
		int c;
		for(c = 0; c < meta.width * meta.height; c++)
		{
			Pixel p = data[c];
			//fwrite(p.r, sizeof(char), sizeof(p.r), out);
			//fwrite(p.g, sizeof(char), sizeof(p.g), out);
			//fwrite(p.b, sizeof(char), sizeof(p.b), out);
			fprintf(out, "%c", p.r);
			fprintf(out, "%c", p.g);
			fprintf(out, "%c", p.b);
		}
	}
	else
	{
		fclose(out);
		// write to stderr "Unsupported PPM type: %i!", type
		fprintf(stderr, "Unsupported PPM type: %d!\n", out_type);
		return 1;
	}
	fclose(out);
	return 0;
}


Pixel* LoadPPM(FILE *file, int type, int size)
{
	Pixel* buffer = malloc(sizeof(Pixel) * size + 1);
	
	if(type == 3)
	{
		char* cbuffer;
		int cbuffer_size;
		char character;
		int c;
		
		for(c = 0; c < size; c++)
		{
			int r = -1;
			int g = -1;
			int b = -1;
			
			while(r < 0 || g < 0 || b < 0) // read values for each of the rgb components for a single pixel
			{
				cbuffer = malloc(sizeof(char) * 4);
				cbuffer_size = 0;
				character = fgetc(file);
				while(character != 10 && cbuffer_size < 3)
				{
					cbuffer[cbuffer_size] = character;
					cbuffer_size ++;
					character = fgetc(file);
				}
				
				if(cbuffer_size > 0)
				{
					int value = atoi(cbuffer);
					if(r == -1) r = value;
					else if(g == -1) g = value;
					else if(b == -1) b = value;
				}
			}
			
			Pixel p;
			p.r = r;
			p.g = g;
			p.b = b;
			
			buffer[c] = p;
		}
	}
	else if(type == 6)
	{
		//printf("type: 6, size: %d\n", size);
		int c;
		for(c = 0; c < size; c++)
		{
			Pixel* pixel = malloc(sizeof(Pixel));
			fread(pixel, sizeof(Pixel), 1, file);
			buffer[c] = *pixel;
		}
		
	}
	
	return buffer;
}

PPMmeta CheckValidPPM(FILE *file)
{
	PPMmeta meta;
	
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
	char* buffer = malloc(sizeof(char) * 4);
	
	// spaghetti code incoming
	
	while(1)
	{
		char c = fgetc(file);
		//printf("READ: %c\n", c);
		if(c == EOF)
		{
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
			//printf("no comment\n");
			if(Mtype <= 1)
			{
				Mtype = 1;
				if(c == 'P')
				{
					int c2 = fgetc(file);
					char _type[2];
					_type[0] = c2;
					meta.type = atoi(_type);
					Mtype = 2;
					c2 = fgetc(file);
				}
				else
				{
					fprintf(stderr, "Absence of P in file header! %c\n", c);
					meta.valid = 1;
					return meta;
				}
			}
			else if(Mwidth <= 1)
			{
				Mwidth = 1;
				//printf("mwidth\n");
				
				if(c == '#')
				{
					Mcomment = 1;
				}
				else if(((c == 10 || c == ' ') && buffer_len > 0) || buffer_len >= 5)
				{
					Mwidth = 2; // done parsing width, put it into meta
					// we want an image with a positive width
					int width_value = atoi(buffer);
					if(width_value < 1) 
					{
						fprintf(stderr, "Image must have positive width: %d %s!\n", width_value, buffer);
						meta.valid = 1;
					}
					
					meta.width = width_value;
					buffer = malloc(sizeof(char) * 5);
					buffer_len = 0;
				}
				else
				{
					//printf("reading char to width: %c\n", c);
					buffer[buffer_len] = c;
					buffer_len ++;
				}
			}	
			else if(Mheight <= 1)
			{
				Mheight = 1;
				//printf("mheight\n");
				
				if(c == '#')
				{
					Mcomment = 1;
				}
				else if(((c == 10 || c == ' ') && buffer_len > 0) || buffer_len >= 5)
				{
					Mheight = 2; // done parsing height, put it into meta
					
					// we want an image with a positive height
					int height_value = atoi(buffer);
					if(height_value < 1) 
					{
						fprintf(stderr, "Image must have positive height!\n");
						meta.valid = 1;
					}
					
					meta.height = height_value;
					buffer = malloc(sizeof(char) * 5);
					buffer_len = 0;
				}
				else
				{
					//printf("reading char to height: %c\n", c);
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
				else if(c == 10 || buffer_len >= 5)
				{
					Mmax = 2; // done parsing max, put it into meta
					
					// the color channel should be 8 bits maximum
					int max_value = atoi(buffer);
					if(max_value > 255) 
					{
						fprintf(stderr, "Image must have 8 bit maximum color channels!\n");
						meta.valid = 1;
					}
					
					meta.max = max_value;
					break; // we're done here
				}
				else
				{
					//printf("reading char to max: %c\n", c);
					buffer[buffer_len] = c;
					buffer_len ++;
				}
			}
		}
		//printf("State of reading: type %d | width %d | height %d | max %d\n", Mtype, Mwidth, Mheight, Mmax);
	}
	
	return meta;
}

char* intToStr(int i, int size)
{
	char* str = malloc(sizeof(char) * size);
	sprintf(str, "%d", i);
	return str;
}




