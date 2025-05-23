#ifndef FILE_SYS
#define FILE_SYS

#include <dirent.h>
#include <sys/stat.h>

#include "strings.cpp"

bool remove_file(const char *path);

struct File {
   FILE *fd;
   const char *path;

   ~File() {
      int err = fclose(fd);

      // Closing the file shouldn't fail and if it does then it's not a big deal.
      // Just report the error.
      if (err != 0) report_error("failed to close file: %", path);
   }

   uint file_length() {
      fseek(fd, 0, SEEK_END);
      uint file_size = ftell(fd);
      rewind(fd);

      return file_size;
   }

   int write(const char *string) {
      return fprintf(fd, "%s", string);
   }

   // Read the entire contents of the file into a string.
   String read_into_string(Fixed_Allocator *allocator) {
      const uint file_len = file_length();

      auto buf = string_with_size(allocator, file_len);
      fread(buf.buf, sizeof(char), file_len, fd);

      return buf;
   }

   String read_into_string() {
      const uint file_len = file_length();

      auto buf = string_with_size(file_len);
      fread(buf.buf, sizeof(char), file_len, fd);

      return buf;
   }
};

struct Directory {
   DIR *fd;
   const char *path;

   ~Directory() {
      int err = closedir(fd);

      // Closing the directory also shouldn't fail and if does then report the error.
      if (err != 0) report_error("failed to close directory: %", path);
   }

   // Resets the directory stream back to the beginning of the directory.
   bool is_empty() {
      uint count = 0;
      while (readdir(fd) != NULL) count += 1;

      if (count <= 2) {
         rewinddir(fd);
         return true;
      }

      rewinddir(fd);
      return false;
   }
};

File open_file(const char *file_path, const char *modes) {
   FILE *file = fopen(file_path, modes);

   if (file == NULL) fatal_error("failed to open file: %", file_path);

   return File {
      .fd = file,
      .path = file_path
   };
}

bool file_exists(const char *path) {
   FILE *file = fopen(path, "r");

   if (file == NULL) return false;

   fclose(file);
   return true;
}

bool dir_exists(const char *path) {
   DIR *dir = opendir(path);
   if (dir == NULL) {
      closedir(dir);
      return false;
   }

   closedir(dir);
   return true;
}

bool is_symlink(const char *path) {
   struct stat st;
   if (lstat(path, &st) != 0) return false;

   if ((st.st_mode & S_IFMT) == S_IFLNK) return true;
   return false;
}

bool is_file(const char *path) {
   struct stat st;
   if (lstat(path, &st) != 0) return false;

   if ((st.st_mode & S_IFMT) == S_IFREG) return true;
   return false;
}

bool is_dir(const char *path) {
   struct stat st;
   if (lstat(path, &st) != 0) return false;

   if ((st.st_mode & S_IFMT) == S_IFDIR) return true;
   return false;
}

// Return's true on success otherwise false
bool remove_file(const char *path) {
   int err = unlink(path);

   if (err != 0) {
      report_error("failed to remove file '%'", path);
      return false;
   }

   return true;
}

// Return's true on success otherwise false
bool move_file(const char *oldpath, const char *newpath) {
   int err = rename(oldpath, newpath);

   if (err != 0) {
      report_error("failed to move '%' to '%'", oldpath, newpath);
      return false;
   }

   return true;
}

String file_basename(Fixed_Allocator *allocator, String *path) {
   if ((*path)[path->len()-1] == '/') {
      path->remove(path->len()-1);
   }

   bool contain_slash = false;
   for (uint i = 0; i < path->len(); i++) {
      if ((*path)[i] == '/') {
         contain_slash = true;
         break;
      }
   }

   if (!contain_slash) return *path;

   auto buffer = string_with_size(allocator, path->len());
   uint file_idx = 0;

   for (int i = path->len(); (*path)[i] != '/'; i--) {
      file_idx++;
   }

   buffer = path->buf;
   buffer.skip(buffer.len() - file_idx+1);

   return buffer;
}

Directory open_dir(const char *dir_path) {
   DIR *dir = opendir(dir_path);
   if (dir == NULL) fatal_error("failed to open directory: %", dir_path);

   return Directory {
      .fd = dir,
      .path = dir_path,
   };
}

enum File_Type : u8 {
   file = 0,
   dir,
   lnk,
   unknown,
};

struct Ex_Res {
   bool found = false;
   File_Type type = unknown;
};

// Check if file|directory exists
Ex_Res exists(const char *path) {
   struct stat st;
   Ex_Res ret;

   if (lstat(path, &st) != 0) return ret;

   auto st_mode = st.st_mode & S_IFMT;

   switch (st_mode) {
      case S_IFREG: {
         ret.type = file;
         ret.found = true;
         return ret;
      }

      case S_IFDIR: {
         ret.type = dir;
         ret.found = true;
         return ret;
      }

      case S_IFLNK: {
         ret.type = lnk;
         ret.found = true;
         return ret;
      }
   }

   return ret;
}

uint file_length(FILE *path) {
   fseek(path, 0, SEEK_END);
   uint file_size = ftell(path);
   rewind(path);

   return file_size;
}

String fread_into_string(Fixed_Allocator *allocator, FILE *path) {
   const uint file_len = file_length(path);

   auto buf = string_with_size(allocator, file_len);
   fread(buf.buf, sizeof(char), file_len, path);

   return buf;
}

#endif // FILE_SYS
