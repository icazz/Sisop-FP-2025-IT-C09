#define FUSE_USE_VERSION 35

#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>

#define SOURCE_DIR "repo"
#define LOG_FILE   "log.txt"

static const char *source_dir = SOURCE_DIR;

#define MAX_LOGGED 1024
static char logged[MAX_LOGGED][256];
static int log_count = 0;

static char previous_visible[MAX_LOGGED][256];
static int previous_count = 0;

static char previous_hidden[MAX_LOGGED][256];
static unsigned char previous_hidden_type[MAX_LOGGED];
static int previous_hidden_count = 0;

int is_logged(const char *filename) {
    for (int i = 0; i < log_count; i++) {
        if (strcmp(logged[i], filename) == 0)
            return 1;
    }
    return 0;
}

void add_to_logged(const char *filename) {
    if (log_count < MAX_LOGGED) {
        strncpy(logged[log_count], filename, 255);
        logged[log_count][255] = '\0';
        log_count++;
    }
}

void append_log(const char *type, const char *filename, unsigned char d_type) {
    if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
        return;

    if ((strcmp(type, "visible") == 0 || strcmp(type, "hidden") == 0) && is_logged(filename))
        return;

    if (strcmp(type, "visible") == 0 || strcmp(type, "hidden") == 0)
        add_to_logged(filename);

    FILE *logf = fopen(LOG_FILE, "a");
    if (!logf) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    const char *obj_type = (d_type == DT_DIR) ? "folder" : "file";

    if (strcmp(type, "hidden") == 0) {
        fprintf(logf, "[%02d:%02d:%02d] [hidden] \"%s\" merupakan %s tersembunyi\n",
                t->tm_hour, t->tm_min, t->tm_sec, filename, obj_type);
    } else if (strcmp(type, "visible") == 0) {
        fprintf(logf, "[%02d:%02d:%02d] [visible] %s \"%s\" berhasil ditambahkan\n",
                t->tm_hour, t->tm_min, t->tm_sec, obj_type, filename);
    } else if (strcmp(type, "delete") == 0) {
        fprintf(logf, "[%02d:%02d:%02d] [delete] \"%s\" berhasil dihapus\n",
                t->tm_hour, t->tm_min, t->tm_sec, filename);
    }

    fclose(logf);
}

static void fullpath(char fpath[PATH_MAX], const char *path) {
    snprintf(fpath, PATH_MAX, "%s%s", source_dir, path);
}

static int x_getattr(const char *path, struct stat *stbuf,
                     struct fuse_file_info *fi) {
    (void) fi;
    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int res = lstat(fpath, stbuf);
    return res == -1 ? -errno : 0;
}

static int x_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                     off_t offset, struct fuse_file_info *fi,
                     enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;
    (void) flags;

    char fpath[PATH_MAX];
    fullpath(fpath, path);

    DIR *dp = opendir(fpath);
    if (dp == NULL) return -errno;

    struct dirent *de;
    char current_visible[MAX_LOGGED][256];
    int current_count = 0;

    char current_hidden[MAX_LOGGED][256];
    unsigned char current_hidden_type[MAX_LOGGED];
    int current_hidden_count = 0;

    while ((de = readdir(dp)) != NULL) {
        const char *name = de->d_name;

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
            filler(buf, name, NULL, 0, 0);
            continue;
        }

        if (name[0] == '.') {
            append_log("hidden", name, de->d_type);

            if (current_hidden_count < MAX_LOGGED) {
                strncpy(current_hidden[current_hidden_count], name, 255);
                current_hidden[current_hidden_count][255] = '\0';
                current_hidden_type[current_hidden_count] = de->d_type;
                current_hidden_count++;
            }

            continue;
        }

        append_log("visible", name, de->d_type);

        if (current_count < MAX_LOGGED) {
            strncpy(current_visible[current_count], name, 255);
            current_visible[current_count][255] = '\0';
            current_count++;
        }

        struct stat st = {0};
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, name, &st, 0, 0)) break;
    }

    if (strcmp(path, "/") == 0) {
        for (int i = 0; i < previous_count; i++) {
            int found = 0;
            for (int j = 0; j < current_count; j++) {
                if (strcmp(previous_visible[i], current_visible[j]) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                append_log("delete", previous_visible[i], DT_REG);
            }
        }

        for (int i = 0; i < previous_hidden_count; i++) {
            int found = 0;
            for (int j = 0; j < current_hidden_count; j++) {
                if (strcmp(previous_hidden[i], current_hidden[j]) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                append_log("delete", previous_hidden[i], previous_hidden_type[i]);
            }
        }

        previous_count = current_count;
        for (int i = 0; i < current_count; i++) {
            strncpy(previous_visible[i], current_visible[i], 255);
            previous_visible[i][255] = '\0';
        }

        previous_hidden_count = current_hidden_count;
        for (int i = 0; i < current_hidden_count; i++) {
            strncpy(previous_hidden[i], current_hidden[i], 255);
            previous_hidden[i][255] = '\0';
            previous_hidden_type[i] = current_hidden_type[i];
        }
    }

    closedir(dp);
    return 0;
}

static int x_open(const char *path, struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int res = open(fpath, fi->flags);
    return res == -1 ? -errno : 0;
}

static int x_read(const char *path, char *buf, size_t size,
                  off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;
    int res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;
    close(fd);
    return res;
}

static const struct fuse_operations x_oper = {
    .getattr = x_getattr,
    .readdir = x_readdir,
    .open    = x_open,
    .read    = x_read,
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <mount_point>\n", argv[0]);
        return 1;
    }

    umask(0);
    return fuse_main(argc, argv, &x_oper, NULL);
}
