
#ifndef _DATA_HEADER_H_
#define _DATA_HEADER_H_

#include <PshPack1.h>

struct z_packheader
{
#define  z_magic  0x1234

	unsigned int magic;
	unsigned int product_id;
	unsigned int target_id;
	unsigned int data_type;		// ¼û z_datatype
	unsigned long data_size;
	unsigned char va[1];
};

enum z_datatype
{
	z_file_normal,
	z_file_image,
	z_file_exec
};

struct z_io_data
{
	unsigned long to_read;
	char buf[2048];

	unsigned long read_size;
	unsigned long write_size;
	struct z_packheader pack_header;
	struct z_packheader *ppack_header;
};

#include <PopPack.h>

#endif // DATA_HEADER_H_