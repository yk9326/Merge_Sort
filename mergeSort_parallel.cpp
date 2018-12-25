#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
using namespace std;
// Varying number of elements and threads. Elements/Threads needs to be an integer.
#define ELEMENTS 30000000
#define THREADS 60

void mergeSort(int[], int, int);
void merge(int[], int, int, int);
void *callMergeSort(void*);

struct argMergeSort {
	int p, r;
	int *A;
};

int main() {
	// initializing input array with random numbers
	int *A = new int[ELEMENTS];
	for (int i = 0; i < ELEMENTS; i++) {
		A[i] = rand() % 1000;
	}

	// timer begin
	auto begin = std::chrono::high_resolution_clock::now();

	// partition array into subarrays
	int** C = new int*[THREADS];
	for (int i = 0; i < THREADS; ++i)
		C[i] = new int[ELEMENTS / THREADS + 1];

	// initializing sub-arrays
	for (int j = 0; j < THREADS; j++) {
		for (int i = 0; i < (ELEMENTS / THREADS); i++) {
			C[j][i] = A[i + (j * ELEMENTS / THREADS)];
		}
	}

	// pass each subarray to mergeSort
	pthread_t thread[THREADS];

	struct argMergeSort args[THREADS];
	for (int i = 0; i < THREADS; i++) {
		args[i].A = C[i];
		args[i].p = 0;
		args[i].r = (ELEMENTS / THREADS - 1);
		pthread_create(&thread[i], NULL, callMergeSort, (void *)&args[i]);
	}

	for (int i = 0; i < THREADS; i++)
		pthread_join(thread[i], NULL);

	// set last elements of subarrays to infinity
	for (int i = 0; i < THREADS; i++) {
		C[i][ELEMENTS / THREADS] = INT32_MAX;
	}

	// merging subarrays
	int B[THREADS];
	for (int i = 0; i < THREADS; i++)
		B[i] = 0;

	for (int i = 0; i < ELEMENTS; i++) {
		int min = C[0][B[0]]; B[0]++; int b = 0;
		for (int j = 1; j < THREADS; j++) {
			if (C[j][B[j]] < min) {
				min = C[j][B[j]]; B[j]++; B[b]--; b = j;
			}
		}
		A[i] = min;
	}

	// timer end
	auto end = std::chrono::high_resolution_clock::now();
	printf("elapsed time: %d \n", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

	// optional output array
	/*for (int i = 0; i < 40; i++)
		cout << A[i] << "   ";*/

	delete[] A; 
	for (int i = 0; i < THREADS; i++)
		delete[] C[i];

	system("pause");
	return 0;
}

void mergeSort(int A[], int p, int r) {
	int q;
	if (p < r) {
		q = (p + r) / 2;
		mergeSort(A, p, q);
		mergeSort(A, q + 1, r);
		merge(A, p, q, r);
	}
	return;
}

void *callMergeSort(void *thread_arg)
{
	argMergeSort* args = (argMergeSort*)thread_arg;
	mergeSort(args->A, args->p, args->r);
	return NULL;
}

void merge(int A[], int p, int q, int r) {
	int n1 = q - p;
	int n2 = r - q - 1;
	int *L, *R;
	L = new int[n1 + 2];
	R = new int[n2 + 2];

	for (int i = 0; i <= n1; i++)
		L[i] = A[p + i];
	for (int i = 0; i <= n2; i++)
		R[i] = A[q + i + 1];
	L[n1 + 1] = INT32_MAX;
	R[n2 + 1] = INT32_MAX;

	int j = 0, k = 0;
	for (int i = p; i <= r; i++) {
		if (L[j] <= R[k]) {
			A[i] = L[j];
			j++;
		}
		else {
			A[i] = R[k];
			k++;
		}
	}

	delete[] L;
	delete[] R;

	return;
}