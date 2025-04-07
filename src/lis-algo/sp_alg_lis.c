#include "../sp_alg_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Longest Increasing Subsequence (LIS) */
/* Dynamic Programming */
#define MAX 1000
// Function to find and print the Longest Increasing Subsequence (LIS)
int sp_alg_lis_dp(int* arr, int n) {
    int dp[MAX] = {0};      // dp[i]: length of LIS ending at i
    int prev[MAX] = { 0 };    // prev[i]: index of previous element in LIS
    int i, j;
    int ret = 0;

    // Initialize dp[] and prev[]
    for (i = 0; i < n; i++) {
        dp[i] = 1;        // Minimum LIS is 1 (the element itself)
        prev[i] = -1;     // No predecessor initially
    }

    // Compute dp[] and prev[]
    for (i = 1; i < n; i++) {
        for (j = 0; j < i; j++) {
            if (arr[j] < arr[i] && dp[j] + 1 > dp[i]) {
                dp[i] = dp[j] + 1;
                prev[i] = j;
            }
        }
    }

    // Find the maximum value in dp[] and its index
    int lis_len = 0, lis_end = 0;
    for (i = 0; i < n; i++) {
        if (dp[i] > lis_len) {
            lis_len = dp[i];
            lis_end = i;
        }
    }

    // Reconstruct the LIS
    int lis[MAX], k = 0;
    while (lis_end != -1) {
        lis[k++] = arr[lis_end];
        lis_end = prev[lis_end];
    }
    ret = k;
    // Print results
    printf("Length of LIS: %d\n", lis_len);
    printf("Longest Increasing Subsequence: ");
    for (i = k - 1; i >= 0; i--) {
        printf("%d ", lis[i]);
    }
    printf("\n");
    return ret;
}

// Main function
//int main() {
//    int arr[] = { 10, 22, 9, 33, 21, 50, 41, 60 };
//    int n = sizeof(arr) / sizeof(arr[0]);
//
//    // Call the LIS function
//    find_LIS(arr, n);
//
//    return 0;
//}
