/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#include <cstring>
#include <zlib.h>
#include <iostream>
#include <cstdio>
#include <cassert>

#include <boost/program_options.hpp>

#include "walker.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

using namespace std;

// disable warnings about fopen v fopen_s
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

void showHelp(const char* program_name, boost::program_options::options_description options)
{
    std::cout << "Usage: " << program_name << " [OPTIONS] output-file input-files\n";
    std::cout << options;
}

int main(int argc, char** argv)
{
    boost::program_options::options_description generic("Generic Options");
    generic.add_options()
        ("help,h", "Show help");

    boost::program_options::options_description config("Configuration");
    config.add_options()
        ("compression,c", "Compress output")
        ("prefix,p", boost::program_options::value< std::string >(),
         "Folder prefix for archived files");

    boost::program_options::options_description hidden("Hidden");
    hidden.add_options()
        ("files", boost::program_options::value< vector<string> >(), "Files to add to archive")
        ("archive-name", boost::program_options::value< string >(), "Output archive name");

    boost::program_options::options_description visible;
    visible.add(generic).add(config);

    boost::program_options::options_description all;
    all.add(visible).add(hidden);

    boost::program_options::positional_options_description po;
    po.add("archive-name", 1);
    po.add("files", -1);

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
              options(all).positional(po).run(), vm);
    boost::program_options::notify(vm);


    if(!vm.count("files") || !vm.count("archive-name"))
    {
        showHelp(argv[0], visible);
        return -1;
    }

    unsigned short int top = 65535;
    file_list files;
    std::string prefix;

    if(vm.count("prefix"))
    {
        prefix = vm["prefix"].as<string>();
        if(prefix.at(prefix.length()-1) != '/') prefix += "/";
        //take all but the trailing slash so the path iterator doesn't generator a '.'
        boost::filesystem::path prefix_path(prefix.substr(0, prefix.size()-1));
        boost::filesystem::path p("");
        for(boost::filesystem::path::iterator it=prefix_path.begin(); it!=prefix_path.end(); ++it)
        {
            p /= *it;
            std::string thisPrefix = p.parent_path().string();
            if(thisPrefix.size()) thisPrefix += "/";
            top = AddFile(*it, top, thisPrefix, files);
        }

    }

    cout << "Creating file list...\n";

    std::vector<std::string> inputFiles = vm["files"].as<std::vector<std::string> >();

    for(std::vector<std::string>::iterator it=inputFiles.begin(); it!=inputFiles.end(); ++it)
    {
        boost::filesystem::path p(*it);
        if(!boost::filesystem::exists(p))
            std::cerr << "Cannot add " << p.string() << ": path does not exist\n";
        else if(boost::filesystem::is_directory(boost::filesystem::status(p)))
            Walk(p, top, prefix, files);
        else
            AddFile(p, top, prefix, files);
    }
    if(files.size()>65534)
    {
        cerr << "Too many files in selected directory." << endl;
        cerr << "SDP format 0.1 can only handle 65,534 files and directories." << endl;
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

    out = fopen(vm["archive-name"].as<string>().c_str(), "wb");

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
        if(vm.count("compression"))
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
            if(vm.count("compression"))
                fwrite(&fileSizeRaw, sizeof(int), 1, out);
        }
    }

cout << "Finished writing file table, now copying data...\n";
    if(vm.count("compression"))
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
        if(!vm.count("compression"))
        {

            int c;
            while((c=fread(in_buf, sizeof(char), CHUNK, in))>0)
            {
                it->size+=c;
                fwrite(in_buf, sizeof(char), c, out);
            }
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

    out = fopen(vm["archive-name"].as<std::string>().c_str(), "rb+");

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
            if(vm.count("compression"))
                fwrite(&it->sizeRaw, sizeof(int), 1, out);
        }
    }

    fclose(out);

    cout << "Wrote " << vm["archive-name"].as<std::string>() << " successfully ";
    cout << "with " << numFiles << " files and directories\n";

    return 0;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif