import heapq
from fileinput import filename

import geopandas as gpd
import matplotlib.pyplot as plt
import networkx as nx

# 1. Coordinate Geografiche (Latitudine, Longitudine)
city_coords = {
    "Glasgow": (-4.25, 55.86),
    "Chester": (-2.89, 53.19),
    "Sheffield": (-1.47, 53.38),
    "Stoke": (-2.17, 53.00),
    "Cambridge": (0.12, 52.20),
    "Oban": (-5.47, 56.41),
    "Sunderland": (-1.38, 54.90),
    "Aberdeen": (-2.11, 57.15),
    "FortWilliam": (-5.10, 56.81),
    "Nottingham": (-1.15, 52.95),
    "Durham": (-1.58, 54.77),
    "Birmingham": (-1.90, 52.48),
    "Derby": (-1.47, 52.92),
    "Norwich": (1.29, 52.63),
    "Newcastle": (-1.61, 54.97),
    "Hartlepool": (-1.21, 54.69),
    "Coventry": (-1.51, 52.41),
    "Torquay": (-3.52, 50.46),
    "Inverness": (-4.22, 57.47),
    "Swindon": (-1.77, 51.56),
    "Oxford": (-1.25, 51.75),
    "Reading": (-0.97, 51.45),
    "Plymouth": (-4.14, 50.37),
    "Peterborough": (-0.24, 52.57),
    "Exeter": (-3.53, 50.71),
    "Cardiff": (-3.18, 51.48),
    "Middlesbrough": (-1.23, 54.57),
    "Blackpool": (-3.03, 53.81),
    "Dundee": (-2.97, 56.46),
    "Portsmouth": (-1.09, 50.81),
    "Dumfries": (-3.60, 55.07),
    "Lancaster": (-2.80, 54.04),
    "Carlisle": (-2.93, 54.89),
    "Liverpool": (-2.99, 53.40),
    "Truro": (-5.05, 50.26),
    "Manchester": (-2.24, 53.48),
    "Bristol": (-2.58, 51.45),
    "Kendal": (-2.74, 54.32),
    "York": (-1.08, 53.96),
    "Swansea": (-3.94, 51.62),
    "Edinburgh": (-3.18, 55.95),
    "Leicester": (-1.13, 52.63),
    "Leeds": (-1.55, 53.80),
    "Preston": (-2.70, 53.76),
    "Brighton": (-0.13, 50.82),
    "Southampton": (-1.40, 50.90),
    "London": (-0.12, 51.50),
    "Winchester": (-1.31, 51.06),
}


# 2. Caricamento Dati e Grafo
def load_graph_data(filename):
    G = nx.Graph()
    try:
        with open(filename, "r") as f:
            for line in f:
                parts = line.strip().split(",")
                if len(parts) >= 3:
                    u = parts[0].strip().replace('"', "").replace("'", "")
                    v = parts[1].strip().replace('"', "").replace("'", "")
                    if u.lower() == "citya":
                        continue
                    # Aggiungi solo se abbiamo le coordinate
                    if u in city_coords and v in city_coords:
                        G.add_edge(u, v)
    except FileNotFoundError:
        print(f"File {filename} non trovato.")
    return G


def plot_route(paths, start, end, styles, save_filename="plot.png", avoid=None):
    filename = "data/route_finding.csv"
    G = load_graph_data(filename)

    fig, ax = plt.subplots(figsize=(10, 12))

    url = "https://naturalearth.s3.amazonaws.com/110m_cultural/ne_110m_admin_0_countries.zip"
    world = gpd.read_file(url)
    uk_map = world[world["NAME"].isin(["United Kingdom", "Ireland"])]
    uk_map.plot(ax=ax, color="#ecf0f1", edgecolor="#bdc3c7")

    pos = {city: coords for city, coords in city_coords.items() if city in G.nodes()}

    nx.draw_networkx_edges(G, pos, ax=ax, edge_color="lightgray", alpha=0.5)
    nx.draw_networkx_nodes(G, pos, ax=ax, node_size=15, node_color="#2c3e50")
    nx.draw_networkx_labels(
        G,
        pos,
        ax=ax,
        labels={node: node for node in G.nodes()},
        font_size=12,
        font_color="#34495e",
        verticalalignment="bottom",
    )

    for path, style in zip(paths, styles):
        path_edges = list(zip(path, path[1:]))
        nx.draw_networkx_edges(
            G, pos, ax=ax, edgelist=path_edges, edge_color=style["color"], width=style["width"], alpha=style["alpha"]
        )
        nx.draw_networkx_nodes(
            G, pos, ax=ax, nodelist=path, node_size=40, node_color=style["color"], alpha=style["alpha"]
        )
        ax.plot([], [], color=style["color"], label=style["label"], linewidth=4, alpha=style["alpha"])

    nx.draw_networkx_nodes(G, pos, ax=ax, nodelist=[start], node_size=80, node_color="green", label="Start")
    nx.draw_networkx_nodes(G, pos, ax=ax, nodelist=[end], node_size=80, node_color="gold", label="End")
    if avoid in city_coords:
        ax.plot(
            city_coords[avoid][0], city_coords[avoid][1], "kx", markersize=12, markeredgewidth=3, label="Avoid (London)"
        )

    path_labels = {node: node for node in paths[0]}
    # Aggiungi anche London se esiste
    if avoid in city_coords:
        path_labels[avoid] = avoid

    # nx.draw_networkx_labels(
    #     G,
    #     pos,
    #     ax=ax,
    #     labels=path_labels,
    #     font_size=9,
    #     font_weight="bold",
    #     verticalalignment="bottom",
    #     font_color="#333333",
    # )

    ax.set_xlim(-6, 2)
    ax.set_ylim(50, 56)

    plt.title(f"Optimal Logistics Route: {start} to {end} (Avoiding {avoid})", fontsize=14)
    plt.legend(loc="upper left")
    plt.axis("off")

    # Salva o Mostra
    plt.tight_layout()
    plt.savefig(save_filename, dpi=300)
    plt.show()
