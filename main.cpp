#include <iostream>
#include <cstdlib>
#include <memory>
#include <time.h>
#include <omp.h>
using namespace std;

const int SIZE = 1e8;
const int MAX_IT = 10;

void serial_counting_sort(int* arr) {
	int index = 0, max_value = 0;

	for (int i = 0; i < SIZE; i++) {
		max_value = max(max_value, arr[i]);
	}

	int* count = new int[max_value + 1];
	fill(count, count + max_value + 1, 0);

	for (int i = 0; i < SIZE; i++) {
		count[arr[i]]++;
	}

	for (int i = 0; i <= max_value; i++) {
		for (int j = 1; j <= count[i]; j++) {
			arr[index++] = i;
		}
	}
}

void parallel_counting_sort(int* arr) {
	int* count;
	int max_value = 0;
	int index = 0;

#pragma omp parallel for reduction(max: max_value)
	for (int i = 0; i < SIZE; i++) {
		max_value = arr[i] > max_value ? arr[i] : max_value;
	}

	count = new int[max_value + 1];
	fill(count, count + max_value + 1, 0);

#pragma omp parallel for
	for (int i = 0; i < SIZE; i++) {
#pragma omp atomic
		count[arr[i]]++;
	}

	for (int i = 0; i <= max_value; i++) {
		for (int j = 1; j <= count[i]; j++) {
			arr[index++] = i;
		}
	}
}

bool valid(int* s_arr, int* p_arr) {
	for (int i = 0; i < SIZE; i++) {
		if (s_arr[i] != p_arr[i])
			return false;
	}
	return true;
}

int main() {
	int thread_size = omp_get_max_threads();
	omp_set_num_threads(thread_size);
	srand(time(0));

	double s_avr = 0.0;
	double p_avr = 0.0;

	for (int it = 1; it <= MAX_IT; it++) {
		cout << " # Iteration " << it << endl;

		int* s_arr = new int[SIZE];
		int* p_arr = new int[SIZE];
		for (int i = 0; i < SIZE; i++) {
			s_arr[i] = rand() % SIZE;
			p_arr[i] = s_arr[i];
		}

		/* Serial counting sort */
		double s_start = omp_get_wtime();
		serial_counting_sort(s_arr);
		double s_end = omp_get_wtime();
		cout << "Serial counting sort elapsed time: " << s_end - s_start << " seconds." << endl;

		/* Parallel counting sort */
		double p_start = omp_get_wtime();
		parallel_counting_sort(p_arr);
		double p_end = omp_get_wtime();
		cout << "Parallel counting sort elapsed time: " << p_end - p_start << " seconds." << endl;

		if (!valid(s_arr, p_arr))
			cout << "Parallel counting sort gives wrong result." << endl;

		s_avr += s_end - s_start;
		p_avr += p_end - p_start;

		delete s_arr;
		delete p_arr;
		cout << endl;
	}

	cout << "Thread size: " << thread_size << endl;
	cout << "Serial average time: " << s_avr / MAX_IT << " seconds." << endl;
	cout << "Parallel average time: " << p_avr / MAX_IT << " seconds." << endl;

	return 0;
}