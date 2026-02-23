// Implement a Deterministic Fininte Automata
// read the machine config from a file, and an input string
// simulate the machine and find out if the string gets accepted or not

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_HANDLER_SAFE(msg)            \
  do                                     \
  {                                      \
    fprintf(stderr, "[Err]: %s\n", msg); \
    freeDFA(machine);                    \
    fclose(fp);                          \
    return NULL;                         \
  } while (0)

// ----- utils -----
int isValidState(int state, int numStates) { return state >= 0 && state < numStates; }

typedef struct DFA_t
{
  int **transitions;  // 8 // transitions[state][input] = nextState
  int *finalStates;   // 8
  int numTransitions; // 4
  int numFinalStates; // 4
  int initState;      // 4
  int numStates;      // 4
} DFA_t;              // 32 bytes

int reserveDFA_finalStates(DFA_t *dfa, int numFinalStates)
{
  dfa->finalStates = (int *)malloc(sizeof(int) * numFinalStates);
  if (!dfa->finalStates)
    return 0; // allocation failed

  dfa->numFinalStates = numFinalStates;
  return 1; // success
}

int reserveDFA_transitions(DFA_t *dfa, int numStates)
{
  dfa->transitions = (int **)malloc(sizeof(int *) * numStates);
  if (!dfa->transitions)
    return 0; // allocation failed

  for (int i = 0; i < numStates; i++)
  {
    dfa->transitions[i] = (int *)malloc(sizeof(int) * 26); // Assuming inputs are a-z
    if (!dfa->transitions[i])
    {
      // Free previously allocated rows
      for (int j = 0; j < i; j++)
      {
        free(dfa->transitions[j]);
      }
      free(dfa->transitions);
      return 0; // allocation failed
    }
    for (int j = 0; j < 26; j++)
    {
      dfa->transitions[i][j] = -1; // -1 indicates no transition defined
    }
  }
  return 1; // success
}

void freeDFA(DFA_t *dfa)
{
  if (!dfa)
    return;

  if (dfa->transitions)
  {
    for (int i = 0; i < dfa->numStates; i++)
    {
      free(dfa->transitions[i]);
    }
  }
  free(dfa->transitions);
  free(dfa->finalStates);
  free(dfa);
}

/*
 * DFA_config.txt format:
  3     ; number of states
  0     ; initial state
  2     ; number of final states
  1 2   ; final states
  5     ; number of transitions
  0 a 0 ; transition 1
  0 b 1 ; transition 2
  1 b 1 ; ..
  1 c 2 ; ..
  2 c 2 ; transition 5
 */
int writeDFAConfig(DFA_t *machine, const char *filename)
{
  FILE *fp = fopen(filename, "w");
  if (!fp)
  {
    perror("Failed to open file");
    return 0; // failure
  }

  fprintf(fp, "%d\n", machine->numStates);
  fprintf(fp, "%d\n", machine->initState);
  fprintf(fp, "%d\n", machine->numFinalStates);
  for (int i = 0; i < machine->numFinalStates; i++)
  {
    fprintf(fp, "%d", machine->finalStates[i]);
    if (i + 1 < machine->numFinalStates)
      fprintf(fp, " ");
  }
  fprintf(fp, "\n");

  fprintf(fp, "%d\n", machine->numTransitions);
  for (int i = 0; i < machine->numStates; i++)
  {
    for (int j = 0; j < 26; j++)
    {
      if (machine->transitions[i][j] != -1)
      {
        fprintf(fp, "%d %c %d\n", i, 'a' + j, machine->transitions[i][j]);
      }
    }
  }

  fclose(fp);
  return 1; // success
}

DFA_t *createNewDFAConfig_user(const char *filename)
{
  printf("# Guide to DFA config file format:\n");
  printf("1. First line: number of states\n");
  printf("2. Second line: initial state\n");
  printf("3. Third line: number of final states\n");
  printf("4. Fourth line: list of final states (space-separated)\n");
  printf("5. Fifth line: number of transitions\n");
  printf("6. Next lines: transition in format 'fromState input toState'\n\n");

  DFA_t *machine = (DFA_t *)malloc(sizeof(DFA_t));
  if (!machine)
  {
    perror("Failed to allocate memory");
    return NULL;
  }

  printf("[NOTE]: Number of states should be between 1 and 100.\n");
  do
  {
    printf("Enter number of states [0, 100]: ");
    scanf("%d", &machine->numStates);
  } while (machine->numStates <= 0 || machine->numStates > 100);

  printf("[NOTE]: Initial state should be between 0 and %d.\n", machine->numStates - 1);
  do
  {
    printf("Enter initial state [0, %d]: ", machine->numStates - 1);
    scanf("%d", &machine->initState);
  } while (!isValidState(machine->initState, machine->numStates));

  printf("[NOTE]: Number of final states should be between 1 and %d.\n", machine->numStates);
  do
  {
    printf("Enter number of final states [1, %d]: ", machine->numStates);
    scanf("%d", &machine->numFinalStates);
  } while (machine->numFinalStates < 1 || machine->numFinalStates > machine->numStates);

  reserveDFA_finalStates(machine, machine->numFinalStates);
  printf("[NOTE]: Any Final state should be between 0 and %d.\n", machine->numStates - 1);
  for (int i = 0; i < machine->numFinalStates; i++)
  {
    int tempFinalState;
    do
    {
      printf("Enter final state %d: ", i + 1);
      scanf("%d", &tempFinalState);
    } while (!isValidState(tempFinalState, machine->numStates));
    machine->finalStates[i] = tempFinalState;
  }

  printf("[NOTE]: Number of transitions should be between 0 and %d.\n", machine->numStates * 26);
  int maxTransitions = machine->numStates * 26; // Assuming max 26 inputs (a-z)
  do
  {
    printf("Enter number of transitions [0, %d]: ", maxTransitions);
    scanf("%d", &machine->numTransitions);
  } while (machine->numTransitions < 0 || machine->numTransitions > maxTransitions);

  reserveDFA_transitions(machine, machine->numStates);
  printf("[NOTE]: Each transition should be in format 'fromState input toState'.\n");
  for (int i = 0; i < machine->numTransitions; i++)
  {
    int fromState, toState;
    char onInput;
    do
    {
      printf("Enter transition %d (fromState, input, toState): ", i + 1);
      scanf("%d %c %d", &fromState, &onInput, &toState);
    } while (!isValidState(fromState, machine->numStates) ||
             !isValidState(toState, machine->numStates) ||
             onInput < 'a' || onInput > 'z');

    machine->transitions[fromState][onInput - 'a'] = toState;
  }

  int result = writeDFAConfig(machine, filename);
  if (result)
  {
    printf("DFA configuration saved successfully to '%s'.\n", filename);
  }
  else
  {
    fprintf(stderr, "Failed to save DFA configuration to '%s'.\n", filename);
  }

  return machine;
}

DFA_t *readDFAConfig(const char *filename)
{
  FILE *fp = fopen(filename, "r");
  if (!fp)
  {
    perror("Failed to open file");
    return NULL;
  }

  DFA_t *machine = (DFA_t *)malloc(sizeof(DFA_t));
  if (!machine)
    ERR_HANDLER_SAFE("Failed to allocate memory");

  // read number of states
  fscanf(fp, "%d", &machine->numStates);
  if (machine->numStates <= 0 || machine->numStates > 100)
    ERR_HANDLER_SAFE("Invalid number of states in config file.");

  // read initial state
  fscanf(fp, "%d", &machine->initState);
  if (!isValidState(machine->initState, machine->numStates))
    ERR_HANDLER_SAFE("Invalid initial state in config file.");

  // read number of final states
  fscanf(fp, "%d", &machine->numFinalStates);
  if (machine->numFinalStates < 1 || machine->numFinalStates > machine->numStates)
    ERR_HANDLER_SAFE("Invalid number of final states in config file.");

  // read final states []
  reserveDFA_finalStates(machine, machine->numFinalStates);
  {
    for (int i = 0; i < machine->numFinalStates; i++)
    {
      int tempFinalState;
      fscanf(fp, "%d", &tempFinalState);
      if (!isValidState(tempFinalState, machine->numStates))
      {
        ERR_HANDLER_SAFE("Invalid final state in config file.");
      }
      machine->finalStates[i] = tempFinalState;
    }
  }

  // read number of transitions
  fscanf(fp, "%d", &machine->numTransitions);
  int maxTransitions = machine->numStates * 26; // Assuming max 26 inputs (a-z)
  if (machine->numTransitions < 0 || machine->numTransitions > maxTransitions)
  {
    ERR_HANDLER_SAFE("Invalid number of transitions in config file.");
  }

  // read transitions [](fromState, input, toState)
  reserveDFA_transitions(machine, machine->numStates);
  for (int i = 0; i < machine->numTransitions; i++)
  {
    int fromState, toState;
    char onInput;
    fscanf(fp, "%d %c %d", &fromState, &onInput, &toState);
    if (!isValidState(fromState, machine->numStates) ||
        !isValidState(toState, machine->numStates) ||
        onInput < 'a' || onInput > 'z')
    {
      ERR_HANDLER_SAFE("Invalid transition in config file.");
    }
    machine->transitions[fromState][onInput - 'a'] = toState;
  }

  fclose(fp);
  return machine;
}

void printDFA(DFA_t *machine)
{
  if (!machine)
  {
    fprintf(stderr, "Error: Invalid DFA machine.\n");
    return;
  }

  printf("DFA Configuration:\n");
  printf("{\n");
  printf("  Number of states: %d,\n", machine->numStates);
  printf("  Initial state: %d,\n", machine->initState);
  printf("  Number of final states: %d,\n", machine->numFinalStates);
  printf("  Final states: [");
  for (int i = 0; i < machine->numFinalStates; i++)
  {
    printf("%d", machine->finalStates[i]);
    if (i < machine->numFinalStates - 1)
      printf(", ");
  }
  printf("],\n");

  printf("  Number of transitions: %d,\n", machine->numTransitions);
  printf("  Transitions: [\n");
  int transitionCount = 0;
  for (int i = 0; i < machine->numStates; i++)
  {
    for (int j = 0; j < 26; j++)
    {
      if (machine->transitions[i][j] != -1)
      {
        printf("    %d -- %c --> %d", i, 'a' + j, machine->transitions[i][j]);
        transitionCount++;

        if (transitionCount < machine->numTransitions)
          printf(",\n");
        else
          break;
      }
    }
  }
  printf("\n  ]\n");

  printf("}\n");
}

typedef enum StringState
{
  REJECTED = 0,
  ACCEPTED = 1
} StringState;

void simulateDFA(DFA_t *machine)
{
  if (!machine)
  {
    fprintf(stderr, "Error: Invalid DFA machine.\n");
    return;
  }

  char inputString[100];
  while (1)
  {
    printf("Enter input string to simulate (only a-z) or type 'quit' to exit\n>>> ");
    scanf("%100s", inputString);

    if (strcmp(inputString, "quit") == 0)
      break;

    int currentState = machine->initState;
    StringState resultFlag = REJECTED;

    for (int i = 0; inputString[i] != '\0'; i++)
    {
      char inputChar = inputString[i];
      if (inputChar < 'a' || inputChar > 'z')
      {
        printf("[Err]: Invalid input character '%c'. Only a-z are allowed.\n", inputChar);
        continue;
      }

      int nextState = machine->transitions[currentState][inputChar - 'a'];
      if (nextState == -1)
      {
        printf("[Log]: No transition defined for state %d on input '%c'.\n", currentState, inputChar);
        resultFlag = REJECTED;
        break;
      }

      // else, valid transition exists, move to next state
      printf("[Log]: %d -- %c --> %d\n", currentState, inputChar, nextState);
      currentState = nextState;
    }

    // Check if the final state is a final state
    for (int i = 0; i < machine->numFinalStates; i++)
    {
      if (currentState == machine->finalStates[i])
      {
        resultFlag = ACCEPTED;
        break;
      }
    }

    if (resultFlag == ACCEPTED)
      printf("[Result]: String accepted.\n");
    else if (resultFlag == REJECTED)
      printf("[Result]: String rejected.\n");
    printf("----------------------------------------------------------------\n");
  }
}

int main()
{
  int choice = -1;
  while (choice != 0)
  {
    printf("\n1. Create and Save DFA config file\n");
    printf("2. Read DFA config file and simulate\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    DFA_t *machine = NULL;
    switch (choice)
    {
    case 1:
    {
      char filename[100];
      printf("Enter filename to save DFA config: ");
      scanf("%100s", filename);
      machine = createNewDFAConfig_user(filename);
      break;
    }
    case 2:
    {
      char filename[100];
      printf("Enter filename to read DFA config: ");
      scanf("%100s", filename);
      machine = readDFAConfig(filename);
      break;
    }
    case 0:
      printf("Exiting...\n");
      return 0;
    default:
      printf("Invalid choice. Please try again.\n");
      break;
    }

    // If a machine was created or read, we can simulate it here
    if (!machine)
      continue; // Skip simulation if machine creation/reading failed

    printDFA(machine);
    simulateDFA(machine);
  }
  return 0;
}