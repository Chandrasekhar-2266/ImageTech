#include <stdio.h>
#include <string.h>
#include "common.h"
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	//printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	//printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files( EncodeInfo *encInfo )
{
	// Src Image file opening
	encInfo -> fptr_src_image = fopen(encInfo -> src_image_fname, "r");
	// Do Error handling
	if (encInfo -> fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> src_image_fname);

		return e_failure;
	}

	// Secret file
	encInfo -> fptr_secret = fopen(encInfo -> secret_fname, "r");
	// Do Error handling
	if (encInfo -> fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	// Stego Image file
	encInfo -> fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	// Do Error handling
	if (encInfo -> fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}
	// No failure return e_success
	return e_success;
}
//Function definition for check and validate for the CLA arguments

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Check if source file is .bmp
    if (strstr(argv[2], ".bmp") == NULL || strcmp(strstr(argv[2], "."), ".bmp") != 0) {
        printf("Usage: ./a.out -e <.bmp file> <.txt file> [optional <.bmp file>]\n");
        printf("Please pass a valid .bmp source file.\n");
        return e_failure;
    }
    
    // Store source file name
    encInfo->src_image_fname = argv[2];

    // Check secret file extension
    if (strstr(argv[3], ".txt") == NULL || strcmp(strstr(argv[3], "."), ".txt") != 0) {
        printf("Usage: ./a.out -e <.bmp file> <.txt file> [optional <.bmp file>]\n");
        printf("Please pass a valid .txt secret file.\n");
        return e_failure;
    }

    // Store secret file name
    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, ".txt"));

    // Check optional stego image argument
    if (argv[4] != NULL) {
        if (strstr(argv[4], ".bmp") == NULL || strcmp(strstr(argv[4], "."), ".bmp") != 0) {
            printf("Usage: ./a.out -e <.bmp file> <.txt file> [optional <.bmp file>]\n");
            printf("Please pass a valid .bmp stego image file.\n");
            return e_failure;
        }
        // Store stego image filename
        encInfo->stego_image_fname = argv[4];
    } else {
        // Default stego image filename
        encInfo->stego_image_fname = "output.bmp";
    }

    return e_success;
}
//Check capacity function Definition
Status check_capacity( EncodeInfo*encInfo)
{
	//function call for get image for bmp
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
	//function call get file size
	encInfo -> size_secret_file = get_file_size( encInfo -> fptr_secret);
	//check the condition
	if(encInfo->image_capacity > (2+4+4+encInfo->size_secret_file+encInfo->size_secret_file)*8)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}
//function definition for get file size
uint get_file_size(FILE *fptr) 
{
	fseek(fptr,0,SEEK_END);
	return ftell(fptr);
}
//Function definition for Copying Header file 
Status copy_bmp_header( FILE *fptr_src_image, FILE *fptr_dest_image )
{
	//Create array of 54 elements because of header is same for every image
	char bmp_header[54];
	rewind(fptr_src_image);
	//Reading 54 bytes from source
	fread(bmp_header, 54, 1, fptr_src_image );
	//Writing 54 bytes to destination file
	fwrite(bmp_header, 54, 1, fptr_dest_image);
	return e_success;
}
//Function definition for encode data to image
Status encode_data_to_image(char *magic_string, int size, FILE *fptr_src_image, FILE *fptr_dest_image)
{
	//Character array
	char buffer[8];
	for(int i=0; i < size; i++)
	{
		fread( buffer, 8, 1, fptr_src_image);
		//Function call for encode byte to lsb
		encode_byte_to_lsb (magic_string[i], buffer);
		fwrite(buffer, 8, 1, fptr_dest_image);
	}
	return e_success;
}
//Function definition for encode byte to lsb
Status encode_byte_to_lsb( char data, char*image_buffer)
{
	for ( int i = 7; i>=0; i-- )
	{
		int mask1 = data & (1 << i);
		int mask2 =	(unsigned)mask1 >> i;
		int mask3 =  mask2 | ( image_buffer[7-i] &(~1) );
		image_buffer[7-i] = mask3;
	}
	return e_success;

}
//Function definition for encoding magic dtring
Status encode_magic_string( char *magic_string , EncodeInfo * encInfo)
{
	//Function Call of encode data to image function
	encode_data_to_image( magic_string,strlen(magic_string), encInfo -> fptr_src_image,encInfo -> fptr_stego_image );
	return e_success;
}
//Function definition of encode secret file extenction
Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo)  
{
	//Function Call for encode data to image
	encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);
	return e_success;
}
//Function definition for encode secret file extension
Status encode_secret_file_extn_size(int size,EncodeInfo *encInfo) 
{
	char buffer[32];
	fread(buffer,32,1,encInfo->fptr_src_image);
	//Function Call for Encode secret file extention
	encode_size_to_lsb(buffer,size);
	fwrite(buffer,32,1,encInfo->fptr_stego_image);
	return e_success;
}
//Function definition for encode size to lsb
Status encode_size_to_lsb(char *image_buffer,int data)  
{
	for(int i=0;i<32;i++)
	{
		image_buffer[i]=(((data&(1<<(31-i)))>>(31-i))|(image_buffer[i]&~1)); 
	}
	return e_success;
}
//Function Definition for Encode secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	char buffer[32];
	fread(buffer,32,1,encInfo->fptr_src_image);
	//Function call encode size to lsb for encode secret file size
	encode_size_to_lsb(buffer,file_size);
	fwrite(buffer,32,1,encInfo->fptr_stego_image);
	return e_success;
}
//Function definition of Encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)  
{
	//Charater array
	char arr[encInfo -> size_secret_file];
	rewind(encInfo -> fptr_secret);
	fread(arr,1,encInfo -> size_secret_file,encInfo->fptr_secret);
	//Function call of encode dato to image for encode secret file data
	encode_data_to_image(arr,strlen(arr),encInfo->fptr_src_image,encInfo->fptr_stego_image);
	return e_success;
}
//Function definition of copying remaining data
Status copy_remaining_img_data(FILE *fptr_src_image,FILE *fptr_stego_image) 
{
	char str;
	while( fread(&str,1,1,fptr_src_image) > 0) 
	{
		fwrite(&str,1,1,fptr_stego_image);
	}
	return e_success;   
}
//Function definition for encoding the data
Status do_encoding(EncodeInfo *encInfo)
{
	//Function call for opening files 
	if ( open_files(encInfo) == e_success)
	{
		//Printing the prompt for success of opening file
		printf("INFO:Open files success\n");
	}
	else
	{
		//Printing Error For failing to opening files
		printf("ERROR:Open files failure\n");
		return e_failure;
	}
	//Function call for checking the capacity
	if ( check_capacity (encInfo) == e_success)
	{
		//Printing prompt for check_capacity success
		printf("INFO:Check Capacity success\n");
	}
	else
	{
		//Printing Error for check capacity failure
		printf("ERROR:Check Capacity failure\n");
	}
	//Function Call for copying header file
	if ( copy_bmp_header( encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
	{
		//Printing prompt for copy header file success
		printf("INFO:Copying header is success\n");
	}
	else
	{
		//Printing Error for copying header failure
		printf("ERROR:Copying header is failure\n");
	}

	//Function Call for encoding magic string file
	if ( encode_magic_string( MAGIC_STRING , encInfo ) == e_success)
	{
		//Printing prompt for encode magic string success
		printf("INFO:Encode magic string is success\n");		
	}
	else
	{
		//Printing Error for encoding magic string failure
		printf("ERROR:Encode magic string is fail\n");
	}
	//Function Call for encode_secret_file_extn_size file
	if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_success) 
	{
		//Printing prompt for encode_secret_file_extn_size success
		printf("INFO:Secret file extension is encoded successfully\n");
	}
	else
	{
		//Printing Error for encode_secret_file_extn_size failure
		printf("ERROR:Secret file extension is failed to encode \n");
	}
	//Function Call for encode_secret_file_size file
	if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
	{
		//Printing prompt for encode_secret_file_size success
		printf("INFO:Secret file extension size is encoded successfully\n");
	}
	else
	{
		//Printing Error for encode_secret_file_size failure
		printf("ERROR:Secret file extension size is failed to encode \n");
	}
	//Function Call for encode_secret_file_size file
	if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
	{
		//Printing prompt for encode_secret_file_size success

		printf("INFO:Secret file size is encoded successfuly\n");
	}
	else 
	{
		//Printing Error for encode_secret_file_size failure
		printf("ERROR:Secret file size is failed\n");
	}
	//Function Call for encode_secret_file_data file
	if(encode_secret_file_data(encInfo)==e_success)
	{
		//Printing prompt for encode_secret_file_data success
		printf("INFO:Secret file data is encoded successfully\n");
	}
	else
	{
		//Printing Error for encode_secret_file_data failure
		printf("ERROR: secret file data is encoding is failure\n");
	}
	//Function Call for copy_remaining_img_data
	if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	{
		//Printing prompt for copy_remaining_img_data success
		printf("INFO:Copy the remaining data is successfully completed\n");
	}
	else
	{
		printf("ERROR: The remaining data is not copied\n");
	}
	return e_success;
}
