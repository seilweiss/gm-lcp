#include "gmMachine.h"
#include "gmLibHooks.h"
#include "gmStreamBuffer.h"

#include <stdio.h>

static void printUsage()
{
    printf("Usage:\n"
        "  compile [-g for gamecube] <input gm source file> <output gm lib file>");
}

int main(int argc, char** argv)
{
    int rc = 1;
    char* inpath = NULL;
    char* outpath = NULL;
    FILE* infile = NULL;
    FILE* outfile = NULL;
    int insize;
    char* source = NULL;
    gmMachine machine;
    gmStreamBufferDynamic stream;
    int outsize;
    int errors;

    machine.SetDebugMode(true);

    printf("GameMonkey source code compiler v1.0\n\n");

    if (argc < 3 || argc > 4)
    {
        printUsage();
        goto done;
    }

    if (argc == 3)
    {
        inpath = argv[1];
        outpath = argv[2];
    }
    else
    {
        if (strcmp(argv[1], "-g") != 0)
        {
            printUsage();
            goto done;
        }

        stream.SetEndianOnWrite(GM_ENDIAN_BIG);

        inpath = argv[2];
        outpath = argv[3];
    }

    infile = fopen(inpath, "rb");

    if (!infile)
    {
        printf("Error: could not open input file.");
        goto done;
    }

    fseek(infile, 0, SEEK_END);
    insize = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    source = new char[insize + 1];
    source[insize] = '\0';

    if (fread(source, insize, 1, infile) != 1)
    {
        printf("Error: could not read input file.");
        goto done;
    }

    fclose(infile);
    infile = NULL;

    errors = machine.CompileStringToLib(source, stream);

    if (errors)
    {
        bool first = true;
        const char* message;

        printf("Error: could not compile gm lib.\n\n");

        while (message = machine.GetLog().GetEntry(first))
        {
            printf("%s\n", message);
        }

        goto done;
    }

    outsize = stream.GetSize();
    outfile = fopen(outpath, "wb");

    if (!outfile)
    {
        printf("Error: could not open output file.");
        goto done;
    }

    if (fwrite(stream.GetData(), outsize, 1, outfile) != 1)
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