#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 512
char *line = NULL;
size_t len = 0;
FILE *wfp;
FILE *rfp;
char comp[] = "";
int fsize(FILE *stream) {
    /**
     * File size function
     * Returns siz of file specified by stream
     */
    fseek(stream, 0, SEEK_END);
    return ftell(stream);
}
int main(int argc, char *argv[])
{
    if(argc==1){
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    int size = 0;
    for (int i = 1; i < argc; ++i) {
        // For each file get size
        FILE *stream = fopen(argv[i], "r");
        size += fsize(stream);
        fclose(stream);
    }

    // Make a buffer of the sum-size
    unsigned char *buffer = (unsigned char *) malloc(size);

    int n = 0;
    for (int i = 1; i < argc; ++i) {
        // Read all the files into the buffer
        FILE *stream = fopen(argv[i], "r");
        if(stream==NULL){
          printf("wzip: cannot open file\n");
          exit(1);
        }
        int c;
        while((c=fgetc(stream))!=EOF) {
            buffer[n] = c;
            ++n;
        }
        fclose(stream);
    }

    int count = 1;
    for (int i = 0; i < size;i++) {
        if (buffer[i] != buffer[i+1]) {
            fwrite(&count, 4, 1, stdout);
            printf("%c", buffer[i]);
            count = 1;
        } else {
            count ++;
        }
    }
    return 0;
}