
#include <zlib.h>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <getopt.h>

#include "walker.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

using namespace std;

int main(int argc, char** argv)
{
	int c, index;
	
	bool compress = false;
	
	while((c=getopt(argc, argv, "c"))!=-1)
	{
		switch(c)
		{
			case 'c':
				compress = true;
				break;
			case '?':
					cerr << "Unknown option " << optopt << "\n";
				break;
			default:
				break;
		}
	}
	
	index = optind;
	
	if(argc-index!=2) 
	{
		cerr << "Bad number of non-option arguments (" << argc-index << "), expected two\n";
		return -1;
	}
	
	file_list files;
	
	cout << "Creating file list...\n";
	
	Walk(argv[index], 65535, files);
			
	/*for(int i=0;i<files.size();i++)
	{
		if(files[i].file)
			cout << "File Entry\n";
		else
			cout << "Directory Entry\n";
		cout << "ID:\t\t" << files[i].fileID << "\n";
		cout << "Parent:\t\t" << files[i].parentID << "\n";
		cout << "Name: \t\t" << files[i].filename <<"\n";
		cout << "Full name:\t" << files[i].fullname << "\n\n";
	}*/
	if(files.size()>65534)
	{
		cerr << "Too many files in selected directory.\nSDP format 0.1 can only handle 65,534 files and directories.\n";
		return -1;
	}
	
	cout << "Finished creating file list, now writing file table...\n";
	
	FILE* out;
		
	const int minor = 2;
	const int major = 0;
	
	unsigned short int numFiles = files.size();
	unsigned int fileSize;
	unsigned int fileSizeRaw; //size of file compressed (only present in compressed files)
	unsigned int filePos;
	
	out = fopen(argv[index+1], "wb");
	
	fputs("SDP", out);
	fputc(major, out);
	fputc(minor, out);
	
	fwrite(&numFiles, sizeof(short int), 1, out);
	
	file_list::iterator it;
	
	for(it=files.begin();it!=files.end();it++)
	{
		fwrite(&it->fileID, sizeof(short int), 1, out);
		
		unsigned char flags;
		flags = 0;
		if(!it->file)
			flags |= 0x01;
		if(compress)
			flags |= 0x02;
		fputc((int)flags, out);
		
		fwrite(&it->parentID, sizeof(short int), 1, out);
		
		fputs(it->filename, out);
		fputc((int)'\0', out);
		if(it->file)
		{
			fileSize = 0;
			filePos = 0;
			fwrite(&filePos, sizeof(int), 1, out);
			fwrite(&fileSize, sizeof(int), 1, out);
			if(compress)
				fwrite(&fileSizeRaw, sizeof(int), 1, out);
		}
	}
	
	cout << "Finished writing file table, now copying data...\n";
	if(compress)
		cout << "Using DEFLATE compression mode\n";
	
	const int CHUNK = 131072;
	
	unsigned char in_buf[CHUNK];
	unsigned char out_buf[CHUNK];
	
	for(it=files.begin();it!=files.end();it++)
	{
		if(!it->file)
			continue;
		FILE* in;
		in = fopen(it->fullname, "rb");
		it->size = 0;
		it->sizeRaw = 0;
		it->pos = ftell(out);
		if(!compress)
		{
			
			int c;
			while((c=fread(in_buf, sizeof(char), CHUNK, in))>0)
			{
				it->size+=c;
				fwrite(in_buf, sizeof(char), c, out);
			}
			//cout << it->filename << "\n";
			//cout << "size: " << it->size << "\n\n";
			fclose(in);
		}
		else
		{
			int ret, flush;
			unsigned have;
			z_stream strm;
			strm.zalloc = Z_NULL;
			strm.zfree = Z_NULL;
			strm.opaque = Z_NULL;
			ret = deflateInit(&strm, -1);
			if (ret != Z_OK)
				return ret;
			do
			{
				strm.avail_in = fread(in_buf, 1, CHUNK, in);
				it->size+=strm.avail_in;
				if (ferror(in)) 
				{
					deflateEnd(&strm);
					return Z_ERRNO;
				}
				flush = feof(in) ? Z_FINISH : Z_NO_FLUSH;
				strm.next_in = in_buf;
				do
				{
					strm.avail_out = CHUNK;
					strm.next_out = out_buf;
					ret = deflate(&strm, flush);    /* no bad return value */
					assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
					have = CHUNK - strm.avail_out;
					if (fwrite(out_buf, 1, have, out) != have || ferror(out)) 
					{
						deflateEnd(&strm);
						return Z_ERRNO;
					}
					it->sizeRaw += have;
				}while (strm.avail_out == 0);
				assert(strm.avail_in == 0);     /* all input will be used */
			}
			while(flush != Z_FINISH);
			assert(ret == Z_STREAM_END);        /* stream will be complete */
			deflateEnd(&strm);
		}
	}
	
	fclose(out);
	
	cout << "Finished copying data, now updating file table...\n";
	
	out = fopen(argv[index+1], "rb+");
	
	fseek(out, 7, SEEK_SET);
	
	int seek;
	
	for(it=files.begin();it!=files.end();it++)
	{
		seek = 5;
		seek += strlen(it->filename) + 1;
		fseek(out, seek, SEEK_CUR);
		if(it->file)
		{
			fwrite(&it->pos, sizeof(int), 1, out);
			fwrite(&it->size, sizeof(int), 1, out);
			if(compress)
				fwrite(&it->sizeRaw, sizeof(int), 1, out);
		}
	}
	
	fclose(out);
	
	cout << "Wrote " << argv[index+1] << " successfully ";
	cout << "with " << numFiles << " files and directories\n";
	
	return 0;
}
