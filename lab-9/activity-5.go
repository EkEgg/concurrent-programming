package main

import (
	"os"
	"fmt"
	"strconv"
)

func MinInt(a int, b int) int {
	if b < a {
		return b
	}
	return a
}

func IsPrime(number int) bool {

	if number <= 1 {
		return false
	}
	if number == 2 {
		return true
	}
	if number % 2 == 0 {
		return false
	}
	for i := 3; i*i <= number; i += 2 {
		if number % i == 0 {
			return false
		}
	}
	return true
}

func CountPrimesInRangeThreaded(rangeMin int, rangeMax int, threadsCount int) int {
	
	rangeSize := rangeMax - rangeMin + 1

	threadsCount = MinInt(threadsCount, rangeSize)

	primesCountChannel := make(chan int)

    threadBegin := 1
    for i := 0; i < threadsCount; i++ {

		elementsForThisThread := rangeSize / threadsCount
		if i < rangeSize % threadsCount {
			elementsForThisThread++
		}

		go func(begin int, elementsCount int) {

			primesCount := 0

			for i := begin; i < begin + elementsCount; i++ {
				if IsPrime(i) {
					primesCount++
				}
			}

			primesCountChannel <- primesCount

		}(threadBegin, elementsForThisThread)

		threadBegin += elementsForThisThread
    }

	primesCount := 0
	for i := 0; i < threadsCount; i++ {
		primesCount += <- primesCountChannel
	}

	return primesCount 
}

func main() {

	if len(os.Args) != 3 {
		fmt.Printf("[Error] Expected 2 arguments. Got %d.\n", len(os.Args)-1)
		fmt.Printf("[Usage] go run activity-5.go [N] [ThreadsCount]\n")
		fmt.Printf("[Description] Counts how many primes there are between 1 and N (inclusive).\n")
		return
	}

	n, error := strconv.Atoi(os.Args[1])
	if error != nil || n <= 0 {
		fmt.Printf("[Error] [N] must be a positive integer.")
		return
	}

	threadsCount, error := strconv.Atoi(os.Args[1])
	if error != nil || threadsCount <= 0 {
		fmt.Printf("[Error] [ThreadsCount] must be a positive integer.")
		return
	}

	fmt.Println(CountPrimesInRangeThreaded(1, n, threadsCount))
}