#include <stdio.h>
#include <stdlib.h>
//#include <cs50.h>

int main(int argc, char *argv[])
{
    //ensure proper usage
    if (argc != 2)
    {
        fprintf (stderr, "Usage: ./recover card.raw\n");
        return 1;
    }

    //keep track of card.raw file
    char *rawdata = argv[1];

    //open card.raw file
    FILE *rawinfo = fopen(rawdata, "r");
    if (rawinfo == NULL)
    {
        fprintf(stderr, "Could not open file %s\n", rawdata);
        return 2;
    }

    //buffer to load 512bytes at a time
    unsigned char buffer[512];

    //Flag to know if it's the first jpeg found
    int found = 0;

    //Variable to keep track of the number of jpegs found (and for the file name)
    int i = 1;

    //FILE variable to save every jpeg, one at a time
    FILE *img = NULL;

    //Bucle to fetch 512 bytes at a time, and only if it can retrive 512 bytes it will perform steps in the bucle
    while (fread(&buffer, sizeof(char), 512, rawinfo) == 512)
    {
        //Checks for the begining of a new jpeg
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            //Variable to store the name of the files
            char filename[3];

            //Checks if this is the first jpeg found and opens a new file if it is
            if (found == 0)
            {
                sprintf(filename, "%03i.jpg", i++);
                img = fopen(filename, "w");
                found = 1;
            }

            //If a jepeg has been found, the file gets closed and a new file gets open
            else
            {
                fclose(img);
                sprintf(filename, "%03i.jpg", i++);
                img = fopen(filename, "w");
            }
        }

        //If a jpeg has been found, the below code, writes the data in the current file.
        if (found == 1)
            {
                fwrite(&buffer, sizeof(char), 512, img);
            }
    }

    fclose (img);
    return 0;
}
