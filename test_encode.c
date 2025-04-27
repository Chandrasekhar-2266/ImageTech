#include <stdio.h>
#include <string.h>
#include "common.h"
#include "encode.h"
#include "types.h"
#include "decode.h"
//Main funcyion for collecting the CLA arguments
int main(int argc, char *argv[])
{
	//typedef Structures
	//For Encoding 
	EncodeInfo encInfo;
	//For  Decoding
	DecodeInfo decInfo;
	//Function call for checking the operation
	if(argc==1)
	{
		printf("./a.out:Encodeing:./a.out -e < .bmpfile> <.txt file> [optional]\n");
		printf("/a.out:Decodeing:./a.out -d < .bmpfile> [optional]\n");
		printf("please enter valid arguments\n");
		return 0;
	}
	
	if (check_operation_type(argv) == e_encode)
	{   
		/*CLA count validation*/
		if ( argc >= 4 && argc <= 5)
		{
			/*Checking CLA is gets passed in proper extensions*/
			if ( read_and_validate_encode_args(argv, &encInfo) == e_success)
			{printf("Selected encoding\n");

				printf("INFO : Read and validate success\n");
			}
			else
			{
				printf("ERROR: Read and validate failure\n");
				return e_failure;
			}
			//Function call for Encoding
			if ( do_encoding(&encInfo) == e_success)
			{
				printf("Encoding success\n");
			}
		
		}
		//Printing Error for invalid arguments
		else
		{
            printf("./a.out:Encodeing:./a.out -e < .bmpfile> <.txt file> [optional]\n");
			printf("please pass valid Arguments\n");
			return e_failure;
		}
		

	}
	//Function call for checking the operation
	else if ( check_operation_type(argv) == e_decode)
	{
		if ( argc >= 3 && argc <= 4)
		{

	
		/*Checking CLA is gets passed in proper extensions*/
		if ( read_and_validate_decode_args(argv, &decInfo) == e_success)
		{
              printf("Selected decoding\n");
			printf("INFO : Read and validate success\n");
		}
		else
		{
			printf("ERROR : Read and validate failure\n");
			return e_failure;
		}
		//Function call for decoding
		if ( do_decoding(&decInfo) == e_success)
		{
			printf("Decoding success\n");
		}
		}
	
    	else
		{
        
		   printf("/a.out:Decodeing:./a.out -d < .bmpfile> [optional]\n");
			printf("please pass valid Arguments\n");
			return e_failure;
		}
	
	}
	//Printing Error For passing other than -e and -d
	else
	{
		printf("Error: Unsupported\n");
		printf("please pass valid Arguments\n");
		printf("INFO:Invalide option\n");
		return e_failure;
	}
	

	return 0;
}
//Check operation type function prototype
OperationType check_operation_type ( char *argv[])
{
	if ( strcmp (argv[1], "-e") == 0)
	{
		return e_encode;
	}
	else if ( strcmp (argv[1], "-d") == 0) 
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
}

