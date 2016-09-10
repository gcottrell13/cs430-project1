#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define newline 10

typedef struct {
	char r;
	char g;
	char b;
} Pixel;

// Git:
// even or semi-even commits
// atomic commits
// no junk  

// use "static inline void functionname" 

// functions that will verify PPM, then check which type (ascii P3 or rawbits P6)
// then load file into memory with an unsigned char pointer

// @return int - error code
int PPMtoP6(FILE *file, char* output);
int PPMtoP3(FILE *file, char* output);

/*	CheckValidPPM
*	@param FILE *file
*	@return int - 0 if valid, 1 if not valid
*/	
int CheckValidPPM(FILE *file);

/*	CheckValidPPM
*	@param FILE *file
*	@return int - number corresponding to PPM type (3 or 6 for our purposes)
*/	
int GetPPMType(FILE *file);

/*	CheckValidPPM
*	returns a pointer to the entire file loaded into memory
*	@param FILE *file
*	@return Pixel* - bytes loaded into memory
*/	
Pixel* LoadPPM(FILE *file);

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
	
	int valid = CheckValidPPM(src);
	
	if(valid == 1)
	{
		// write to stderr "invalid PPM file!"
		return 1;
	}
	
	if(target_type == '3')
		PPMtoP3(src, out_name);
	if(target_type == '6')
		PPMtoP6(src, out_name);
	
	return 0;
}


int PPMtoP3(FILE *file, char* output)
{
	
	int type = GetPPMType(file);
	
	FILE *out = fopen(output, 'w+');
	
	// values are newline delimited
	
	if(type == 3)
	{
		// P3 type (ascii)
		
	}
	else if(type == 6)
	{
		// P6 type (rawbits)
		unsigned char* data = LoadPPM(file);
		
		
		
	}
	
	// write to stderr "Unsupported PPM type: %i!", type
	return 1;
}


unsigned char* LoadPPM(FILE *file);
{
	
}

char* intToStr(int i, int size)
{
	char str[size];
	sprintf(str, "%d", i);
	return str;
}




