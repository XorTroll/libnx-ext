#include "log.hpp"

#define LOG_FILE "sdmc:/cruiser-example.log"

static bool initialized = false;

// Log both to stdout and a file

void customLog(const char *buf, size_t buf_size) {
    if(!initialized) {
        remove(LOG_FILE);
        initialized = true;
    }

    printf("%s\n", buf);
    consoleUpdate(NULL);

    FILE *log_file = fopen(LOG_FILE, "a");
    if(log_file == nullptr) {
        printf("Failed to open log file\n");
        return;
    }
    else {
        fprintf(log_file, "%s\n", buf);
        fflush(log_file);
        fclose(log_file);
    }
}
