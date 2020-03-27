// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile");
        return 1;
    }

    // ensure to pass only values between 1 and 100
    if (atoi(argv[1]) <= 0 || atoi(argv[1]) > 100)
    {
        fprintf(stderr, "Usage: n must be a positive integer less than or equal to 100");
        return 2;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    //Redimension factor
    int n = atoi(argv[1]);

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 5;
    }

    //original values for Width and Height
    int origWidth = bi.biWidth;
    int origHeight = bi.biHeight;

    // determine scanline padding from original bmp
    int originalpadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //New headers fields dimensions
    bi.biWidth *= n;
    bi.biHeight *= n;

    // determine padding for scanlines for resized bmp
    int newpadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //new bmp's biSizeImage and bfSize
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + newpadding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines (loop to iterate over original bmp rows)
    for (int i = 0, biHeight = abs(origHeight); i < biHeight; i++)
    {
        // variable and while, to control the times every row must be copied into new bmp
        int z = 0;
        while (n > z)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < origWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // copies every RGBTRIPLE n number of times into new bmp
                for (int k = 0; k < n; k++)
                    {
                        // write RGB triple times n to outfile
                        fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    }
            }

            // add new padding
            for (int k = 0; k < newpadding; k++)
            {
                fputc(0x00, outptr);
            }

            // go back at the bigining of the scanline so that it can be copied n number of times
            fseek(inptr, -(origWidth * sizeof(RGBTRIPLE)), SEEK_CUR);

            // increment control row variable for copied scanlines
            z++;
        }

        // move to the end of the scanline so that the next scanline gets copied
        fseek(inptr, ((origWidth * sizeof(RGBTRIPLE)) + originalpadding), SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
