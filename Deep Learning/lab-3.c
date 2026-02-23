// WAP to implement a learnable Perceptron

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PERCEPTRON_D_TYPE float

typedef struct Perceptron
{
  PERCEPTRON_D_TYPE *weights;
  PERCEPTRON_D_TYPE bias;
  int numWeights;
  PERCEPTRON_D_TYPE learningRate;
} Perceptron;

Perceptron *new_Perceptron(int numWeights, PERCEPTRON_D_TYPE learningRate)
{
  Perceptron *p = malloc(sizeof(*p));
  if (!p)
  {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  p->weights = malloc(numWeights * sizeof(PERCEPTRON_D_TYPE));
  if (!p->weights)
  {
    fprintf(stderr, "Memory allocation for weights failed\n");
    free(p);
    return NULL;
  }

  for (int i = 0; i < numWeights; i++)
    p->weights[i] = ((PERCEPTRON_D_TYPE)rand() / RAND_MAX) * 2 - 1;

  p->bias = ((PERCEPTRON_D_TYPE)rand() / RAND_MAX) * 2 - 1;
  p->numWeights = numWeights;
  p->learningRate = learningRate;

  return p;
}

void delete_Perceptron(Perceptron *p)
{
  if (p)
  {
    free(p->weights);
    free(p);
  }
}

PERCEPTRON_D_TYPE activate(PERCEPTRON_D_TYPE x)
{
  return x >= 0 ? 1 : 0;
}

PERCEPTRON_D_TYPE predict(Perceptron *p,
                          PERCEPTRON_D_TYPE *inputs, int numInputs)
{
  if (!p)
  {
    fprintf(stderr, "Invalid Perceptron\n");
    return -1; // Indicate an error
  }

  if (!inputs || numInputs != p->numWeights)
  {
    fprintf(stderr, "Invalid inputs or input size mismatch\n");
    return -1; // Indicate an error
  }

  PERCEPTRON_D_TYPE sum = p->bias;
  for (int i = 0; i < p->numWeights; i++)
    sum += p->weights[i] * inputs[i];

  return activate(sum);
}

void update(Perceptron *p,
            PERCEPTRON_D_TYPE *inputs, int numInputs,
            PERCEPTRON_D_TYPE error)
{
  if (!p)
  {
    fprintf(stderr, "Invalid Perceptron\n");
    return;
  }

  if (!inputs || numInputs != p->numWeights)
  {
    fprintf(stderr, "Invalid inputs or input size mismatch\n");
    return;
  }

  PERCEPTRON_D_TYPE delta = p->learningRate * error;
  p->bias += delta;
  for (int i = 0; i < p->numWeights; i++)
    p->weights[i] += delta * inputs[i];
}

void fit(Perceptron *p,
         PERCEPTRON_D_TYPE **X, PERCEPTRON_D_TYPE *y,
         int numSamples, int numInputs, int numEpochs)
{
  if (!p)
  {
    fprintf(stderr, "Invalid Perceptron\n");
    return;
  }

  if (!X || !y || numInputs != p->numWeights)
  {
    fprintf(stderr, "Invalid inputs or input size mismatch\n");
    return;
  }

  for (int epoch = 0; epoch < numEpochs; epoch++)
  {
    PERCEPTRON_D_TYPE lossPerEpoch = 0;
    for (int i = 0; i < numSamples; i++)
    {
      PERCEPTRON_D_TYPE prediction = predict(p, X[i], numInputs);
      PERCEPTRON_D_TYPE error = y[i] - prediction;

      update(p, X[i], numInputs, error);
      lossPerEpoch += error * error;
    }
    printf("Epoch %04d | Loss: %8.4f\n", epoch + 1, lossPerEpoch);

    if (lossPerEpoch == 0)
    {
      printf("Training complete at epoch %04d\n", epoch + 1);
      break;
    }
  }
}

PERCEPTRON_D_TYPE evaluate(Perceptron *p,
                           PERCEPTRON_D_TYPE **X, PERCEPTRON_D_TYPE *y,
                           int numSamples, int numInputs)
{
  if (!p)
  {
    fprintf(stderr, "Invalid Perceptron\n");
    return -1;
  }

  if (!X || !y || numInputs != p->numWeights)
  {
    fprintf(stderr, "Invalid inputs or input size mismatch\n");
    return -1;
  }

  int correct = 0;
  for (int i = 0; i < numSamples; i++)
  {
    PERCEPTRON_D_TYPE prediction = predict(p, X[i], numInputs);
    if (prediction == y[i])
      correct++;
  }
  return (PERCEPTRON_D_TYPE)correct / numSamples;
}

int create_dataset(PERCEPTRON_D_TYPE ***X, PERCEPTRON_D_TYPE **y, int numInputs)
{
  int numSamples = 1 << numInputs; // 2^numInputs
  *X = (PERCEPTRON_D_TYPE **)malloc(numSamples * sizeof(PERCEPTRON_D_TYPE *));
  if (!*X)
  {
    fprintf(stderr, "Memory allocation for X failed\n");
    return -1;
  }

  *y = (PERCEPTRON_D_TYPE *)malloc(numSamples * sizeof(PERCEPTRON_D_TYPE));
  if (!*y)
  {
    fprintf(stderr, "Memory allocation for y failed\n");
    free(*X);
    return -1;
  }

  for (int i = 0; i < numSamples; i++)
  {
    (*X)[i] = (PERCEPTRON_D_TYPE *)malloc(numInputs * sizeof(PERCEPTRON_D_TYPE));
    if (!(*X)[i])
    {
      fprintf(stderr, "Memory allocation for X[%d] failed\n", i);
      for (int j = 0; j < i; j++)
        free((*X)[j]);
      free(*X);
      free(*y);
      return -1;
    }

    // Generate random inputs
    // for (int j = 0; j < numInputs; j++)
    // (*X)[i][j] = ((PERCEPTRON_D_TYPE)rand() / RAND_MAX) > 0.5 ? 1 : 0;

    // Generate labels based on XOR of inputs
    // PERCEPTRON_D_TYPE sum = 0;
    // for (int j = 0; j < numInputs; j++)
    // sum += (*X)[i][j];

    // (*y)[i] = sum > numInputs / 2 ? 1 : 0;
  }

  for (int i = 0; i < numSamples; i++)
  {
    for (int j = 0; j < numInputs; j++)
      (*X)[i][numInputs - 1 - j] = (i >> j) & 1;
  }

  for (int i = 0; i < numSamples; i++)
    (*y)[i] = 0;
  (*y)[numSamples - 1] = 1;

  return 0;
}

void print_dataset(PERCEPTRON_D_TYPE **X, PERCEPTRON_D_TYPE *y, int numSamples, int numInputs)
{
  printf("Dataset:\n");
  for (int i = 0; i < numSamples; i++)
  {
    printf("Input: [");
    for (int j = 0; j < numInputs; j++)
      printf("%d ", (int)X[i][j]);
    printf("] -> Label: %d\n", (int)y[i]);
  }
}

int main()
{
  srand((unsigned int)time(NULL));

  int numInputs = 15;
  int numSamples = 1 << numInputs; // 2^numInputs
  PERCEPTRON_D_TYPE **X;
  PERCEPTRON_D_TYPE *y;

  printf("Creating dataset with %d samples and %d features...\n", numSamples, numInputs);
  int res = create_dataset(&X, &y, numInputs);
  if (!X || !y || res != 0)
  {
    fprintf(stderr, "Failed to create dataset\n");
    return 1;
  }
  else
  {
    printf("Dataset created successfully\n");
    // print_dataset(X, y, numSamples, numInputs);
  }

  Perceptron *p = new_Perceptron(numInputs, 0.03);
  if (!p)
  {
    fprintf(stderr, "Failed to create Perceptron\n");
    return 1;
  }

  // Test the initial Perceptron
  printf("==================================================\n");
  printf("Initial Perceptron predictions:\n");
  PERCEPTRON_D_TYPE initialAccuracy = evaluate(p, (PERCEPTRON_D_TYPE **)X, y, numSamples, numInputs);
  printf("Initial Accuracy: %.2f%%\n", initialAccuracy * 100);

  printf("==================================================\n");
  fit(p, (PERCEPTRON_D_TYPE **)X, y, numSamples, numInputs, 1000);

  // Test the trained Perceptron
  printf("==================================================\n");
  printf("Trained Perceptron predictions:\n");
  PERCEPTRON_D_TYPE finalAccuracy = evaluate(p, (PERCEPTRON_D_TYPE **)X, y, numSamples, numInputs);
  printf("Final Accuracy: %.2f%%\n", finalAccuracy * 100);

  // Test the trained Perceptron
  // printf("==================================================\n");
  // for (int i = 0; i < numSamples; i++)
  // {
  //   PERCEPTRON_D_TYPE prediction = predict(p, X[i], numInputs);
  //   printf("Input: [%d,%d] -> Output: %f\n", (int)X[i][0], (int)X[i][1], prediction);
  // }

  delete_Perceptron(p);
  return 0;
}