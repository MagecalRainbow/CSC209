#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void read_image(int num_rows, int num_cols, 
                int arr[num_rows][num_cols], FILE *fp);
                
void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]);

// Remember to include the function prototype for count_cells
int count_cells(int num_rows, int num_cols, int arr[num_rows][num_cols]);

int main(int argc, char **argv) {
    // Print a message to stderr and exit with an argument of 1 if there are
    // not the right number of parameters, or the second argument is not -p

    int num_rows;
    int num_cols;
    int cell_count;
    FILE *fp;

    if (!(argc == 2 || argc == 3)) {
        printf("Usage: count_cells <imagefile.txt> [-p]\n");

        return 1;
    } else {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            printf("ERROR: the file format inappropriate or not found.\n");

            return 1;
        }

        fscanf(fp, "%d", &num_rows);
        fscanf(fp, "%d", &num_cols);
        
        int arr[num_rows][num_cols];
        
        read_image(num_rows, num_cols, arr, fp);

        cell_count = count_cells(num_rows, num_cols, arr); 

        if (argc == 3) {
            if (strcmp(argv[2], "-p") == 0) {
                print_image(num_rows, num_cols, arr);
                 
                printf("Number of Cells is %d\n", cell_count);

                return 0;
            } else {
                printf("Usage: count_cells <imagefile.txt> [-p]\n");

                return 1;
            }
        } else { 
            printf("Number of Cells is %d\n", cell_count);

            return 0;
        }
    }
}

