import matplotlib.pyplot as plt
import numpy as np

def read_times_from_file(filename):
    """Reads times from a file and returns them as a list of floats."""
    with open(filename, 'r') as file:
        times = [float(line.strip()) for line in file]
    return times

def plot_times(times, title, filename):
    """Plots times and saves the plot to a file."""
    plt.figure(figsize=(10, 6))
    plt.plot(times, color='blue', marker='o', markersize=1, linestyle='-', linewidth=0.5)
    plt.title(title)
    plt.ylabel('Time (ms)')
    plt.grid(True)
    plt.savefig(filename)
    plt.close()

def main():
    # File paths
    write_time_file = '_benchmark_data/write_times.txt'
    read_time_file = '_benchmark_data/read_times.txt'

    # Read times
    write_times = read_times_from_file(write_time_file)[:1000]
    read_times = read_times_from_file(read_time_file)[:1000]

    # Convert times from microseconds to milliseconds
    write_times_ms = np.array(write_times) / 1000
    read_times_ms = np.array(read_times) / 1000

    # Plot and save graphs
    plot_times(write_times_ms, 'Write Operation Times (First 1000)', 'lib/kvstore/benchmark/write_times.png')
    plot_times(read_times_ms, 'Read Operation Times (First 1000)', 'lib/kvstore/benchmark/read_times.png')

if __name__ == '__main__':
    main()
