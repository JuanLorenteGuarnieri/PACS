import matplotlib.pyplot as plt

# Data for i7 4710hq
Msize = [100, 200, 500, 800, 1000]
eigen_i7 = [0.000269, 0.0016, 0.02, 0.086, 0.1559]
own_i7 = [0.0018, 0.009, 0.19, 0.434, 4]

# Data for i5 8300H
eigen_i5 = [0.000252648, 0.00194936, 0.019397, 0.0836897, 0.149753]
own_i5 = [0.00199728, 0.0111416, 0.207646, 0.943771, 1.71802]


# Data for i7 4710hq
Msize_hendrix = [10, 20, 50, 100, 200]
Eigen_hendrix = [2.95018e-05, 1.04585e-04, 2.20484e-03, 1.63089e-02, 1.29277e-01]
own_hendrix = [3.39655e-04, 2.66297e-03, 4.23170e-02, 3.36349e-01, 2.72197e+00]


# Create the plot
plt.figure(figsize=(10, 6))

# Plot for i7 4710hq
plt.plot(Msize, eigen_i7, marker='s', color='blue', label='i7 4710HQ Eigen')
plt.plot(Msize, own_i7, marker='s', color='orange', label='i7 4710HQ Own')

# Plot for i5 8300H
plt.plot(Msize, eigen_i5, marker='o', color='red', label='i5 8300H Eigen')
plt.plot(Msize, own_i5, marker='o', color='green', label='i5 8300H Own')

# Plot for hendrix
plt.plot(Msize_hendrix, Eigen_hendrix, marker='x', color='black', label='Hendrix Eigen')
plt.plot(Msize_hendrix, own_hendrix, marker='x', color='grey', label='Hendrix Own')

# Add labels and title
plt.xlabel('Matrix Size (Msize)')
plt.ylabel('Average Time (seconds)')
plt.title('Comparison of Eigen and Own Implementations on i7 4710HQ & i5 8300H & hendrix')
plt.legend()

# Show plot
plt.grid(True)
plt.show()
