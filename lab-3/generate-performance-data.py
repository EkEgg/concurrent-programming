import os
import shutil
import subprocess
import statistics
import csv

GENERATE_MATRIX = "./generate-matrix"
COMPUTE_MATRIX_PRODUCT = "./compute-product"
COMPUTE_MATRIX_PRODUCT_THREADED = "./compute-product-threaded"

TEMP_DIRECTORY = "./temp"
MATRIX_A_PATH = f"{TEMP_DIRECTORY}/matrix-a"
MATRIX_B_PATH = f"{TEMP_DIRECTORY}/matrix-b"
MATRIX_PRODUCT_PATH = f"{TEMP_DIRECTORY}/matrix-product"
MATRIX_PRODUCT_THREADED_PATH = f"{TEMP_DIRECTORY}/matrix-threaded-product"

EXECUTIONS_PER_TEST = 5
MATRIX_SIZES = [300, 550, 800]
THREADS_COUNTS = [1, 2, 4, 8]

EXECUTION_TIME_OUTPUT_PATH = "./execution-time.csv"
ACCELERATION_OUTPUT_PATH = "./acceleration.csv"
EFFICIENCY_OUTPUT_PATH = "./efficiency.csv"

if not os.path.isdir(TEMP_DIRECTORY):
    os.mkdir(TEMP_DIRECTORY)

execution_time_file = open(EXECUTION_TIME_OUTPUT_PATH, "w")
execution_time_csv = csv.writer(execution_time_file)
execution_time_csv.writerow(["Size/Threads", "Sequential"] + [f"{i} threads" for i in THREADS_COUNTS])

acceleration_file = open(ACCELERATION_OUTPUT_PATH, "w")
acceleration_csv = csv.writer(acceleration_file)
acceleration_csv.writerow(["Size/Threads"] + THREADS_COUNTS)

efficiency_file = open(EFFICIENCY_OUTPUT_PATH, "w")
efficiency_csv = csv.writer(efficiency_file)
efficiency_csv.writerow(["Size/Threads"] + THREADS_COUNTS)

for matrix_size in MATRIX_SIZES:

    os.system(f"{GENERATE_MATRIX} {MATRIX_A_PATH} {matrix_size} {matrix_size}")
    os.system(f"{GENERATE_MATRIX} {MATRIX_B_PATH} {matrix_size} {matrix_size}")

    print(f"Simulating with matrix size {matrix_size}...")

    execution_times = []
    for _ in range(EXECUTIONS_PER_TEST):
        process = subprocess.run(
            f"{COMPUTE_MATRIX_PRODUCT} {MATRIX_A_PATH} {MATRIX_B_PATH} {MATRIX_PRODUCT_PATH}",
            shell=True,
            check=True,
            capture_output=True    
        )
        execution_time = float(process.stdout.decode().split("\n")[1].split(" ")[-1])
        execution_times.append(execution_time)
    
    average_sequential_execution_time = statistics.mean(execution_times)

    execution_time_csv_row = [matrix_size, average_sequential_execution_time]
    acceleration_csv_row = [matrix_size]
    efficiency_csv_row = [matrix_size]

    for threads_count in THREADS_COUNTS:

        print(f"Simulating with {threads_count} threads...")

        execution_times = []
        for _ in range(EXECUTIONS_PER_TEST):
            process = subprocess.run(
                f"{COMPUTE_MATRIX_PRODUCT_THREADED} {MATRIX_A_PATH} {MATRIX_B_PATH} {MATRIX_PRODUCT_PATH} {threads_count}",
                shell=True,
                check=True,
                capture_output=True
            )
            execution_time = float(process.stdout.decode().split("\n")[1].split(" ")[-1])
            execution_times.append(execution_time)
        
        average_threaded_execution_time = statistics.mean(execution_times)
        execution_time_csv_row.append(average_threaded_execution_time)

        acceleration = average_sequential_execution_time / average_threaded_execution_time
        acceleration_csv_row.append(acceleration)

        efficiency = acceleration / threads_count
        efficiency_csv_row.append(efficiency)
    
    execution_time_csv.writerow(execution_time_csv_row)
    acceleration_csv.writerow(acceleration_csv_row)
    efficiency_csv.writerow(efficiency_csv_row)

execution_time_file.close()
acceleration_file.close()
efficiency_file.close()

shutil.rmtree(TEMP_DIRECTORY)
