/*
Operating Systems Exercize 1.4
Matan Blaich
Neta Cohen 325195774
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void myzip(const char *folderPath) {
    
    // Creating the tar command:
    char tarCommand[100];
    sprintf(tarCommand, "tar -czf - -C %s .", folderPath);

    // Zipped file name:
    char gpgOutputFile[] = "output.gpg";

    // Creating the gpg command:
    char gpgCommand[150];
    sprintf(gpgCommand, "gpg --symmetric --output %s", gpgOutputFile);

    // Opening pipes for tar and gpg:
    FILE *tarPipe = popen(tarCommand, "r");
    if (!tarPipe) {
        perror("popen tar");
        exit(EXIT_FAILURE);
    }

    FILE *gpgPipe = popen(gpgCommand, "w");
    if (!gpgPipe) {
        perror("popen gpg");
        exit(EXIT_FAILURE);
    }

    // Transfering data through the pipes:
    char buffer[4096];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), tarPipe)) > 0) {
        fwrite(buffer, 1, bytesRead, gpgPipe);
    }

    //closing pipes:
    pclose(tarPipe);
    pclose(gpgPipe);

    printf("Successfully created zipped file: %s\n", gpgOutputFile);
}

void myunzip(const char *gpgFilePath) {
    
    // Creating the gpg decryption command:
    char gpgDecryptCommand[150];
    sprintf(gpgDecryptCommand, "gpg --decrypt %s", gpgFilePath);

    // Opening a pipe for gpg decryption:
    FILE *gpgDecryptPipe = popen(gpgDecryptCommand, "r");
    if (!gpgDecryptPipe) {
        perror("popen gpg decrypt");
        exit(EXIT_FAILURE);
    }

    // Creating the tar extraction command:
    char tarExtractCommand[] = "tar -xzf - -C ./";  // Change this line

    // Opening a pipe for tar extraction:
    FILE *tarExtractPipe = popen(tarExtractCommand, "w");
    if (!tarExtractPipe) {
        perror("popen tar extract");
        exit(EXIT_FAILURE);
    }

    // Tranfering data through the pipes:
    char buffer[4096];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), gpgDecryptPipe)) > 0) {
        fwrite(buffer, 1, bytesRead, tarExtractPipe);
    }

    //closing pipes:
    pclose(gpgDecryptPipe);
    pclose(tarExtractPipe);

    printf("Successfully unzipped in this folder"); 
}



int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <myzip|myunzip> </path/to/folder|/path/to/zipped/fplder>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[1], "myzip") == 0) {
        myzip(argv[2]);
    } 
    else if (strcmp(argv[1], "myunzip") == 0) {
        myunzip(argv[2]);
    } 
    else {
        fprintf(stderr, "Usage: %s <myzip|myunzip> </path/to/folder|/path/to/zipped/fplder>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}
