#include <stdio.h> 

void helper(int row, int column, int num_rows, int num_cols, int arry[num_rows][num_cols]) {
    if (row == 0) {
        if (column == 0) {
            if ((arry)[row + 1][column] == 255) {
                (arry)[row + 1][column] = 1;
                helper(row + 1, column, num_rows, num_cols, arry);
            }
            if ((arry)[row][column + 1] == 255) {
                (arry)[row][column + 1] = 1;
                helper(row, column + 1, num_rows, num_cols, arry);
            }
        } else if (column == num_cols - 1){
            if ((arry)[row - 1][column] == 255) {
                (arry)[row - 1][column] = 1;
                helper(row - 1, column, num_rows, num_cols, arry);
            }
            if ((arry)[row][column + 1] == 255) {
                (arry)[row][column + 1] = 1;
                helper(row, column + 1, num_rows, num_cols, arry);
            }
        } else {
            if ((arry)[row + 1][column] == 255) {
                (arry)[row + 1][column] = 1;
                helper(row + 1, column, num_rows, num_cols, arry);
            }
            if ((arry)[row][column + 1] == 255) {
                (arry)[row][column + 1] = 1;
                helper(row, column + 1, num_rows, num_cols, arry);
            }
            if ((arry)[row - 1][column] == 255) {
                (arry)[row - 1][column] = 1;
                helper(row - 1, column, num_rows, num_cols, arry);
            }
        }
    } else if (row == num_rows - 1) {
        if (column == 0) {
            if ((arry)[row][column + 1] == 255) {
                (arry)[row][column + 1] = 1;
                helper(row, column + 1, num_rows, num_cols, arry);
            }
            if ((arry)[row - 1][column] == 255) {
                (arry)[row - 1][column] = 1;
                helper(row - 1, column, num_rows, num_cols, arry);
            }
        } else if (column == num_cols - 1) {
            if ((arry)[row - 1][column] == 255) {
                (arry)[row - 1][column] = 1;
                helper(row - 1, column, num_rows, num_cols, arry);
            }
            if ((arry)[row][column - 1] == 255) {
                (arry)[row][column - 1] = 1;
                helper(row, column - 1, num_rows, num_cols, arry);
            }
        } else {
            if ((arry)[row][column + 1] == 255) {
                (arry)[row][column + 1] = 1;
                helper(row, column + 1, num_rows, num_cols, arry);
            }
            if ((arry)[row - 1][column] == 255) {
                (arry)[row - 1][column] = 1;
                helper(row - 1, column, num_rows, num_cols, arry);
            }
            if ((arry)[row][column - 1] == 255) {
                (arry)[row][column - 1] = 1;
                helper(row, column - 1, num_rows, num_cols, arry);
            }
        }
    } else if (column == 0) {
        if ((arry)[row + 1][column] == 255) {
            (arry)[row + 1][column] = 1;
            helper(row + 1, column, num_rows, num_cols, arry);
        }
        if ((arry)[row][column + 1] == 255) {
            (arry)[row][column + 1] = 1;
            helper(row, column + 1, num_rows, num_cols, arry);
        }
        if ((arry)[row - 1][column] == 255) {
            (arry)[row - 1][column] = 1;
            helper(row - 1, column, num_rows, num_cols, arry);
        }
    } else if (column == num_cols - 1) {
        if ((arry)[row + 1][column] == 255) {
            (arry)[row + 1][column] = 1;
            helper(row + 1, column, num_rows, num_cols, arry);
        }
        if ((arry)[row - 1][column] == 255) {
            (arry)[row - 1][column] = 1;
            helper(row - 1, column, num_rows, num_cols, arry);
        }
        if ((arry)[row][column - 1] == 255) {
            (arry)[row][column - 1] = 1;
            helper(row, column - 1, num_rows, num_cols, arry);
        }
    }  
    else {
        if ((arry)[row + 1][column] == 255) {
            (arry)[row + 1][column] = 1;
            helper(row + 1, column, num_rows, num_cols, arry);
        }
        if ((arry)[row][column + 1] == 255) {
            (arry)[row][column + 1] = 1;
            helper(row, column + 1, num_rows, num_cols, arry);
        }
        if ((arry)[row - 1][column] == 255) {
            (arry)[row - 1][column] = 1;
            helper(row - 1, column, num_rows, num_cols, arry);
        }
        if ((arry)[row][column - 1] == 255) {
            (arry)[row][column - 1] = 1;
            helper(row, column - 1, num_rows, num_cols, arry);
        }
    }    
}

/* Reads the image from the open file fp into the two-dimensional array arr 
 * num_rows and num_cols specify the dimensions of arr
 */
void read_image(int num_rows, int num_cols, 
                int arr[num_rows][num_cols], FILE *fp) {

    int x;
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            fscanf(fp, "%d", &x);
            arr[i][j] = x;
        }
    } 

}

/* Print to standard output the contents of the array arr */
void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]) {
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols - 1; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("%d\n", arr[i][num_cols - 1]);
    }
}

/* TODO: Write the count_cells function */
int count_cells(int num_rows, int num_cols, int arr[num_rows][num_cols]) {
    int total = 0;

    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            if (arr[i][j] == 255) {
                arr[i][j] = 1;
                if (i == 0) {
                    if (j == 0) {
                        if (arr[i][j + 1] != 1 && arr[i + 1][j] != 1) {
                            total += 1;
                        }
                    } else if (j == num_cols - 1) {
                        if (arr[i - 1][j] != 1 && arr[i][j + 1] != 1) {
                            total += 1;
                        }
                    } else{
                        if (arr[i + 1][j] != 1 && arr[i][j + 1] != 1 && arr[i][j - 1] != 1) {
                            total += 1;
                        }
                    }
                } else if (i == num_rows - 1) {
                    if (j == 0) {
                        if (arr[i - 1][j] != 1 && arr[i][j + 1] != 1) {
                            total += 1;
                        } 
                    } else if (j == num_cols - 1) {
                        if (arr[i - 1][j] != 1 && arr[i][j - 1] != 1) {
                            total += 1;
                        }
                    } else {
                        if (arr[i - 1][j] != 1 && arr[i][j - 1] != 1 && arr[i + 1][j] != 1) {
                            total += 1;
                        } 
                    }
                } else if (j == 0) {
                    if (arr[i + 1][j] != 1 && arr[i - 1][j] != 1 && arr[i][j + 1] != 1) {
                        total += 1;
                    }
                } else if (j == num_cols - 1) {
                    if (arr[i - 1][j] != 1 && arr[i][j + 1] != 1 && arr[i][j - 1] != 1) {
                        total += 1;
                    }
                } else { 
                    if (arr[i - 1][j] != 1 && arr[i][j - 1] != 1 && arr[i + 1][j] != 1 && arr[i][j + 1] != 1) {
                        total += 1;
                    }
                }
                helper(i, j, num_rows, num_cols, arr);
            }
        }
    }

    return total;
}

