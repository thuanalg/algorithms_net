#include "../sp_alg_vector.h"


// Function to find the maximum sum subarray and its positions using Kadane's Algorithm
int sp_alg_kadane(int *arr, int n, SP_ALG_KADANE_RESULT *resurt) {
	int ret = 0;
	do {
    int max_so_far = arr[0];      // Holds the maximum sum found so far
    int max_ending_here = arr[0]; // Holds the maximum sum of a subarray ending at the current position
    int start = 0;                // Starting index of the maximum sum subarray
    int end = 0;                  // Ending index of the maximum sum subarray
    int temp_start = 0;           // Temporary start index for the current subarray being evaluated
    if (!resurt) {
        ret = -1;
        break;
    }
    // Iterate through the array starting from the second element
    for (int i = 1; i < n; i++) {
        // Decide whether to start a new subarray or extend the existing one
        if (arr[i] > max_ending_here + arr[i]) {
            max_ending_here = arr[i]; // Start a new subarray at arr[i]
            temp_start = i;           // Update the temporary start index
        } else {
            max_ending_here = max_ending_here + arr[i]; // Extend the current subarray
        }

        // Update max_so_far and the start/end indices if a larger sum is found
        if (max_ending_here > max_so_far) {
            max_so_far = max_ending_here;
            start = temp_start; // Set the start of the maximum subarray
            end = i;            // Set the end of the maximum subarray
        }
    }
	
    // Create and return the result structure
        resurt->start = start;
        resurt->max_sum = max_so_far;
        resurt->end = end;
	} while(0);
    return ret;
}
