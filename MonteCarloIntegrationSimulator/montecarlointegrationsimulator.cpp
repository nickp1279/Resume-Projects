// Copyright 2025 Nick Pieroni All rights reserved.

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <limits>  // For integer max
#include <random>  // For mt19937
#include <iomanip>  // For setprecision

/* 
 * Monte Carlo Integration function to estimate PI.
 *
 * This function simulates throwing some number of blocks of darts, where
 * one block is 1,000,000 darts. It will then check if a dart landed within
 * the unit sphere. The ratio of darts inside versus outside is used to
 * estimate PI.
*/
double MCInt(int numBlocks, int seed, std::mt19937 mt,
            const uint_fast32_t MT_MAX) {
    // Cast to avoid rounding issues
    uint64_t numDarts = static_cast<uint64_t>(numBlocks) * 1000000;
    uint64_t in = 0;
    uint64_t out = 0;

    for (uint64_t i = 0; i < numDarts; i++) {
        // Generate random x, y and z coordinates
        double x = static_cast<double>(mt()) / MT_MAX;
        double y = static_cast<double>(mt()) / MT_MAX;
        double z = static_cast<double>(mt()) / MT_MAX;

        // Use equation to check if dart falls within sphere
        if ((x * x + y * y + z * z) < 1.0) {
            // Dart inside the unit sphere
            in++;
        } else {
            // Dart outside the unit sphere
            out++;
        }
    }

    // Percentage of darts that were inside the sphere
    // Then calculate the estimation for PI and return to future
    double frac = static_cast<double>(in) / numDarts;
    double est = frac * 6.0;

    return est;
}

/*
 * Runs the Monte Carlo Integration in parallel across mutliple
 * async threads.
 * 
 * This function divides the total number of dart blocks among the
 * threads given, assigns each thread a unique seed then runs the 
 * Monte Carlo Integration. Once all threads return their estimates,
 * an average will be calculated.
*/
void doMt(int nBlk, int nThs, int rSd) {
    std::vector<std::future<double>> futures;
    std::vector<int> threadBlocks(nThs);

    // Set the number of blocks for each thread and handle odd numBlocks
    int baseBlocks = nBlk / nThs;
    int remainder = nBlk % nThs;
    for (int i = 0; i < nThs; ++i) {
        threadBlocks[i] = baseBlocks + (i < remainder ? 1 : 0);
    }

    // Set precision once before creating the new threads, more effecient
    std::cout << std::fixed << std::setprecision(8);
    for (int i = 0; i < nThs; i++) {
        // Randomize the seed for each thread
        int mySeed = rSd + 23*i;
        int threadBlockNum = threadBlocks[i];

        // Generate each random sequence using unique seed, call max
        // once, more effecient
        std::mt19937 mt(mySeed);
        const uint_fast32_t MT_MAX = mt.max();

        // Create new async thread(s) with unique seed and number of blocks
        futures.push_back(std::async(std::launch::async, MCInt,
            threadBlockNum, mySeed, mt, MT_MAX));
    }

    // Add estimates from async threads
    double estSum = 0.0;
    for (auto& f : futures) {
        estSum += f.get();
    }

    // Average the estimates from each thread
    double piEst = estSum / nThs;
    std::cout << "Estimate for pi is " << piEst << std::endl;
}

/*
 * Validates CLA arguments and starts the multithread function.
 *
 * This function checks the input CLAs from the user, ensuring
 * only 3 params are passed. Then it will validate the args given.
 * Checking the num of blocks, num of threads, and the seed. If
 * all are valid it starts the multithreaded method.
*/
void validCLA(int argc, char* argv[]) {
    // Check for the correct number of CLAs, print error if not
    if (argc != 4) {
        std::cerr << "Usage: homework02 numBlocks numthreads RNSeed"
            << std::endl;
        exit(0);
    }
    try {
        // Check CLAs are valid ints
        int nBlk = std::stoi(argv[1]);
        int nThs = std::stoi(argv[2]);
        int rSd = std::stoi(argv[3]);

        if (nBlk <= 0 || nThs <= 0 || rSd <= 0) throw std::invalid_argument("");

        // Number of blocks must be more than the number of threads
        if (nBlk <= nThs) {
            std::cerr << "The number of blocks must be >= the number of threads"
                << std::endl;
            exit(0);
        }

        // Start multithreading
        doMt(nBlk, nThs, rSd);
    } catch (const std::exception& e) {
        // If arguments are not valid ints, catch and print error
        std::cerr << "Error: arguments must be positive int values"
            << std::endl;
        exit(0);
    }
}

int main(int argc, char* argv[]) {
    // Validate CLAs given
    validCLA(argc, argv);

    return 0;
}
