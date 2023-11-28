#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
 
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

typedef struct {
    unsigned char b;
    unsigned char g;
    unsigned char r;
} Pixel;


void processFile(const char *filename, struct stat *file_info, const char *output_dir) {

  char stat_output_path[256];
  snprintf(stat_output_path, sizeof(stat_output_path), "%s/%s_statistica.txt", output_dir, filename);

  pid_t child_pid = fork();

  if(child_pid == -1){
    perror("EROARE la fork");
    exit(EXIT_FAILURE);
  }

  if(child_pid == 0){
    int fd_output = open(stat_output_path, O_WRONLY | O_CREAT| O_TRUNC, S_IRUSR | S_IWUSR);
    if(fd_output == -1){
      perror("EROARE la deschidere fisier output");
      exit(EXIT_FAILURE);
    }
    
    char statistic_info[4096];
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
	    
                processFile(entry->d_name, &entry_info, output_dir);
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
 
	    if(read(input_fd, &bmp_header, sizeof(BMPHeader)-2) == -1){
	      perror("EROARE citire BMP header");
	      close(input_fd);
	      exit(-1);
	    }
 
	    InfoHeader info_header;
 
	    if(read(input_fd, &info_header, sizeof(InfoHeader)-4) == -1){
	      perror("EROARE citire Info header");
	      close(input_fd);
	      exit(-1);
	    }
 
	    close(input_fd);
 
            sprintf(statistic_info, "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %lld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %s\n"
                                    "tipul fisierului: %s\ncontorul de legaturi: %d\n",
                    filename, info_header.height, info_header.width, (long long)file_info->st_size, file_info->st_uid, ctime(&(file_info->st_mtime)), file_type,
                    (int)file_info->st_nlink);//fisier cu .bmp
        

	}
	else {
            sprintf(statistic_info, "nume fisier: %s\ndimensiune: %lld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %s\n"
                                    "tipul fisierului: %s\ncontorul de legaturi: %d\n",
                    filename, (long long)file_info->st_size, file_info->st_uid, ctime(&(file_info->st_mtime)), file_type,
                    (int)file_info->st_nlink);//fisier fara .bmp
        }
    } else {
        return; // Pentru orice alt caz, nu scriem nimic in fisier
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
  } else { int status;
    waitpid(child_pid, &status, 0);

    if(WIFEXITED(status)) {
      printf("s-a incheiat procesul cu pid-ul %d si codul %d\n", child_pid, WEXITSTATUS(status));
    } else {
      printf("Procesul cu pid-ul %d s-a incheiat anormal\n", child_pid);
    }
  }
}
 
void convertToGray(const char *input_path, const char *output_path) {
    int input_fd = open(input_path, O_RDONLY);

    if (input_fd == -1) {
        perror("EROARE la deschidere fisier");
        exit(-1);
    }

    BMPHeader bmp_header;
    InfoHeader info_header;

    if (read(input_fd, &bmp_header, sizeof(BMPHeader) - 2) == -1 || read(input_fd, &info_header, sizeof(InfoHeader) - 4) == -1) {
        perror("EROARE citire header BMP");
        close(input_fd);
	// close(output_fd);
        exit(-1);
    }

    Pixel pixel;

    int output_fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if(output_fd == -1){
      perror("EROARE la deschidere fisier output");
      close(output_fd);
      exit(-1);
    }

    if(write(output_fd, &bmp_header, sizeof(BMPHeader) - 2) == -1){
  perror("EROARE scriere header BMP");
  close(input_fd);
  close(output_fd);
  exit(-1);
}

if(write(output_fd, &info_header, sizeof(InfoHeader) - 4) == -1){
  perror("EROARE scriere info header BMP");
  close(input_fd);
  close(output_fd);
  exit(-1);
 }
    
lseek(input_fd, bmp_header.offset, SEEK_SET);

    for (int y = 0; y < info_header.height; ++y) {
        for (int x = 0; x < info_header.width; ++x) {
            if (read(input_fd, &pixel, sizeof(Pixel)) == -1) {
                perror("EROARE citire pixel");
                close(input_fd);
                close(output_fd);
                exit(-1);
            }

            unsigned char gray = (unsigned char)(0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b);

            
            pixel.r = gray;
            pixel.g = gray;
            pixel.b = gray;

            if (write(output_fd, &pixel, sizeof(Pixel)) == -1) {
                perror("EROARE scriere pixel gri");
                close(input_fd);
                close(output_fd);
                exit(-1);
            }
        }
    }

    close(input_fd);
    close(output_fd);
}

int main(int argc, char *argv[]) {
    if (argc > 3) {
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
 
    processFile(argv[1], &file_info, argv[2]);

    int status;
    pid_t childPid;
    while ((childPid = wait(&status)) != -1) {
        printf("S-a incheiat procesul cu pid-ul %d si codul %d\n", childPid, status);
    }
    
    // convertToGray(argv[1], "output_gray.bmp");
 
 FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("EROARE la deschidere fisier input");
        exit(EXIT_FAILURE);
    }
   
    int counter = 0;

    char line[256];
    while (fgets(line, sizeof(line), input_file) != NULL) {
        
        if (strstr(line, argv[2]) && strstr(line, argv[2]) < strstr(line, ": output_gray.bmp")) {
           
            counter++;
        }
    }

   fclose(input_file);

    
    printf("Numarul de propozitii corecte este: %d\n", counter);

 

 
    return 0;
}
