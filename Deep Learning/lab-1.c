// WAP to implement matrix addition and multiplication
// then do matrix addition and multiplication on two random matrices
// and plot the mean of the resultant matrices

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_TYPE float

typedef struct Matrix_t
{
  int rows;
  int cols;
  MATRIX_TYPE *data;
} Matrix_t;

// Object management functions
Matrix_t *newMatrix(int rows, int cols)
{
  Matrix_t *matrix = (Matrix_t *)malloc(sizeof(*matrix));
  if (!matrix)
  {
    fprintf(stderr, "Error: Memory allocation failed for matrix struct.\n");
    return NULL;
  }

  matrix->rows = rows;
  matrix->cols = cols;
  matrix->data = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * rows * cols);
  if (!matrix->data)
  {
    free(matrix);
    return NULL;
  }
  return matrix;
}

void deleteMatrix(Matrix_t *matrix)
{
  if (matrix)
  {
    free(matrix->data);
    free(matrix);
  }
}

// Matrix operations
Matrix_t *addMatrices(const Matrix_t *A, const Matrix_t *B)
{
  if (A->rows != B->rows || A->cols != B->cols)
  {
    fprintf(stderr, "Error: Matrices must have the same dimensions for addition.\n");
    return NULL;
  }

  Matrix_t *result = newMatrix(A->rows, A->cols);
  if (!result)
  {
    return NULL;
  }

  for (int i = 0; i < A->rows; i++)
  {
    for (int j = 0; j < A->cols; j++)
    {
      result->data[i * A->cols + j] = A->data[i * A->cols + j] + B->data[i * B->cols + j];
    }
  }
  return result;
}

Matrix_t *multiplyMatrices(const Matrix_t *A, const Matrix_t *B)
{
  if (A->cols != B->rows)
  {
    fprintf(stderr, "Error: Number of columns in A must equal number of rows in B for multiplication.\n");
    return NULL;
  }

  Matrix_t *result = newMatrix(A->rows, B->cols);
  if (!result)
  {
    return NULL;
  }

  for (int i = 0; i < A->rows; i++)
  {
    for (int j = 0; j < B->cols; j++)
    {
      result->data[i * B->cols + j] = 0;
      for (int k = 0; k < A->cols; k++)
      {
        result->data[i * B->cols + j] += A->data[i * A->cols + k] * B->data[k * B->cols + j];
      }
    }
  }
  return result;
}

double sumMatrix(const Matrix_t *matrix)
{
  double sum = 0.0;
  for (int i = 0; i < matrix->rows * matrix->cols; i++)
  {
    sum += matrix->data[i];
  }
  return sum;
}

double meanMatrix(const Matrix_t *matrix)
{
  double sum = sumMatrix(matrix);
  return sum / (matrix->rows * matrix->cols);
}

// Utility function to fill a matrix with random values
void makeMatrixRandom(Matrix_t *matrix)
{
  if (!matrix || !matrix->data)
  {
    fprintf(stderr, "Error: Invalid matrix pointer.\n");
    return;
  }

  for (int i = 0; i < matrix->rows * matrix->cols; i++)
  {
    matrix->data[i] = (MATRIX_TYPE)rand() / RAND_MAX; // Random float between 0 and 1
    matrix->data[i] = matrix->data[i] * 2.0f - 1.0f;  // Scale to range [-1, 1]
  }
}

void plotResults(double *addResults, double *multResults, int numIterations, const char *outputFile)
{
  if (!addResults || !multResults || numIterations <= 0)
  {
    fprintf(stderr, "Error: Invalid input for plotting results.\n");
    return;
  }

  FILE *output = fopen(outputFile, "w");
  if (!output)
    output = stdout;

  double maxAdd = addResults[0];
  double maxMult = multResults[0];
  double minAdd = addResults[0];
  double minMult = multResults[0];

  // Find max and min values for scaling the plot
  for (int i = 0; i < numIterations; i++)
  {
    if (addResults[i] > maxAdd)
      maxAdd = addResults[i];
    if (multResults[i] > maxMult)
      maxMult = multResults[i];

    if (addResults[i] < minAdd)
      minAdd = addResults[i];
    if (multResults[i] < minMult)
      minMult = multResults[i];
  }

  // Simple text-based plot
  fprintf(output, "Addition Results:\n");
  for (int i = 0; i < numIterations; i++)
  {
    int numStars = (int)((addResults[i] - minAdd) / (maxAdd - minAdd) * 50);
    fprintf(output, "%2d: %6.3f ", i + 1, addResults[i]);
    for (int j = 0; j < numStars; j++)
    {
      fprintf(output, "*");
    }
    fprintf(output, "\n");
  }

  fprintf(output, "\nMultiplication Results:\n");
  for (int i = 0; i < numIterations; i++)
  {
    int numStars = (int)((multResults[i] - minMult) / (maxMult - minMult) * 50);
    fprintf(output, "%2d: %6.3f ", i + 1, multResults[i]);
    for (int j = 0; j < numStars; j++)
    {
      fprintf(output, "*");
    }
    fprintf(output, "\n");
  }
  fclose(output);
}

int main()
{
  const char *outputFile = "results.txt";
  srand((unsigned int)time(NULL));
  int numIterations = 10;
  double addResults[20];
  double MultResults[20];

  for (int i = 0; i < 1000; ++i)
  {
    Matrix_t *A = newMatrix(2, 3);
    Matrix_t *B = newMatrix(2, 3);
    if (!A || !B)
    {
      fprintf(stderr, "Error: Failed to create matrices.\n");
      deleteMatrix(A);
      deleteMatrix(B);
      return EXIT_FAILURE;
    }
    for (int t = 0; t < numIterations; t++)
    {
      makeMatrixRandom(A);
      makeMatrixRandom(B);
      Matrix_t *C = addMatrices(A, B);
      if (!C)
      {
        fprintf(stderr, "Error: Failed to add matrices.\n");
        deleteMatrix(A);
        deleteMatrix(B);
        return EXIT_FAILURE;
      }

      addResults[t] = meanMatrix(C);
      deleteMatrix(C);
    }

    deleteMatrix(A);
    deleteMatrix(B);
    A = newMatrix(2, 3);
    B = newMatrix(3, 2);
    for (int t = 0; t < numIterations; t++)
    {
      makeMatrixRandom(A);
      makeMatrixRandom(B);
      Matrix_t *D = multiplyMatrices(A, B);
      if (!D)
      {
        fprintf(stderr, "Error: Failed to multiply matrices.\n");
        deleteMatrix(A);
        deleteMatrix(B);
        return EXIT_FAILURE;
      }

      MultResults[t] = sumMatrix(D);
      deleteMatrix(D);
    }

    _sleep(250);
    plotResults(addResults, MultResults, numIterations, outputFile);
    // plotResults(addResults, MultResults, numIterations, NULL);

    deleteMatrix(A);
    deleteMatrix(B);
  }
  return 0;
}