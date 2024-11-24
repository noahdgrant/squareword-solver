#include "factorial.h"
#include "logger.h"

// Function to calculate factorial
unsigned long long factorial(int n) {
    if (n == 0 || n == 1) {
        return 1;
    }
    unsigned long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    logger(INFO, "%d! = %llu", n, result);
    return result;
}

// Function to calculate binomial coefficient
unsigned long long binomial_coefficient(int n, int k) {
    if (k > n) {
        return 0;
    }
    unsigned long long n_fact = factorial(n);
    unsigned long long k_fact = factorial(k);
    unsigned long long n_minus_k_fact = factorial(n - k);
    unsigned long long result = n_fact / (k_fact * n_minus_k_fact);
    logger(INFO, "Binomial coefficient (%d choose %d): %llu", n, k, result);
    return result;
}
