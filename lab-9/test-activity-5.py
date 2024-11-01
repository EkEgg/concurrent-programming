import subprocess

COUNT_PRIMES_SCRIPT = "activity-5.go"

# From <https://en.wikipedia.org/wiki/Prime-counting_function>
INPUTS_AND_EXPECTED_OUTPUTS = [
    (10, 4),
    (100, 25),
    (1000, 168),
    (10000, 1229),
    (100000, 9592),
    (1000000, 78498)
]

THREADS_COUNT_TO_TRY = [1, 2, 3, 4]

for n, expected_output in INPUTS_AND_EXPECTED_OUTPUTS:

    for threads_count in THREADS_COUNT_TO_TRY:

        print(f"Testing for N={n} and {threads_count} threads... ", end="")

        try:
            actual_output = int(
                subprocess.check_output(
                    f"go run {COUNT_PRIMES_SCRIPT} {n} {threads_count}",
                    shell=True,
                    text=True
                )
            )
        except (ValueError, subprocess.CalledProcessError):
            actual_output = None
        
        if actual_output == expected_output:
            print("Ok.")
        else:
            print("Failed.")
        