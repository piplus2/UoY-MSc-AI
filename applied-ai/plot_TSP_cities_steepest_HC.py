import matplotlib.pyplot as plt
import pandas as pd

def plot_tsp(file_init, file_final):
    df_init = pd.read_csv(file_init, names=['X', 'Y'])
    df_final = pd.read_csv(file_final, names=['X', 'Y'])

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 7))

    # Plot Percorso Iniziale
    ax1.plot(df_init['X'], df_init['Y'], 'ro-', markersize=5, linewidth=1)
    ax1.set_title(f"Percorso Iniziale (Random)")
    
    # Plot Percorso Finale
    ax2.plot(df_final['X'], df_final['Y'], 'bo-', markersize=5, linewidth=1)
    ax2.set_title(f"Percorso Finale (Steepest HC)")

    for ax in [ax1, ax2]:
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.grid(True)

    plt.tight_layout()
    plt.savefig("plots/best_sol.png")
    plt.close()

plot_tsp("results/path_initial.csv", "results/path_final.csv")
