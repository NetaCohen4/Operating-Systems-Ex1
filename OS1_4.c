#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void myzip(const char *folderPath) {
    // Create the tar command
    char tarCommand[100];
    sprintf(tarCommand, "tar -czf - -C %s .", folderPath);

    // Specify the output file for GPG
    char gpgOutputFile[] = "output.gpg";

    // Create the gpg command
    char gpgCommand[150];
    sprintf(gpgCommand, "gpg --symmetric --output %s", gpgOutputFile);

    // Open pipes for tar, gpg
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

    // Transfer data through the pipes
    char buffer[4096];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), tarPipe)) > 0) {
        fwrite(buffer, 1, bytesRead, gpgPipe);
    }

    pclose(tarPipe);

    // Wait for gpg process to finish
    int gpgStatus = pclose(gpgPipe);

    if (gpgStatus != 0) {
        fprintf(stderr, "GPG process exited with status %d\n", gpgStatus);
        exit(EXIT_FAILURE);
    }

    printf("Successfully created encrypted and compressed file: %s\n", gpgOutputFile);
}

void myunzip(const char *gpgFilePath) {
    // Create the gpg decryption command
    char gpgDecryptCommand[150];
    sprintf(gpgDecryptCommand, "gpg --decrypt %s", gpgFilePath);

    // Open a pipe for gpg decryption
    FILE *gpgDecryptPipe = popen(gpgDecryptCommand, "r");
    if (!gpgDecryptPipe) {
        perror("popen gpg decrypt");
        exit(EXIT_FAILURE);
    }

    // Create the tar extraction command
    char tarExtractCommand[] = "tar -xzf - -C ./";  // Change this line

    // Open a pipe for tar extraction
    FILE *tarExtractPipe = popen(tarExtractCommand, "w");
    if (!tarExtractPipe) {
        perror("popen tar extract");
        exit(EXIT_FAILURE);
    }

    // Transfer data through the pipes
    char buffer[4096];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), gpgDecryptPipe)) > 0) {
        fwrite(buffer, 1, bytesRead, tarExtractPipe);
    }

    pclose(gpgDecryptPipe);

    // Wait for tar extraction process to finish
    int tarExtractStatus = pclose(tarExtractPipe);

    if (tarExtractStatus != 0) {
        fprintf(stderr, "Tar extraction process exited with status %d\n", tarExtractStatus);
        exit(EXIT_FAILURE);
    }

    printf("Successfully decrypted and extracted contents to: ./unzipped\n");  // Change this line
}



int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <myzip|myunzip> <directory|encrypted_compressed_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "myzip") == 0) {
        myzip(argv[2]);
    } else if (strcmp(argv[1], "myunzip") == 0) {
        myunzip(argv[2]);
    } else {
        fprintf(stderr, "Usage: %s <myzip|myunzip> <directory|encrypted_compressed_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}
