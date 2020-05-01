#define FUSE_USE_VERSION 28

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <wait.h>

static const char *dirpath = "/home/bonisaz/Documents";
char* key = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
char encv1[10] = "encv1_";

// Periksa apakah ada titik atau gak pada suatu string
// kalau ada, return posisi titik-nya
// kalau gaada, return panjang string "path"
int dot(char* path){
	int i;
	int panjang = strlen(path);

	for(i = panjang - 1; i >= 0; i--){
		if(path[i] == '.'){
			return i;
		}
	}

	return panjang;
}

// Periksa apakah ada slash atau gak pada suatu string
// kalau ada, return posisi slash-nya
// kalau gaada, return 0 (awal string)
int slash(char* path, int n){
	int i;

	for(i = 0; i < strlen(path); i++){
		if(path[i] == '/'){
			return i + 1;
		}
	}

	return n;
}

// Fungsi untuk melakukan encrypt & decrypt dari posisi awal string
// hingga ke akhir yang ditentukan berdasarkan ada gak nya
// '/' atau '.' pada string path
// apa == 1 untuk encrypt
// apa == 2 untuk decrypt
void cipher(char *path, char a){
    if (!strcmp(path, ".") || !strcmp(path, ".."))
		return;
    
    int flag, awal;

    int temp = 0;
    int panjang_key = strlen(key);

	int akhir = dot(path);

    if(a == 'e'){
        flag = 1;
        awal = slash(path, 0);
    }else if(a == 'd'){
        flag = 2;
        awal = slash(path, akhir);
    }
    
    for (int i = awal; i < akhir; i++){
		if(path[i] != '/'){ // error handling jika string berisi slash
			for (int j = 0; j < panjang_key; j++){
				if (path[i] == key[j]){
                    if(flag == 1){       // encyrpt
                        temp = (j + 10) % panjang_key;
                        path[i] = key[temp];
                        break;
                    }else if(flag == 2){     // decrypt
                        temp = (j + (panjang_key - 10)) % panjang_key;
                        path[i] = key[temp];
                        break;
                    }
				}
			}
		}
	}
}

// Akses atribut dari file atau folder
// Setiap kali mengakses sebuah file atau folder,
// diperlukan atribut bahwa file atau folder tersebut
// benar-benar ada. maka dari itu digunakan lstat
static int xmp_getattr(const char *path, struct stat *stbuf){
    int res;
    char fpath[1000];
    
    // debug getattr
    // printf("getattr: %s\n", path);
    char *flag_encrypt_1 = strstr(path, encv1);

    if(flag_encrypt_1 != NULL)
        cipher(flag_encrypt_1, 'd');

    if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

    res = lstat(fpath, stbuf);
    
    if (res == -1)
        return -errno;

    return 0;
}

// Akses folder
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi){
    char fpath[1000];

    // debug readdir
    // printf("readdir: %s\n", path);
    char *flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL)
        cipher(flag_encrypt_1, 'd');

    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }else{
        sprintf(fpath, "%s%s",dirpath, path);
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

        // encrypt
        if(flag_encrypt_1 != NULL){
            cipher(de->d_name, 'e');
        }

        res = (filler(buf, de->d_name, &st, 0));
        if(res!=0)
            break;
    }

    closedir(dp);

    return 0;
}  

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi){
    char fpath[1000];

    // debug read
    // printf("read: %s\n", path);
    char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }

    if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

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

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];

	char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }

	if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

	// logging(1, "MKDIR", fpath, NULL);

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
	char fpath[1000];

	char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }

	if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

	// logging(0,"RMDIR",fpath,NULL);

	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	char src[1000], dst[1000];
	char temp[1000];

	printf("from: %s\n", from);
	strcpy(temp, from);
	cipher(temp, 'e');
	printf("to: %s\n", to);
    
    // if(strcmp(from, "/") == 0){
    //     from = dirpath;
    //     sprintf(src, "%s", from);
    // }else{
    //     char tempa[1000];     
    //     strcpy(tempa, from);

    //     if(strncmp(from, "/encv1_", 7) == 0) {  
    //         cipher(tempa, 'd');
    //     }

    //     sprintf(src, "%s%s", dirpath, tempa);
    // }

    // if(strcmp(to, "/") == 0){
    //     to = dirpath;
    //     sprintf(dst, "%s", to);
    // }else{
    //     char tempb[1000];  
    //     strcpy(tempb, to);

    //     if(strncmp(to, "/encv1_", 7) == 0) {
    //         cipher(tempb, 'd'); 
    //     }

    //     sprintf(dst, "%s%s", dirpath, tempb);
    // }
    
	printf("src: %s\n", src);
    printf("dst: %s\n", dst);
	int res;

    res = rename(src, dst);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
	char fpath[1000];

	char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }

	if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size,
					 off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];

	char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }

	if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

	(void)fi;

	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);

	return res;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	(void)fi;

	int res;
	char fpath[1000];

	char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }

	if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

	res = creat(fpath, mode);
	if (res == -1)
		return -errno;

	close(res);

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	char fpath[1000];

	char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }
    
	if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;
	char fpath[1000];

	char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }
    

	if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

	res = access(fpath, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];

	char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }

	if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev) {
	int res;
	char fpath[1000];

	char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }

	if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

	// logging(1, "MKNODE", fpath, NULL);

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path){
    int res;
    char fpath[1000];

    char* flag_encrypt_1 = strstr(path, encv1);
    
    if(flag_encrypt_1 != NULL){
        cipher(flag_encrypt_1, 'd');
    }

	if(strcmp(path, "/") == 0){
		path = dirpath;
		sprintf(fpath, "%s", path);
	} else {
		sprintf(fpath, "%s%s", dirpath, path);
	}

    // logging(0, "UNLINK", fpath, NULL);
    res = unlink(fpath);
    if(res == -1){
        return -errno;
    }

    return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.mkdir = xmp_mkdir,
	.rmdir = xmp_rmdir,
	.rename = xmp_rename,
	.truncate = xmp_truncate,
	.write = xmp_write,
	.create = xmp_create,
	.utimens = xmp_utimens,
	.access = xmp_access,
	.open = xmp_open,
	.mknod = xmp_mknod,
	.unlink = xmp_unlink,
};

  
int main(int  argc, char *argv[]){
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}