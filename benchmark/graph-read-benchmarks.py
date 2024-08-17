import json
import matplotlib.pyplot as plt

data_filepath = 'benchmark/read_times.json'

with open(data_filepath, 'r') as file:
    data = json.load(file)

benchmark_n = data['benchmark_n']
read_times = data['read_times']

plt.figure(figsize=(10, 6))
plt.plot(benchmark_n, read_times, color='blue', marker='o', markersize=1, linestyle='-', linewidth=0.5)
plt.title('Total Read Time vs Number of Requests')
plt.xlabel('Number of Requests')
plt.ylabel('Read Times (microseconds)')
plt.grid(True)
plt.xscale('log')
plt.yscale('log')
plt.savefig("benchmark/read-benchmarks")
plt.close()