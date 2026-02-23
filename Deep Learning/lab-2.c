// WAP to implement MP Neuron

#include <stdio.h>
#include <stdlib.h>

typedef struct MPNeuron
{
  int *weights;
  int numWeights;
  int threshold;
} MPNeuron;

MPNeuron *new_MPNeuron(int numWeights, int threshold)
{
  MPNeuron *neuron = (MPNeuron *)malloc(sizeof(MPNeuron));
  if (!neuron)
  {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }
  neuron->weights = (int *)malloc(numWeights * sizeof(int));
  if (!neuron->weights)
  {
    fprintf(stderr, "Memory allocation for weights failed\n");
    free(neuron);
    return NULL;
  }

  for (int i = 0; i < numWeights; i++)
    neuron->weights[i] = 1; // default

  neuron->numWeights = numWeights;
  neuron->threshold = threshold;
  return neuron;
}

void delete_MPNeuron(MPNeuron *neuron)
{
  free(neuron->weights);
  free(neuron);
}

int activate(MPNeuron *neuron, const int *inputs, int numInputs)
{
  if (!neuron || !neuron->weights || !inputs)
  {
    fprintf(stderr, "Invalid neuron OR inputs\n");
    return -1; // Error code
  }

  if (numInputs != neuron->numWeights)
  {
    fprintf(stderr, "Input size mismatch\n");
    return -1; // Error code
  }

  int sum = 0;
  for (int i = 0; i < numInputs; i++)
  {
    sum += neuron->weights[i] * inputs[i];
  }

  return sum >= neuron->threshold ? 1 : 0;
}

void set_weights(MPNeuron *neuron, const int *weights, int numWeights)
{
  if (!neuron || !neuron->weights || !weights)
  {
    fprintf(stderr, "Invalid neuron OR weights\n");
    return;
  }

  if (numWeights != neuron->numWeights)
  {
    fprintf(stderr, "Weights size mismatch\n");
    return;
  }

  for (int i = 0; i < numWeights; i++)
    neuron->weights[i] = weights[i];
}

int main()
{
  const int num_dimns = 3;
  const int inputs[][3] = {
      {0, 0, 0},
      {0, 0, 1},
      {0, 1, 0},
      {0, 1, 1},
      {1, 0, 0},
      {1, 0, 1},
      {1, 1, 0},
      {1, 1, 1}};
  const int num_samples = 1 << num_dimns;

  // AND gate
  printf("\n----- ----- ----- AND GATE ----- ----- -----\n");
  MPNeuron *and_neuron = new_MPNeuron(num_dimns, num_dimns);
  for (int i = 0; i < num_samples; i++)
  {
    int result = activate(and_neuron, inputs[i], num_dimns);
    printf("Input: ");
    for (int j = 0; j < num_dimns; j++)
      printf("%d ", inputs[i][j]);

    printf("-> Output: %d\n", result);
  }
  delete_MPNeuron(and_neuron);

  // OR gate
  printf("\n----- ----- ----- OR GATE ----- ----- -----\n");
  MPNeuron *or_neuron = new_MPNeuron(num_dimns, 1);
  for (int i = 0; i < num_samples; i++)
  {
    int result = activate(or_neuron, inputs[i], num_dimns);
    printf("Input: ");
    for (int j = 0; j < num_dimns; j++)
      printf("%d ", inputs[i][j]);

    printf("-> Output: %d\n", result);
  }
  delete_MPNeuron(or_neuron);

  // NOT gate
  printf("\n----- ----- ----- NOT GATE ----- ----- -----\n");
  MPNeuron *not_neuron = new_MPNeuron(1, 0);
  not_neuron->weights[0] = -1;
  const int not_inputs[2][1] = {{0}, {1}};
  for (int i = 0; i < 2; i++)
  {
    int result = activate(not_neuron, not_inputs[i], 1);
    printf("Input: %d -> Output: %d\n", not_inputs[i][0], result);
  }
  delete_MPNeuron(not_neuron);

  // NAND gate
  printf("\n----- ----- ----- NAND GATE ----- ----- -----\n");
  MPNeuron *nand_neuron = new_MPNeuron(num_dimns, -num_dimns + 1);
  const int nand_weights[] = {-1, -1, -1};
  set_weights(nand_neuron, nand_weights, num_dimns);
  for (int i = 0; i < num_samples; i++)
  {
    int result = activate(nand_neuron, inputs[i], num_dimns);
    printf("Input: ");
    for (int j = 0; j < num_dimns; j++)
      printf("%d ", inputs[i][j]);

    printf("-> Output: %d\n", result);
  }
  delete_MPNeuron(nand_neuron);

  // NOR gate
  printf("\n----- ----- ----- NOR GATE ----- ----- -----\n");
  MPNeuron *nor_neuron = new_MPNeuron(num_dimns, 0);
  const int nor_weights[] = {-1, -1, -1};
  set_weights(nor_neuron, nor_weights, num_dimns);
  for (int i = 0; i < num_samples; i++)
  {
    int result = activate(nor_neuron, inputs[i], num_dimns);
    printf("Input: ");
    for (int j = 0; j < num_dimns; j++)
      printf("%d ", inputs[i][j]);

    printf("-> Output: %d\n", result);
  }
  delete_MPNeuron(nor_neuron);

  return 0;
}
