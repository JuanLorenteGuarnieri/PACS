import numpy as np
import matplotlib.pyplot as plt

# Datos extendidos
w_h_div_values_extended = [1, 2, 4, 8, 16, 32, 64, 128, 200, 256, 300]
execution_times_extended = [68483, 21283, 6609, 2397, 1092, 900, 836, 790, 1253, 2945, 7734]

# Convertir los tiempos de ejecución de milisegundos a segundos
execution_times_seconds = np.array(execution_times_extended) / 1000

# Plotting con escalas adecuadas en el eje X y Y
plt.figure(figsize=(10, 6))
plt.plot(w_h_div_values_extended, execution_times_seconds, marker='o', color='b', linestyle='-')
# Ajustar tamaño de las fuentes
plt.xlabel("N (w_div = h_div)", fontsize=18)
plt.ylabel("Execution Time (seconds)", fontsize=18)
plt.title("Execution Time vs. Task Region Size (N)", fontsize=20)

# Ajustar el tamaño de los números en los ejes
plt.xticks(fontsize=16)
plt.yticks(fontsize=16)

# Mostrar el gráfico
plt.grid(True, which="both", linestyle='--', linewidth=0.5)
plt.tight_layout()
plt.show()