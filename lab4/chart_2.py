# Input values from the output
#lockfree
numbers_per_ms = [1494, 1482, 1659, 1738, 1771]
time_taken_ms = [13385, 13494, 12055, 11502, 11288]

#thread_safe
# numbers_per_ms = [1460, 1411, 1458, 1619, 1361]
# time_taken_ms = [13695, 14170, 13710, 12348, 14693]


# Calculate the average of "Numbers per ms"
avg_numbers_per_ms = sum(numbers_per_ms) / len(numbers_per_ms)

# Calculate the average of "Time taken"
avg_time_taken_ms = sum(time_taken_ms) / len(time_taken_ms)

# Output the averages
print(f"Average Numbers per ms: {avg_numbers_per_ms:.2f}")
print(f"Average Time taken (ms): {avg_time_taken_ms:.2f}")
