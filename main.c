#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

typedef struct {
    char signature[2];
    int size;
    int reserved;
    int offset;
} BMPHeader;

typedef struct {
    int header_size;
    int width;
    int height;
    int planes;
    int bpp;
    int compression;
    int image_size;
    int x_pixels_per_meter;
    int y_pixels_per_meter;
    int colors_used;
    int colors_important;
} InfoHeader;

void processFile(const char *filename, struct stat *file_info) {
    char statistic_info[256];
    int fd_output;

    char *file_type = "fisier obisnuit";

    if (S_ISLNK(file_info->st_mode)) {
        file_type = "legatura simbolica";

        char targetPath[256];
        ssize_t bytesRead = readlink(filename, targetPath, sizeof(targetPath) - 1);
        if (bytesRead != -1) {
            targetPath[bytesRead] = '\0';
            sprintf(statistic_info, "nume legatura: %s\ndimensiune legatura: %lld\ndimensiune fisier target: %lld\n",
                    filename, (long long)file_info->st_size, (long long)file_info->st_size);

	    // DREPTURI DE ACCES

  // drepturi pentru user
  sprintf(statistic_info, "Drepturi de acces user:");
  write(fd_output, statistic_info, strlen(statistic_info));

  sprintf(statistic_info, (file_info->st_mode & S_IRUSR) ? "R" : "-");
  write(fd_output, statistic_info, strlen(statistic_info));

  sprintf(statistic_info, (file_info->st_mode & S_IWUSR) ? "W" : "-");
  write(fd_output, statistic_info, strlen(statistic_info));

  sprintf(statistic_info, (file_info->st_mode & S_IXUSR) ? "X\n" : "-\n");
  write(fd_output, statistic_info, strlen(statistic_info));

  
  // drepturi pentru grup
  sprintf(statistic_info, "Drepturi de acces grup: ");
  write(fd_output, statistic_info, strlen(statistic_info));

  sprintf(statistic_info, (file_info->st_mode & S_IRGRP) ? "R" : "-");
  write(fd_output, statistic_info, strlen(statistic_info));

  sprintf(statistic_info, (file_info->st_mode & S_IWGRP) ? "W" : "-");
  write(fd_output, statistic_info, strlen(statistic_info));

  sprintf(statistic_info, (file_info->st_mode & S_IXGRP) ? "X\n" : "-\n");
  write(fd_output, statistic_info, strlen(statistic_info));

  

  // drepturi pentru altii
  sprintf(statistic_info, "Drepturi de acces altii: ");
  write(fd_output, statistic_info, strlen(statistic_info));

  sprintf(statistic_info, (file_info->st_mode & S_IROTH) ? "R" : "-");
  write(fd_output, statistic_info, strlen(statistic_info));

  sprintf(statistic_info, (file_info->st_mode & S_IWOTH) ? "W" : "-");
  write(fd_output, statistic_info, strlen(statistic_info));

  sprintf(statistic_info, (file_info->st_mode & S_IXOTH) ? "X\n" : "-\n");
  write(fd_output, statistic_info, strlen(statistic_info));
  
        } else {
            perror("readlink");
            exit(EXIT_FAILURE);
        }
    } else if (S_ISDIR(file_info->st_mode)) {
        file_type = "director";
        sprintf(statistic_info, "nume director: %s\nidentificatorul utilizatorului: %d\n",
                filename, file_info->st_uid);
  
        // Adaugam informatii despre fisierele din director
        DIR *dir;
        struct dirent *entry;

        dir = opendir(filename);
        if (dir == NULL) {
            perror("opendir");
            exit(EXIT_FAILURE);
        }

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char path[256];
                snprintf(path, sizeof(path), "%s/%s", filename, entry->d_name);

                struct stat entry_info;
                if (lstat(path, &entry_info) == -1) {
                    perror("Eroare la primirea informatiilor");
                    exit(EXIT_FAILURE);
                }

                processFile(path, &entry_info);

		
            }
        }

        closedir(dir);
    } else if (S_ISREG(file_info->st_mode)) {
        if (strstr(filename, ".bmp") != NULL) {

	  int input_fd = open(filename, O_RDONLY);
	  
	    if(input_fd == -1){
	      perror("EROARE la deschidere fisier BMP");
	      exit(-1);
	    }

	    BMPHeader bmp_header;
 
	    if(read(input_fd, &bmp_header, sizeof(BMPHeader)) == -1){
	      perror("EROARE citire BMP header");
	      close(input_fd);
	      exit(-1);
	    }

	    InfoHeader info_header;
  
	    if(read(input_fd, &info_header, sizeof(InfoHeader)) == -1){
	      perror("EROARE citire Info header");
	      close(input_fd);
	      exit(-1);
	    }

	    close(input_fd);
	  
            sprintf(statistic_info, "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %lld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %s\n"
                                    "tipul fisierului: %s\ncontorul de legaturi: %d\n",
                    filename, info_header.height, info_header.width, (long long)file_info->st_size, file_info->st_uid, ctime(&(file_info->st_mtime)), file_type,
                    (int)file_info->st_nlink);//fisier cu .bmp
        } else {
            sprintf(statistic_info, "nume fisier: %s\ndimensiune: %lld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %s\n"
                                    "tipul fisierului: %s\ncontorul de legaturi: %d\n",
                    filename, (long long)file_info->st_size, file_info->st_uid, ctime(&(file_info->st_mtime)), file_type,
                    (int)file_info->st_nlink);//fisier fara .bmp
        }
    } else {
        return; // Pentru orice alt caz, nu scriem nimic în fisier
    }

    fd_output = open("statistica.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd_output == -1) {
        perror("EROARE la deschidere fisier output");
        exit(-1);
    }

    if (write(fd_output, statistic_info, strlen(statistic_info)) != strlen(statistic_info)) {
        perror("EROARE la scriere in fisier output");
        close(fd_output);
        exit(-1);
    }

    // DREPTURI DE ACCES

    // drepturi pentru user
    sprintf(statistic_info, "Drepturi de acces user:");
    write(fd_output, statistic_info, strlen(statistic_info));

    sprintf(statistic_info, (file_info->st_mode & S_IRUSR) ? "R" : "-");
    write(fd_output, statistic_info, strlen(statistic_info));

    sprintf(statistic_info, (file_info->st_mode & S_IWUSR) ? "W" : "-");
    write(fd_output, statistic_info, strlen(statistic_info));

    sprintf(statistic_info, (file_info->st_mode & S_IXUSR) ? "X\n" : "-\n");
    write(fd_output, statistic_info, strlen(statistic_info));

    // drepturi pentru grup
    sprintf(statistic_info, "Drepturi de acces grup: ");
    write(fd_output, statistic_info, strlen(statistic_info));

    sprintf(statistic_info, (file_info->st_mode & S_IRGRP) ? "R" : "-");
    write(fd_output, statistic_info, strlen(statistic_info));

    sprintf(statistic_info, (file_info->st_mode & S_IWGRP) ? "W" : "-");
    write(fd_output, statistic_info, strlen(statistic_info));

    sprintf(statistic_info, (file_info->st_mode & S_IXGRP) ? "X\n" : "-\n");
    write(fd_output, statistic_info, strlen(statistic_info));

    // drepturi pentru altii
    sprintf(statistic_info, "Drepturi de acces altii: ");
    write(fd_output, statistic_info, strlen(statistic_info));

    sprintf(statistic_info, (file_info->st_mode & S_IROTH) ? "R" : "-");
    write(fd_output, statistic_info, strlen(statistic_info));

    sprintf(statistic_info, (file_info->st_mode & S_IWOTH) ? "W" : "-");
    write(fd_output, statistic_info, strlen(statistic_info));

    sprintf(statistic_info, (file_info->st_mode & S_IXOTH) ? "X\n" : "-\n");
    write(fd_output, statistic_info, strlen(statistic_info));

    close(fd_output);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("Usage ./program <fisier_intrare>");
        exit(-1);
    }

    int fd_output = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd_output == -1) {
        perror("EROARE la deschidere fisier output");
        exit(-1);
    }
    
    struct stat file_info;

    if (lstat(argv[1], &file_info) == -1) {
        perror("Eroare la primirea informatiilor");
        exit(EXIT_FAILURE);
    }

    processFile(argv[1], &file_info);

    return 0;
}
