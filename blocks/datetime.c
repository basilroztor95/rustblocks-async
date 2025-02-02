#include <stdio.h>
#include <time.h>

#define BUF_SIZE 128 // Define a buffer size large enough for the formatted string

const char * datetime(const char *fmt)
{
    static char buf[BUF_SIZE]; // static buffer to persist across calls
    time_t t;

    t = time(NULL);
    if (t == (time_t)-1) {
        fprintf(stderr, "time: Failed to get current time\n");
        return NULL;
    }

    struct tm *local_tm = localtime(&t);
    if (!local_tm) {
        fprintf(stderr, "localtime: Failed to convert time\n");
        return NULL;
    }

    if (!strftime(buf, sizeof(buf), fmt, local_tm)) {
        fprintf(stderr, "strftime: Result string exceeds buffer size\n");
        return NULL;
    }

    return buf;
}

int main() {
    const char *fmt = "%Y-%m-%d %H:%M:%S";
    const char *result = datetime(fmt);
    if (result) {
        printf("%s", result);
    }
    return 0;
}

