import matplotlib.pyplot as plt
import pandas as pd

def plot_tsp(file_final):
    df_final = pd.read_csv(file_final, names=['X', 'Y'])

    fig, ax1 = plt.subplots(1, 1, figsize=(15, 7))
    
    ax1.plot(df_final['X'], df_final['Y'], 'bo-', markersize=5, linewidth=1)
    ax1.set_title(f"Final path")
    ax1.set_xlabel("X")
    ax1.set_ylabel("Y")
    ax1.grid(True)

    plt.tight_layout()
    plt.savefig("plots/sa_final_path.png")
    plt.close()

plot_tsp("results/sa_final_path.csv")
