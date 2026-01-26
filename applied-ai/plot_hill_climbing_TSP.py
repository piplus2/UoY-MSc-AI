import glob

import matplotlib.pyplot as plt
import pandas as pd

plt.figure(figsize=(10, 6))
for file in sorted(glob.glob("run_*.csv")):
    df = pd.read_csv(file, header=None)
    df.columns = ["Iteration", "Cost"]
    plt.plot(df["Iteration"], df["Cost"], label=file)

plt.title("Hill Climbing TSP: Cost")
plt.xlabel("Iterations")
plt.ylabel("Cost (Total Distance)")
plt.legend()
plt.grid(True)
# plt.show()

plt.savefig("Cost_TSP_hill_climbing.png")
plt.close()
