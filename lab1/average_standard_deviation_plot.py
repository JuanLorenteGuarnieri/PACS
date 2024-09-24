import matplotlib.pyplot as plt
import numpy as np

# Provided data
data = {
    'type': ['own', 'own', 'own', 'own', 'own', 'eigen', 'eigen', 'eigen', 'eigen', 'eigen'],
    'matrix_size': [10, 20, 50, 100, 200, 10, 20, 50, 100, 200],
    'average_time': [
        3.39655E-04, 2.66297E-03, 4.23170E-02, 3.36349E-01, 2.72197E+00,
        2.95018E-05, 1.04585E-04, 2.20484E-03, 1.63089E-02, 1.29277E-01
    ],
    'standard_deviation_time': [
        1.357270E-06, 2.952470E-06, 2.347020E-04, 5.713070E-05, 2.581770E-03,
        2.662940E-05, 3.722960E-05, 6.184200E-05, 1.551120E-04, 4.848220E-04
    ]
}

# Convert data to numpy arrays for easier handling
matrix_sizes = np.array(data['matrix_size'])
average_times = np.array(data['average_time'])
standard_deviations = np.array(data['standard_deviation_time'])
types = np.array(data['type'])

# Create a figure for the plots
plt.figure(figsize=(12, 6))

# Configure the average time plot
plt.subplot(1, 2, 1)
for label in np.unique(types):
    plt.plot(matrix_sizes[types == label], average_times[types == label], marker='o', label=label)

plt.title('Average Times of Implementations (Hendrix machine)')
plt.xlabel('Matrix Size')
plt.ylabel('Average Time (s)')
plt.xticks(matrix_sizes)  # Display all matrix sizes on the x-axis
plt.legend()
plt.grid()

# Configure the standard deviation plot
plt.subplot(1, 2, 2)
for label in np.unique(types):
    plt.plot(matrix_sizes[types == label], standard_deviations[types == label], marker='o', label=label)

plt.title('Standard Deviation of Implementations (Hendrix machine)')
plt.xlabel('Matrix Size')
plt.ylabel('Standard Deviation (s)')
plt.xticks(matrix_sizes)  # Display all matrix sizes on the x-axis
plt.legend()
plt.grid()

# Show the plots
plt.tight_layout()
plt.show()
