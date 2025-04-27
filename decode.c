#include <stdio.h>
#include<stdlib.h>
#include "types.h"
#include "decode.h"
#include<string.h>
#include<unistd.h>
#define MAGIC_STRING "#*"

/* Function Definitions */
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo )
{
	// Check if source file is .bmp
    if (strstr(argv[2], ".bmp") == NULL || strcmp(strstr(argv[2], "."), ".bmp") != 0) {
        printf("Usage: ./a.out -e <.bmp file> <.txt file> [optional <.bmp file>]\n");
        printf("Please pass a valid .bmp source file.\n");
        return e_failure;
    }

    // Store source file name
    decInfo->src_image_fname = argv[2];

    // Check secret file extension

	if(argv[3]!=NULL) 
	{
		strcpy( decInfo->stego_image_fname,argv[3]);
	}
	else
	{
		strcpy(decInfo->stego_image_fname,"output.txt");
		return e_success;
	}
	return e_success;
} 
//Function definition for Decoding
Status do_decoding(DecodeInfo *decInfo)  
{
	//Function call for Opening the files
	if(dopen_files(decInfo)==e_success)
	{
		//Printing the prompt message for Opening files success
		printf("INFO:files are opened successfully\n");
	}
	else
	{
		//Printing the prompt message for Opening files Failure
		printf("ERROR:files opening failed\n");
	}
	if(skip_bmp_header(decInfo->fptr_src_image)==e_success)
	{
		printf("INFO:bmp header file is  skipped\n");
	}
	else
	{
		printf("ERROR: bmp header file are not skipped succesfully\n");
	}

	//Function call for decode magic string
	if(decode_magic_string(MAGIC_STRING,decInfo) == e_success)
	{
		//Printing the prompt message decode magic string success
		printf("INFO:magic string decode sucessfully\n");
	}
	else
	{
		//Printing the prompt message for decode magic string failure
		printf("ERROR:magic string decoding is failed\n");
		exit(1);
	}
	//Function call for decode_secret_file_extn_size
	if(decode_secret_file_extn_size(decInfo)==e_success) 
	{
		//Printing the prompt message for decode_secret_file_extn_size success
		printf("INFO:secret file extension size is decoded successfully\n");
	}
	else
	{	
		//Printing the prompt message for decode_secret_file_extn_size failure
		printf("ERROR:secret file extension size is decoding is failed\n");
	}
	//Function call for decode_secret_file_extn
	if(decode_secret_file_extn(decInfo)==e_success)
	{
		//Printing the prompt message decode_secret_file_extn success
		printf("INFO:secret file extension is sucessfully\n");
	}
	else
	{
		//Printing the prompt message decode_secret_file_extn failure
		printf("ERROR:secret file extension is failed\n");
	}
	//Function call for decode_secret_file_data

	if(decode_secret_file_size(decInfo)==e_success) 
	{
		//Printing the prompt decode_secret_file_size files success
		printf("INFO:secret file size is decoded successfully\n");
	}
	else
	{
		//Printing the prompt decode_secret_file_size failure
		printf("ERROR:secret file size is decoding is failed\n");
	}
	//Function call for decode_secret_file_data
	if(decode_secret_file_data(decInfo)==e_success) 
	{
		//Printing the prompt message for decode_secret_file_data success
		printf("INFO:secret file data is decoded successfully\n");
	}
	else
	{
		//Printing the prompt message for decode_secret_file_data failure
		printf("ERROR:secret file data is decoded successfully\n");
	}
	return e_success;
}
//Function definition for open files 
Status dopen_files(DecodeInfo *decInfo)
{
	//Open the source image file in read mode 

	decInfo->fptr_src_image=fopen(decInfo->src_image_fname,"r");
	//Check the condition 

	if(decInfo->fptr_src_image==NULL)    
	{
		perror("fopen");
		//Print the error message
		fprintf(stderr,"ERROR:Unable to open files %s\n",decInfo->src_image_fname); 
		return e_failure;
	}
	return e_success;
}
Status skip_bmp_header(FILE *fptr_src_image)
{
	if(fseek(fptr_src_image,54,SEEK_SET)!=0)
	{
		return e_failure;
	}
	else
	{
		return e_success;
	}
}

//Function definition for decode magiic string 
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{

	char my_magic_string[3] = {0};
//	fseek ( decInfo -> fptr_src_image, 54, SEEK_SET);
	//Function call for data to image 
	decode_data_to_image(my_magic_string,strlen(magic_string),decInfo->fptr_src_image,decInfo); 
	char str[20];

	printf("Enter the magic string\n");
	scanf("%s",str);
	if(strcmp(my_magic_string,str)==0)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}

}
//Function definition for data to image 
Status decode_data_to_image(char *my_magic_string,int size,FILE *fptr_src_image,DecodeInfo *decInfo) 
{
	int i;
	char buffer[8];
	for(i=0;i<size;i++)
	{
		//Read the source image
		fread(buffer,1,8,fptr_src_image); 
		my_magic_string[i]=decode_lsb_to_byte(buffer);
	}
	//Assign the null to my magic string 
	my_magic_string[i]='\0';
	//Check the condition for magic string 
	return e_success;
	
}
//Function definition for lsb to byte 
Status decode_lsb_to_byte( char *buffer)
{
	char ch=0;
	for(int i=0;i<8;i++)
	{
		//Bitwise operation 
		ch=((buffer[i]&1)<<(7-i))|ch; 
	}
	return ch;
}
//Function definition for secret file extn size
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	char buffer[32];
	//Read source image 
	fread(buffer,1,32,decInfo->fptr_src_image); 
	//Funtion call size to lsb
	decode_size_to_lsb(buffer,&(decInfo->size_output_file));


	return e_success;
}


void append_extension(char *stego_image_fname,char * arr)
{
	char *ext = strrchr(stego_image_fname, '.');

    if (ext != NULL) {
        // If the file has a '.', remove the old extension and add the new one
        // Null-terminate the string before the '.' to remove the old extension
        *ext = '\0';
    }

    // Append the new extension (e.g., ".arr")
    strcat(stego_image_fname, arr);
}


//Function definition for secret file extn 
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	//Feclare array
	char arr[decInfo->size_output_file];  
	//Function call for data to image 
	decode_data_to_image(arr,sizeof(arr),decInfo->fptr_src_image,decInfo);

//	printf("\n\n\nExtension : %s\n\n\n",arr);

	append_extension(decInfo->stego_image_fname,arr);
	//Open the stego image
	decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"w");
	//Check the condition 
	if(decInfo->fptr_stego_image==NULL)  
	{
		perror("fopen");
		//Print error message 
		fprintf(stderr,"ERROR:Unable to open file %s\n",decInfo->stego_image_fname);

		return e_failure;
	}
	return e_success;
}
//Function definition for size to lsb
Status decode_size_to_lsb(char *buffer,long int *size)
{
	*size=0;
	for(int i=0;i<32;i++)
	{
		//Bitwise operation 
		*size=(((buffer[i]&1)<<(31-i))|*size);
	}
	return e_success;

}
//Function definition for secret file size 
Status decode_secret_file_size(DecodeInfo *decInfo) 
{
	char buffer[32];
	fread(buffer,1,32,decInfo->fptr_src_image);
	//Function call for decode size to lsb
	decode_size_to_lsb(buffer,&(decInfo->size_output_file)); 
	return e_success;
}
//Function definition for secret file data 
Status decode_secret_file_data(DecodeInfo *decInfo)   
{
	int size=decInfo->size_output_file;
	char arr[size];
	//Function call data to image 
	decode_data_to_image(arr,decInfo->size_output_file,decInfo->fptr_src_image,decInfo);  
	//Write output file 
	fwrite(arr,1,decInfo->size_output_file,decInfo->fptr_stego_image); 
	return e_success;
}


