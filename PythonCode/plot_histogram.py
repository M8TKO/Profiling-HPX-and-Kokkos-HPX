import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

input_filename_hpx = 'thread_counts_hpx.txt'
input_filename_openmp = 'thread_counts_openmp.txt'
plot_title = 'Distribution of Active Threads: HPX vs. OpenMP'
output_filename = 'thread_histogram_frequency_comparison.png'

try:
    data_hpx = np.loadtxt(input_filename_hpx, dtype=int)
    print(f"📈 Successfully loaded {len(data_hpx)} data points from '{input_filename_hpx}'.")

    data_openmp = np.loadtxt(input_filename_openmp, dtype=int)
    print(f"📈 Successfully loaded {len(data_openmp)} data points from '{input_filename_openmp}'.")

    sns.set_theme(style="whitegrid")
    plt.figure(figsize=(12, 7))

    # We will use a higher bin count for more detail
    number_of_bins = 40

    # Plot the OpenMP data as a standard, semi-transparent histogram
    sns.histplot(data_openmp, bins=number_of_bins, color='salmon', alpha=0.6, label='OpenMP')

    # Plot the HPX data as an outline ("step") on top, with a thicker line
    sns.histplot(data_hpx, bins=number_of_bins, color='skyblue', label='HPX', element='step', linewidth=2)

    plt.legend()
    plt.xlabel('Number of Active OS-Level Threads')
    plt.ylabel('Frequency (Number of Samples)')
    plt.title(plot_title)

    plt.savefig(output_filename, dpi=300)
    print(f"✅ Plot saved successfully as '{output_filename}'")

except FileNotFoundError as e:
    print(f"🛑 Error: The file '{e.filename}' was not found.")
except Exception as e:
    print(f"An error occurred: {e}")