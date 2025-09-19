import matplotlib.pyplot as plt
import numpy as np

hpx_file = 'resultsHPX.txt'
omp_file = 'resultsOMP.txt'

try:
    data_hpx = np.loadtxt(hpx_file, delimiter=',', skiprows=1)
    data_omp = np.loadtxt(omp_file, delimiter=',', skiprows=1)

    x_hpx, y_hpx = data_hpx[:, 0], data_hpx[:, 1]
    x_omp, y_omp = data_omp[:, 0], data_omp[:, 1]

    plt.style.use('seaborn-v0_8-whitegrid')
    plt.figure(figsize=(12, 7))

    plt.plot(x_hpx, y_hpx, marker='o', linestyle='-', color='#0077b6', linewidth=2, label='Kokkos with HPX Backend')
    plt.plot(x_omp, y_omp, marker='s', linestyle='--', color='#d62828', linewidth=2, label='Kokkos with OpenMP Backend')

    plt.title('Performance Comparison: HPX vs. OpenMP Backend', fontsize=16, fontweight='bold')
    plt.xlabel('Number of HPX Scheduled Tasks', fontsize=12)
    plt.ylabel('Execution Time (seconds)', fontsize=12)
    
    plt.legend(fontsize=11)
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    
    plt.tight_layout()
    plt.savefig('performance_comparison.png', dpi=300)

    print("✅ Plot saved successfully as 'performance_comparison.png'")

except FileNotFoundError as e:
    print(f"🛑 Error: Could not find the file '{e.filename}'. Please ensure it is in the same directory.")
except Exception as e:
    print(f"An unexpected error occurred: {e}")