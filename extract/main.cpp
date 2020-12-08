#include "gmMachine.h"
#include "gmLibHooks.h"
#include "gmStreamBuffer.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    int rc = 1;
    FILE* infile = NULL;
    FILE* outfile = NULL;
    int insize;
    char* buffer = NULL;
    gmuint32 magic;
    gmMachine machine;
    gmStreamBufferStatic stream;
    gmFunctionObject* func = NULL;
    const char* source;
    const char* fname;
    int outsize;

    machine.SetDebugMode(true);

    printf("GameMonkey source code extractor v1.0\n\n");

    if (argc != 3)
    {
        printf("Usage:\n"
               "  extract <input gm lib file> <output gm source file>");
        goto done;
    }

    infile = fopen(argv[1], "rb");

    if (!infile)
    {
        printf("Error: could not open input file.");
        goto done;
    }

    fseek(infile, 0, SEEK_END);
    insize = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    buffer = new char[insize];

    if (fread(buffer, insize, 1, infile) != 1)
    {
        printf("Error: could not read input file.");
        goto done;
    }

    fclose(infile);
    infile = NULL;

    stream.Open(buffer, insize);

    magic = *(gmuint32*)buffer;

    if (magic != 'gml0')
    {
        if (magic == '0lmg')
        {
            stream.SetSwapEndianOnWrite(true);
        }
        else
        {
            printf("Error: input file is not a valid gm lib.");
            goto done;
        }
    }
    
    func = gmLibHooks::BindLib(machine, stream, NULL);

    if (!func)
    {
        printf("Error: could not parse input file.");
        goto done;
    }

    if (!machine.GetSourceCode(func->GetSourceId(), source, fname))
    {
        printf("Error: no source code was found in input file.");
        goto done;
    }

    outsize = strlen(source);
    outfile = fopen(argv[2], "wb");

    if (!outfile)
    {
        printf("Error: could not open output file.");
        goto done;
    }

    if (fwrite(source, outsize, 1, outfile) != 1)
    {
        printf("Error: could not write output file.");
        goto done;
    }

    fclose(outfile);
    outfile = NULL;

    printf("Done.");

    rc = 0;

done:
    printf("\n");

    if (infile) fclose(infile);
    if (outfile) fclose(outfile);

    return rc;
}