import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import sys

# Run with: python plot_histogram.py thread_counts_openmp.txt

# --- Configuration ---
# Get the input filename from the command line argument.
input_filename = sys.argv[1] if len(sys.argv) > 1 else 'thread_counts.txt'

# The title of your plot.
plot_title = 'Distribution of Active Threads'

# The filename for the saved image.
output_filename = 'thread_histogram.png'

# --- Script Logic ---
try:
    # Load the data from your text file.
    data = np.loadtxt(input_filename, dtype=int)
    print(f"📈 Successfully loaded {len(data)} data points from '{input_filename}'.")

    # Set a nice visual style for the plot.
    sns.set_theme(style="whitegrid")

    # Create the plot figure.
    plt.figure(figsize=(10, 6))

    # Create the histogram using seaborn.
    sns.histplot(data, kde=True, bins=30, color='skyblue', edgecolor='black')

    # Add labels and a title for clarity.
    plt.xlabel('Number of Active OS-Level Threads')
    plt.ylabel('Frequency (Number of Samples)')
    plt.title(plot_title)

    # Save the plot to a file.
    plt.savefig(output_filename, dpi=300)
    print(f"✅ Plot saved successfully as '{output_filename}'")

except FileNotFoundError:
    print(f"🛑 Error: The file '{input_filename}' was not found.")
except Exception as e:
    print(f"An error occurred: {e}")