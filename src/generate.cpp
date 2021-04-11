#include <fstream>
#include <iostream>
#include <random>
#include <string>

using namespace std;

int main(int argc, char** argv) {
  // Read arguments
  int n = 2048;
  string inputPath = "data/input.txt";
  string outputPath = "data/output.txt";
  if (argc == 1) {
    // nothing
  } else if (argc == 4) {
    n = atoi(argv[1]);
    inputPath = argv[2];
    outputPath = argv[3];
  } else {
    cout << "./generate n inputPath outputPath" << endl;
    exit(-1);
  }
  // Print arguments
  cout << endl << "======== Matrix Multiplication ========" << endl;
  cout << "n : " << n << endl;
  cout << "inputPath : " << inputPath << endl;
  cout << "outputPath : " << outputPath << endl;
  cout << "=======================================" << endl;
  // Allocate memory
  int** matrixA = new int*[n];
  int** matrixB = new int*[n];
  int** matrixC = new int*[n];
  for (int i = 0; i < n; i++) matrixA[i] = new int[n];
  for (int i = 0; i < n; i++) matrixB[i] = new int[n];
  for (int i = 0; i < n; i++) matrixC[i] = new int[n];
  // Generate problem
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) matrixA[i][j] = rand() % 3 - 1;
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) matrixB[i][j] = rand() % 3 - 1;
  // Solve matrix
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) matrixC[i][j] = 0;
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      for (int k = 0; k < n; k++)
        matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
  // Write input file
  ofstream inputFile(inputPath.data());
  if (inputFile.is_open()) {
    inputFile << n << "\n";
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        inputFile << matrixA[i][j] << " ";
      }
      inputFile << "\n";
    }
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        inputFile << matrixB[i][j] << " ";
      }
      inputFile << "\n";
    }
    inputFile.close();
  }
  // Write output file
  ofstream outputFile(outputPath.data());
  if (outputFile.is_open()) {
    outputFile << n << endl;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        outputFile << matrixC[i][j] << " ";
      }
      outputFile << "\n";
    }
    outputFile.close();
  }
  // Free memory
  for (int i = 0; i < n; i++) delete[] matrixA[i];
  for (int i = 0; i < n; i++) delete[] matrixB[i];
  for (int i = 0; i < n; i++) delete[] matrixC[i];
  delete[] matrixA;
  delete[] matrixB;
  delete[] matrixC;
  return 0;
}