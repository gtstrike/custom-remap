#include <iostream>
#include <iomanip>
using namespace std;

// Fungsi untuk menampilkan matriks
void displayMatrix(float matrix[3][4]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            cout << setw(10) << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

// Fungsi untuk melakukan eliminasi Gauss
void gaussElimination(float matrix[3][4]) {
    float temp;
    
    // Langkah 1: Eliminasi pada baris kedua dan ketiga dengan menggunakan pivot baris pertama
    for (int i = 0; i < 2; i++) {
        if (matrix[i][i] == 0) {
            cout << "Pivot 0 ditemukan di baris " << i + 1 << ", pertukarkan baris!" << endl;
            return;
        }
        
        for (int j = i + 1; j < 3; j++) {
            temp = matrix[j][i] / matrix[i][i];
            for (int k = 0; k < 4; k++) {
                matrix[j][k] -= temp * matrix[i][k];
            }
        }

        // Menampilkan langkah matriks setelah eliminasi baris
        cout << "Langkah " << i + 1 << " (Eliminasi pada baris " << i + 2 << " dan " << i + 3 << "):" << endl;
        displayMatrix(matrix);
    }

    // Langkah 2: Substitusi balik
    float x, y, z;
    z = matrix[2][3] / matrix[2][2];
    y = (matrix[1][3] - matrix[1][2] * z) / matrix[1][1];
    x = (matrix[0][3] - matrix[0][1] * y - matrix[0][2] * z) / matrix[0][0];

    cout << "Solusi: x = " << x << ", y = " << y << ", z = " << z << endl;
}

int main() {
    float matrix[3][4];
    
    // Input matriks augmented dari sistem persamaan linier
    cout << "Masukkan koefisien dan hasil dari 3 persamaan linier (3 variabel):" << endl;
    cout << "Contoh format: x + y + z = 4, 8x - y - 2z = 0, 4x + 2y + 3z = 17" << endl;

    for (int i = 0; i < 3; i++) {
        cout << "Masukkan persamaan " << i + 1 << " (koefisien x, y, z dan hasil): ";
        for (int j = 0; j < 4; j++) {
            cin >> matrix[i][j];
        }
    }

    // Menampilkan matriks augmented sebelum eliminasi
    cout << "\nMatriks Augmented Awal:" << endl;
    displayMatrix(matrix);

    // Menjalankan eliminasi Gauss
    gaussElimination(matrix);

    return 0;
}
