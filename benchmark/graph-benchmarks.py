import json
import matplotlib.pyplot as plt

data_filepath = 'benchmark/stats.json'

with open(data_filepath, 'r') as file:
    data = json.load(file)

num_servers = [x['num_servers'] for x in data]
read_times = [x['read_time'] for x in data]
write_times = [x['write_time'] for x in data]

plt.figure(figsize=(10, 6))
plt.plot(num_servers, read_times, marker='o', label='read time (100000 requests)', color='b')
plt.plot(num_servers, write_times, marker='o', label='write time (100000 requests)', color='r')

plt.xlabel('number of servers')
plt.ylabel('microseconds')
plt.title('read/write time vs. number of servers (full replication)')
plt.legend()
plt.grid(True)

plt.savefig("benchmark/benchmarks")
plt.close()