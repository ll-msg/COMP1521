// COMP1521 21T2 ... final exam, question 7

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const mode_t NEW_DIR_MODE = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

int is_directory(char *dir_path);
void cp_file (char *path_from, char *path_to);

void
cp_directory (char *dir_from, char *dir_to)
{
	// open the source directory
	DIR *from_dir = opendir(dir_from);
	// create a target directory
	mkdir(dir_to, NEW_DIR_MODE);
	// make a struct to the source directory
	struct dirent *dir_ptr;

	// loop through the original directory
	while ((dir_ptr = readdir(from_dir)) != NULL) {
		if ((strcmp(dir_ptr->d_name, ".") != 0) && (strcmp(dir_ptr->d_name, "..") != 0)) {
			int size = strlen(dir_from) + strlen(dir_ptr->d_name) + strlen("/") + 1;
			char *folder_from = malloc(size * sizeof(char));
			char *folder_to = malloc(size * sizeof(char));
			// get the path
			snprintf(folder_from, size, "%s/%s", dir_from, dir_ptr->d_name);
			snprintf(folder_to, size, "%s/%s", dir_to, dir_ptr->d_name);
			// check if the path is a directory
			int result = is_directory(folder_from);
			// not a directory but a file
			if (result != 1) {
				cp_file(folder_from, folder_to);
				// if is a directory
			} else {
				cp_directory(folder_from, folder_to);
			}
			free(folder_from);
			free(folder_to);
		}
	}
	closedir(from_dir);
}

// copy from file into to file
void
cp_file (char *path_from, char *path_to) {
	FILE *from_file = 0;
	FILE *to_file = 0;

	// open the from file
	if ((from_file = fopen(path_from, "r+")) == NULL) {
		return;
	};
	// open the to file
	if ((to_file = fopen(path_to, "w+")) == NULL) {
		return;
	};
	// copy
	int charac;
	while ((charac = fgetc(from_file)) != EOF) {
		fputc(charac, to_file);
	}
	fclose(from_file);
	fclose(to_file);
}

int
is_directory(char *dir_path) {
	DIR *direc;
	int result = 0;
	if ((direc = opendir(dir_path)) != NULL) {
		result = 1;
	}
	return result;
}
