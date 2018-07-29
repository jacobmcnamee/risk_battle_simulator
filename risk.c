#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <assert.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define ROLL_ATTACK_DICE_MAX 3
#define ROLL_DEFEND_DICE_MAX 2

/* Returns a uniformly distributed random integer in [0, n) */
int rand_below(int n)
{
  assert(n > 0);
  assert(n <= RAND_MAX);

  int bin_width = RAND_MAX / n;
  int limit = n * bin_width;

  int r;
  do {
    r = rand();
  } while (r >= limit);

  return r / bin_width;
}

/* Returns a uniformly distributed random integer in [min, max] */
int rand_range(int min, int max)
{
  assert(min <= max);

  return min + rand_below(max - min + 1);
}

/* Returns a random integer in [1, 6] */
int rand_die(void)
{
  return rand_range(1, 6);
}

/* Compare function for qsort() */
int compare_int_desc(const void *a, const void *b)
{
  int n = *(const int *)a;
  int m = *(const int *)b;

  if (n > m) {
    return -1;
  } else if (n < m) {
    return 1;
  } else {
    return 0;
  }
}

/* Populates the dice array with n sorted random die rolls */
void rand_dice_sorted(int *dice, int n)
{
  for (int i = 0; i < n; i++) {
    dice[i] = rand_die();
  }

  qsort(dice, n, sizeof(*dice), compare_int_desc);
}

/* Simulates a single roll of the dice */
void risk_roll(int n_a, int n_d, int *loss_a, int *loss_d)
{
  assert(n_a > 0);
  assert(n_a <= ROLL_ATTACK_DICE_MAX);
  assert(n_d > 0);
  assert(n_d <= ROLL_DEFEND_DICE_MAX);

  int a[n_a];
  rand_dice_sorted(a, n_a);

  int d[n_d];
  rand_dice_sorted(d, n_d);

  *loss_a = 0;
  *loss_d = 0;

  int pairs = MIN(n_a, n_d);
  for (int i = 0; i < pairs; i++) {
    if (d[i] < a[i]) {
      (*loss_d)++;
    } else {
      (*loss_a)++;
    }
  }
}

/* Simulates dice rolls until the attacker or defender wins */
void risk_battle(int n_a, int n_d, int *loss_a, int *loss_d)
{
  int curr_n_a = n_a;
  int curr_n_d = n_d;

  while (curr_n_a > 0 && curr_n_d > 0) {
    int roll_n_a = MIN(curr_n_a, ROLL_ATTACK_DICE_MAX);
    int roll_n_d = MIN(curr_n_d, ROLL_DEFEND_DICE_MAX);
    int roll_loss_a;
    int roll_loss_d;
    risk_roll(roll_n_a, roll_n_d, &roll_loss_a, &roll_loss_d);
    curr_n_a -= roll_loss_a;
    curr_n_d -= roll_loss_d;
  }

  *loss_a = n_a - curr_n_a;
  *loss_d = n_d - curr_n_d;
}

void usage(void)
{
  puts("usage:");
  puts("risk sim <num_attackers> <num_defenders>");
  puts("risk prob <num_attackers> <num_defenders> <runs>");
}

/* Main function for the 'sim' subcommand. Simulates a battle */
int run_sim(int argc, char *argv[])
{
  if (argc != 4) {
    usage();
    return EXIT_FAILURE;
  }

  int n_a;
  sscanf(argv[2], "%d", &n_a);

  int n_d;
  sscanf(argv[3], "%d", &n_d);

  printf("simulating battle with %d attackers vs. %d defenders\n", n_a, n_d);

  int loss_a;
  int loss_d;
  risk_battle(n_a, n_d, &loss_a, &loss_d);

  puts("results:");
  printf("attacker loses %d armies\n", loss_a);
  printf("defender loses %d armies\n", loss_d);
  printf("%s wins\n", loss_a == n_a ? "defender" : "attacker");

  return EXIT_SUCCESS;
}

void print_hist(int *hist, int n, int runs)
{
  puts("armies lost | probability | cumulative");

  int hist_sum = 0;
  for (int i = 0; i < n; i++) {
    hist_sum += hist[i];
    printf("%3d           %5.1f%%        %5.1f%%\n",
           i, 100.0 * (double)hist[i] / runs, 100.0 * (double)hist_sum / runs);
  }
}

/* Main function for the 'prob' subcommand. Computes probabilities for the
 * possible outcomes of a battle */
int run_prob(int argc, char *argv[])
{
  if (argc != 5) {
    usage();
    return EXIT_FAILURE;
  }

  int n_a;
  sscanf(argv[2], "%d", &n_a);

  int n_d;
  sscanf(argv[3], "%d", &n_d);

  int runs;
  sscanf(argv[4], "%d", &runs);

  printf("calculating probabilities for %d attackers vs. %d defenders using "
         "%d runs\n", n_a, n_d, runs);

  int *hist_loss_a = (int *)calloc(n_a, sizeof(int));
  assert(hist_loss_a != NULL);

  int *hist_loss_d = (int *)calloc(n_d, sizeof(int));
  assert(hist_loss_d != NULL);

  for (int i = 0; i < runs; i++) {
    int loss_a;
    int loss_d;
    risk_battle(n_a, n_d, &loss_a, &loss_d);

    if (loss_a < n_a) {
      hist_loss_a[loss_a]++;
    } else {
      hist_loss_d[loss_d]++;
    }
  }

  puts("results:");
  puts("attacker wins:");
  print_hist(hist_loss_a, n_a, runs);
  puts("defender wins:");
  print_hist(hist_loss_d, n_d, runs);

  free(hist_loss_a);
  free(hist_loss_d);

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
  srand(time(NULL));

  if (argc < 2) {
    usage();
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "sim") == 0) {
    return run_sim(argc, argv);
  } else if (strcmp(argv[1], "prob") == 0) {
    return run_prob(argc, argv);
  } else {
    usage();
    return EXIT_FAILURE;
  }

  return 0;
}
