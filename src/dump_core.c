#include <stdio.h>
#include <stdlib.h>

#include "../include/dump_core.h"
#include "../include/utils/utils.h"

int get_dump_amount_of_lines( dump_file *dump )
{
	FILE *dump_file 					= NULL;

	if( dump == NULL || dump->dump_path == NULL )
		return -1;

	dump_file = fopen( dump->dump_path, "r" );
	if( dump_file == NULL )
		return -1;

	char *cur_line						= NULL;
	size_t cur_len 						= 0;

	unsigned long amount_of_lines		= 0;

	while( getline( &cur_line, &cur_len, dump_file ) != -1 )
	{
		amount_of_lines++;
	}

	free( cur_line );

	fclose( dump_file );

	dump->rom_length 			= amount_of_lines * 32;
	dump->translated_length 	= amount_of_lines * 16;

	return 0;
}

int read_dump_file( dump_file *dump )
{
	FILE *dump_file 						= NULL;

	if( dump == NULL || dump->dump_path == NULL || dump->rom_buffer == NULL || dump->translated_buffer == NULL )
		return -1;	

	dump_file = fopen( dump->dump_path, "r" );
	if( dump_file == NULL )
		return -1;

	char *cur_line							= NULL;
	size_t cur_len 							= 0;

	unsigned char address[ 11 ]				= { 0 };

	unsigned char byte_literal[ 72 ] 		= { 0 };
	unsigned char byte_hex[ 32 ] 			= { 0 };

	unsigned char translated_literal[ 18 ] 	= { 0 };

	unsigned long cur_rom_buffer_pos		= 0;

	while( getline( &cur_line, &cur_len, dump_file ) != -1 )
	{
		sscanf( cur_line, "%s |\t%71c \t|%17c", address, byte_literal, translated_literal );

		remove_spaces_from_string( byte_literal );

		byte_literal_to_hex_value( byte_hex, byte_literal, 64 );
		memcpy( dump->rom_buffer + cur_rom_buffer_pos, byte_hex, 32 );
		//first character is a space to deal with sscanf not filling values correctly otherwise
		memcpy( dump->translated_buffer + (cur_rom_buffer_pos / 2 ), translated_literal + sizeof( unsigned char ), 16 );
		cur_rom_buffer_pos += 32;

		memset( translated_literal, 0, 18 );
	}

	free( cur_line );

	fclose( dump_file );

	return 0;
}

int write_translated_dump( dump_file *dump )
{
	char readable 						= 0;
	unsigned char byte_value[ 2 ]		= { 0 };

	unsigned char new_byte_value[ 2 ]	= { 0 };

	if( dump == NULL || get_byte_to_readable_hash() == NULL )
		return -1;

	for( int i = 0; i < dump->rom_length; i += 2 )
	{
		memcpy( byte_value, dump->rom_buffer + i, 2 );

		if( (readable = find_byte_to_readable_hash_value( byte_value ) ) != 0 )
		{
			if( dump->translated_buffer[ i / 2 ] != readable )
			{
				if( 1 == find_readable_to_byte_hash_value( dump->translated_buffer[ i / 2 ], new_byte_value ) )
				{
					memcpy( dump->rom_buffer + i, new_byte_value, 2 );
				}
			}
		}
	}

	fwrite( dump->rom_buffer, dump->rom_length, 1, stdout );

	delete_byte_to_readable_hash( );

	return 0;
}