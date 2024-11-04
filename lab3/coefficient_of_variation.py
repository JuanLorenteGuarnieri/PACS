import numpy as np

# Given execution times
execution_times = [
    349.474279000,
    349.454280469,
    174.508893260,
    174.486753277,
    174.466720359
]

# execution_times = [
#     22.098378604,
#     22.103034770,
#     22.101011378,
#     22.103192381,
#     22.102881509,
#     44.158820640,
#     44.149906529,
#     44.158342752,
#     44.158367139,
#     44.158522049,
#     87.989026308,
#     87.986558149,
#     87.986558149,
#     87.992549185,
#     87.995777133,
#     174.470359508,
#     174.496656112,
#     174.508893260,
#     174.486753277,
#     174.466720359,
# ]

# Calculate the coefficient of variation for the execution times
mean_execution_time = np.mean(execution_times)
std_dev_execution_time = np.std(execution_times)
coefficient_of_variation = (std_dev_execution_time / mean_execution_time) * 100

print(coefficient_of_variation)
