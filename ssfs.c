#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/bonisaz/Sisop/modul_4/test_fuse";
char* key = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

// 1. Fungsi encrypt1
void encryptString(char *source)
{
  char new[100];
  strcpy(new, source);
  for(int i = 0; i < strlen(new); i++)
  {
    int j=0;
    while(1)
    {
      if(new[i] == key[j])
      {
        new[i] = key[j + 10];
        break;
      }
      j++;
    }
  }
  strcpy(source, new);
}

void encrypt(char *source)
{
    int flag = 1;
    char str[100];
  strcpy(str, source);
  
  char* token = strtok(str, "/");
  char encv1[5];

  strncpy(encv1, token, 6); // ngambil 6 char paling depan
  if (strcmp(encv1, "encv1_") == 0)
  {
    sprintf(source, "%s", token);
    token = strtok(NULL, "/"); 
    while (token != NULL) 
    { 
        char* token_ext = strrchr(token, '.'); // cek ada ekstensi atau gak
        char ext[10];
        if(token_ext){
            printf("%s Ada ekstensi\n", token);
            
            char* cocok;

            strcpy(ext, token_ext); // ekstensinya dimasukin ke string sendiri (ext)
            printf("ekstensi: %s\n", ext);
            
            // buat ngehapus ekstensi dari token
            while ((cocok = strstr(token, ext)))
            {
                *cocok = '\0';
                strcat(token, cocok+(strlen(ext)));
            }
            
            flag = 0;
        }else{
            printf("%s Gaada ekstensi\n", token);
        }

        if(flag){
            encryptString(token);
            printf("token: %s\n", token);
            strcat(source, "/");
            strcat(source, token);
            token = strtok(NULL, "/"); 
        }else{
            printf("Masuk ekstensi\n");
            encryptString(token);
            printf("token: %s\n", token);
            strcat(source, "/");
            strcat(source, token);
            strcat(source, ext); // sama kek kodingan ega, cuman akhirnya ditambah ekstensi
            token = strtok(NULL, "/"); 
        }
    }
  }
}

void decryptString(char *source)
{
  char new[100];
  strcpy(new, source);
  for(int i = 0; i < strlen(new); i++)
  {
    int j = strlen(key);
    while(1)
    {
      if(new[i] == key[j])
      {
        new[i] = key[j - 10];
        break;
      }
      j--;
    }
  }
  strcpy(source, new);
}

void decrypt(char *source)
{
	int flag = 1;
    char str[100];
    strcpy(str, source);
  
    char* token = strtok(str, "/");
    char encv1[5];

    strncpy(encv1, token, 6);
  if (strcmp(encv1, "encv1_") == 0)
  {
    sprintf(source, "%s", token);
    token = strtok(NULL, "/"); 
    while (token != NULL) 
    { 
        char* token_ext = strrchr(token, '.');
        char ext[10];
        if(token_ext){
            printf("%s Ada ekstensi\n", token);
            
            char* cocok;

            strcpy(ext, token_ext);
            printf("ekstensi: %s\n", ext);
            
            while ((cocok = strstr(token, ext)))
            {
                *cocok = '\0';
                strcat(token, cocok+(strlen(ext)));
            }
            
            flag = 0;
        }else{
            printf("%s Gaada ekstensi\n", token);
        }

        if(flag){
            decryptString(token);
            printf("token: %s\n", token);
            strcat(source, "/");
            strcat(source, token);
            token = strtok(NULL, "/"); 
        }else{
            printf("Masuk ekstensi\n");
            decryptString(token);
            printf("token: %s\n", token);
            strcat(source, "/");
            strcat(source, token);
            strcat(source, ext);
            token = strtok(NULL, "/"); 
        }
      
    }
  }
}

// Akses file
static int xmp_getattr(const char *path, struct stat *stbuf){
    int res;
    char fpath[1000];

    printf("getattr: %s\n", path);
    // char* flag_encrypt_1 = strstr(path, "encv1_");

    // if(flag_encrypt_1 != NULL)
        decrypt(path);

    sprintf(fpath,"%s%s",dirpath,path);
    res = lstat(fpath, stbuf);
    
    if (res == -1)
    return -errno;

    return 0;
}

// Akses folder
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi){
    char fpath[1000];
    char name[1000];

    printf("readdir: %s\n", path);
    // char* flag_encrypt_1 = strstr(path, "encv1_");
    
    // if(flag_encrypt_1 != NULL)
        decrypt(path);

    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }else{
        sprintf(fpath, "%s%s",dirpath,name);
    }

    int res = 0;
    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    // Buka folder
    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL){
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        ////
        // if(flag_encrypt_1 != NULL)
            encrypt(de->d_name);
        ////
        res = (filler(buf, de->d_name, &st, 0));
        if(res!=0) break;
    }

    closedir(dp);

    return 0;
}  

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi){

    printf("read: %s\n", path);
    // decrypt(path);

    char fpath[1000];
    sprintf(fpath, "%s%s",dirpath,path);
    int res = 0;
    int fd = 0 ;

    (void) fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);

    return res;
}


static struct fuse_operations xmp_oper = {
.getattr    = xmp_getattr,
.readdir    = xmp_readdir,
.read       = xmp_read,
};

  
int main(int  argc, char *argv[]){
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}