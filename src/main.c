#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "rng.h"

typedef struct Vec {
  double x, y;
} vec;

int index(int i, int j, int n);
int get_n();
vec *get_cities();
double dist(vec v1, vec v2);
double *get_dist_matrix(vec *cities, int n);
int *initial_guess(int n);
double get_weight(int *route, double *dist_matrix, int n);
int mod(int a, int b);
void cp_array(int *arr1, int *arr2, int n);
void read_n_anneal(double T, int steps, int simuls);
void update_route(
  int *route, double *dist_matrix,
  double *weight, double T, int n
);
void print_route(int *route, double weight, int n);
double check_dist(int idx1, int idx2, int *route, double *dist_matrix, int n);

int main(int argc, char **argv) {
  seed_mt(time(NULL));

  assert(argc == 4);
  double T = atof(argv[1]); // Starting temperature
  int steps = atoi(argv[2]); // Steps per single simulation
  int simuls = atoi(argv[3]); // Number of individual simulations

  read_n_anneal(T, steps, simuls);

  return 0;
}

int index(int i, int j, int n) {
  return i*n + j;
}

int get_n() {
  FILE *f_in;
  f_in = fopen("../input/coords.txt", "r");

  int n;
  fscanf(f_in, "%d", &n);

  fclose(f_in);
  return n;
}

vec *get_cities() {
  double coord;
  FILE *f_in;
  f_in = fopen("../input/coords.txt", "r");

  int n;
  fscanf(f_in, "%d", &n);
  vec *cities = malloc(n * sizeof(vec));

  for (int i = 0; i<n; i++) {
    for (int j = 0; j<2; j++) {
      fscanf(f_in, "%lf", &coord);

      if (j == 0) {
        cities[i].x = coord;
      } else {
        cities[i].y = coord;
      }
    }
  }

  fclose(f_in);
  return cities;
}

double dist(vec v1, vec v2) {
  return sqrt(pow(v1.x-v2.x, 2) + pow(v1.y-v2.y, 2));
}

double *get_dist_matrix(vec *cities, int n) {
  double *dist_matrix = malloc(n*n*sizeof(double));

  for (int i = 0; i<n; i++) {
    for (int j = 0; j<n; j++) {
      dist_matrix[index(i,j,n)] = dist(cities[i], cities[j]);
    }
  }

  return dist_matrix;
}

int *initial_guess(int n) {
  int temp, rand_idx;

  // Initializing
  int *guess = malloc(n*sizeof(int));
  for (int i = 0; i<n; i++) {
    guess[i] = i;
  }

  // Shuffling
  for (int i = 0; i<n; i++) {
    rand_idx = rand_mt_int()%n;
    temp = guess[i];
    guess[i] = guess[rand_idx];
    guess[rand_idx] = temp;
  }

  return guess;
}

double get_weight(int *route, double *dist_matrix, int n) {
  double sum = 0;
  int idx1, idx2;
  for (int i = 0; i<n; i++) {
    idx1 = route[i];
    idx2 = route[(i+1)%n];

    sum += dist_matrix[index(idx1,idx2,n)];
  }

  return sum;
}

int mod(int a, int b) {
  int ret = a % b;
  if (ret < 0) ret+=b;
  return ret;
}

void cp_array(int *arr1, int *arr2, int n) {
  for (int i = 0; i<n; i++) {
    arr1[i] = arr2[i];
  }
}

void read_n_anneal(double T, int steps, int simuls) {
  assert(steps>=10 && simuls>0);

  double weight, best_weight;
  int *best_route, *route, n=get_n();
  best_route = malloc(n*sizeof(int));
  vec *cities = get_cities();
  double *dist_matrix = get_dist_matrix(cities, n);

  assert(n>1);

  for (int r = 0; r<simuls; r++) {
    route = initial_guess(n);
    weight = get_weight(route, dist_matrix, n);

    if (r==0) {
      cp_array(best_route, route, n);
      best_weight = weight;
    }

    for (int i = 0; i<steps; i++) {
      update_route(route, dist_matrix, &weight, T, n);

      if (weight < best_weight) {
        best_weight = weight;
        cp_array(best_route, route, n);
      }

      if (i % (steps/10) == 0) {
        T /= 2;
      }
    }

    printf("\r%d/%d", r+1, simuls);
    fflush(stdout);

    free(route);
  }

  printf("\r");
  print_route(best_route, best_weight, n);
  free(dist_matrix);
  free(cities);
  free(best_route);
}

void update_route(
  int *route, double *dist_matrix,
  double *weight, double T, int n
) {
  int temp;

  // Picking random indexes to switch
  int idx1, idx2;
  idx1 = idx2 = rand_mt_int()%n;
  while (idx1 == idx2) idx2 = rand_mt_int()%n;

  double old=0, new=0, dE;
  old = check_dist(idx1, idx2, route, dist_matrix, n);

  // Do the switch
  temp = route[idx1];
  route[idx1] = route[idx2];
  route[idx2] = temp;

  new = check_dist(idx1, idx2, route, dist_matrix, n);
  dE = new-old;

  // Possibly switch back
  if (dE > 0 && rand_mt() > exp(-dE/T)) {
    temp = route[idx1];
    route[idx1] = route[idx2];
    route[idx2] = temp;
  } else {
    *weight += dE;
  }
}

void print_route(int *route, double weight, int n) {
  FILE *f_out;
  f_out = fopen("../output/best_route.txt", "w");

  fprintf(stdout, "%lf:\t", weight);
  fprintf(f_out, "%lf:\t", weight);
  for (int i = 0; i<n; i++) {
    fprintf(stdout, "%d ", route[i]);
    fprintf(f_out, "%d ", route[i]);
  }
  fprintf(stdout, "\n");
  fprintf(f_out, "\n");

  fclose(f_out);
}

double check_dist(int idx1, int idx2, int *route, double *dist_matrix, int n) {
  double check = 0;
  int next, prev;

  next = mod(idx1+1, n);
  prev = mod(idx1-1, n);
  check += dist_matrix[index(route[idx1],route[prev],n)];
  check += dist_matrix[index(route[idx1],route[next],n)];

  next = mod(idx2+1, n);
  prev = mod(idx2-1, n);
  check += dist_matrix[index(route[idx2],route[prev],n)];
  check += dist_matrix[index(route[idx2],route[next],n)];

  return check;
}
