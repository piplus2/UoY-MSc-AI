import pandas as pd
import networkx as nx
import itertools


def find_interesting_routes(filename="data/route_finding.csv"):
    # --- 1. CARICAMENTO E PULIZIA DATI ---
    print("Caricamento dati in corso...")
    try:
        with open(filename, "r") as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Errore: Il file '{filename}' non è stato trovato.")
        return

    data = []
    for line in lines:
        # Rimuove spazi e caratteri di a capo
        line = line.strip()
        if not line:
            continue

        # Divide la riga per virgola
        parts = line.split(",")
        if len(parts) >= 3:
            # Pulisce eventuali virgolette o spazi extra
            u = parts[0].strip().replace('"', "").replace("'", "")
            v = parts[1].strip().replace('"', "").replace("'", "")
            w_str = parts[2].strip().replace('"', "").replace("'", "")

            # Salta l'intestazione o righe non valide
            if u.lower() == "citya" or not w_str.isdigit():
                continue

            data.append((u, v, int(w_str)))

    # --- 2. CREAZIONE DEL GRAFO ---
    G = nx.Graph()
    for u, v, w in data:
        G.add_edge(u, v, weight=w)

    print(f"Grafo creato con {len(G.nodes)} città e {len(G.edges)} collegamenti.")

    # --- 3. IDENTIFICAZIONE NODO 'LONDON' ---
    # Cerca il nodo che contiene la parola "London" (case insensitive)
    london_node = next((n for n in G.nodes if "London" in str(n)), None)

    if not london_node:
        print("Errore: Nodo 'London' non trovato nel grafo.")
        return

    print(f"Nodo Londra identificato come: '{london_node}'")

    # Crea una copia del grafo SENZA Londra per il confronto
    G_no_london = G.copy()
    G_no_london.remove_node(london_node)

    # --- 4. ANALISI DI TUTTE LE COPPIE ---
    interesting_pairs = []
    all_nodes = list(G.nodes)

    # itertools.combinations crea tutte le coppie possibili senza ripetizioni (A-B è uguale a B-A)
    for u, v in itertools.combinations(all_nodes, 2):
        # Ignora le coppie che iniziano o finiscono proprio a Londra
        if u == london_node or v == london_node:
            continue

        try:
            # Calcola percorso MIGLIORE nel grafo completo (con Londra)
            # Usiamo Dijkstra (weight='weight') per trovare la distanza minima reale
            path_orig = nx.shortest_path(G, u, v, weight="weight")

            # Ci interessa solo se il percorso migliore PASSA effettivamente per Londra
            if london_node in path_orig:
                dist_orig = nx.shortest_path_length(G, u, v, weight="weight")

                # Ora calcola percorso alternativo SENZA Londra
                if nx.has_path(G_no_london, u, v):
                    path_alt = nx.shortest_path(G_no_london, u, v, weight="weight")
                    dist_alt = nx.shortest_path_length(G_no_london, u, v, weight="weight")

                    diff = dist_alt - dist_orig

                    # Salviamo solo se c'è una differenza di distanza (detour)
                    if diff > 0:
                        interesting_pairs.append(
                            {
                                "Start": u,
                                "End": v,
                                "Via_London_Dist": dist_orig,
                                "Avoid_London_Dist": dist_alt,
                                "Penalty": diff,
                                "Path_Via_London": path_orig,
                                "Path_Avoid_London": path_alt,
                            }
                        )
        except nx.NetworkXNoPath:
            # Se non c'è collegamento tra due città (grafo disconnesso), ignora
            continue

    # --- 5. ORDINAMENTO E OUTPUT ---
    # Ordina per penalità decrescente (i detour più grandi per primi)
    interesting_pairs.sort(key=lambda x: x["Penalty"], reverse=True)

    print(f"\nTrovate {len(interesting_pairs)} coppie di città interessanti.")
    print("-" * 60)
    print(f"{'Città A':<15} | {'Città B':<15} | {'Diff (Miglia)':<12} | {'Dist. Senza Londra'}")
    print("-" * 60)

    for p in interesting_pairs:
        print(f"{p['Start']:<15} | {p['End']:<15} | +{p['Penalty']:<12} | {p['Avoid_London_Dist']}")

    return interesting_pairs


# Esegui la funzione
if __name__ == "__main__":
    results = find_interesting_routes()
