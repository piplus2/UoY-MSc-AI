import csv
import heapq
import os
import random
import time
from pathlib import Path
from typing import List, Optional, Tuple

from plot_route import plot_route


def init_csv(path, header):
    Path(path).parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", newline="", encoding="utf-8") as f:
        csv.writer(f).writerow(header)


def append_csv(path, row):
    with open(path, "a", newline="", encoding="utf-8") as f:
        csv.writer(f).writerow(row)


def load_data(filename: str) -> dict[str, list[tuple[str, float]]]:
    with open(filename, "r") as f:
        lines = f.readlines()

    graph = {}
    for i, line in enumerate(lines):
        if i == 0:
            continue
        parts = line.strip().split(",")
        if len(parts) == 3:
            u = parts[0].strip().replace('"', "").replace("'", "")
            v = parts[1].strip().replace('"', "").replace("'", "")
            w_str = parts[2].strip().replace('"', "").replace("'", "")
            graph.setdefault(u, []).append((v, float(w_str)))
            graph.setdefault(v, []).append((u, float(w_str)))  # undirected graph
    return graph


def generate_random_path(edge_list, current, goal, visited, path, avoid):
    """Generate a random initial solution (random walk / DFS with randomised neighbor order)."""
    if current == goal:
        return path

    visited.add(current)
    neighbors = [n for n, w in edge_list.get(current, []) if n not in visited and n != avoid]
    random.shuffle(neighbors)

    for neighbor in neighbors:
        res = generate_random_path(edge_list, neighbor, goal, visited, path + [neighbor], avoid)
        if res:
            return res
    return None


def get_path_cost(edge_list: dict[str, list[tuple[str, float]]], path: List[str]) -> float:
    """Total cost (objective function)."""
    cost = 0.0
    for i in range(len(path) - 1):
        u, v = path[i], path[i + 1]
        found = False
        for neighbor, weight in edge_list.get(u, []):
            if neighbor == v:
                cost += weight
                found = True
                break
        if not found:
            return float("inf")
    return cost


def dijkstra(
    edge_list: dict[str, list[tuple[str, float]]],
    start: str,
    goal: str,
    city_to_avoid: Optional[str] = None,
    log_path: Optional[str] = None,
) -> Tuple[List[str], float, int]:
    """
    Dijkstra shortest path with non-negative weights.
    'iterations' = number of priority-queue pops (node expansions).
    If log_path is provided, writes a per-iteration trace.
    """
    iterations = 0
    visited = set()
    queue: List[Tuple[float, str, List[str]]] = [(0.0, start, [start])]

    if log_path:
        init_csv(log_path, ["iter", "popped_city", "popped_cost", "queue_size", "visited_size", "pushes"])

    while queue:
        iterations += 1
        cost, current_city, path = heapq.heappop(queue)

        if city_to_avoid and current_city == city_to_avoid:
            continue
        if current_city in visited:
            continue

        visited.add(current_city)

        pushes = 0
        if current_city in edge_list:
            for v, w in edge_list[current_city]:
                if city_to_avoid and v == city_to_avoid:
                    continue
                if v in visited:
                    continue
                heapq.heappush(queue, (cost + w, v, path + [v]))
                pushes += 1

        if log_path:
            append_csv(log_path, [iterations, current_city, cost, len(queue), len(visited), pushes])

        if current_city == goal:
            return path, cost, iterations

    return [], float("inf"), iterations


def hill_climbing(
    edge_list: dict[str, list[tuple[str, float]]],
    start: str,
    goal: str,
    city_to_avoid: Optional[str] = None,
    log_path: Optional[str] = None,
) -> Tuple[List[str], float, int, int, int]:
    """
    Hill climbing with a 'shortcut' neighbourhood:
    If there is a direct edge between u and v on the current path that is cheaper than the subpath u..v, replace the subpath.
    Returns: (path, final_cost, iterations, last_improved_iter, evals_total)
    'iterations' = number of outer-loop neighbourhood scans until no improvement.
    """

    current_path = generate_random_path(edge_list, start, goal, set(), [start], city_to_avoid)
    if not current_path:
        return [], float("inf"), 0, 0, 0

    current_cost = get_path_cost(edge_list, current_path)
    evals_total = 0
    iterations = 0
    last_improved = 0

    if log_path:
        init_csv(log_path, ["iter", "cost", "improved", "delta", "evals_total"])
        append_csv(log_path, [0, current_cost, 0, 0.0, evals_total])

    while True:
        iterations += 1
        prev_cost = current_cost
        best_path = None
        best_cost = current_cost

        path_len = len(current_path)

        for i in range(1, path_len - 1):
            for j in range(i + 2, path_len):
                evals_total += 1
                u = current_path[i]
                v = current_path[j]

                # Check whether there is a direct edge u->v
                direct_dist = float("inf")
                for neigh, w in edge_list.get(u, []):
                    if neigh == v:
                        direct_dist = w
                        break

                if direct_dist < float("inf"):
                    old_segment_cost = get_path_cost(edge_list, current_path[i : j + 1])
                    if direct_dist < old_segment_cost:
                        candidate_path = current_path[: i + 1] + current_path[j:]
                        candidate_cost = get_path_cost(edge_list, candidate_path)
                        if candidate_cost < best_cost:
                            best_cost = candidate_cost
                            best_path = candidate_path

        if best_path and best_cost < current_cost:
            current_path = best_path
            current_cost = best_cost
            improved = 1
            delta = prev_cost - current_cost
            last_improved = iterations
        else:
            improved = 0
            delta = 0.0

        if log_path:
            append_csv(log_path, [iterations, current_cost, improved, delta, evals_total])

        if improved == 0:
            break

    return current_path, current_cost, iterations, last_improved, evals_total


def experiment(edge_list, start, goal, num_runs=10, avoid=None):
    OUTDIR = "outdir"
    os.makedirs(OUTDIR, exist_ok=True)

    summary = f"{OUTDIR}/summary.csv"
    init_csv(
        summary,
        ["algorithm", "run", "success", "final_cost", "iters", "last_improved", "evals_total", "runtime_ms", "path_len", "seed"],
    )

    results_dijkstra = []
    results_hill_climbing = []

    # Run Dijkstra 'num_runs' times for assessment parity (deterministic -> same output each run)
    for i in range(num_runs):
        t0 = time.perf_counter()
        path, cost, iters = dijkstra(edge_list, start, goal, avoid, log_path=(f"{OUTDIR}/dijkstra_log.csv" if i == 0 else None))
        t1 = time.perf_counter()
        append_csv(
            summary,
            [
                "dijkstra",
                i,
                int(bool(path)),
                cost,
                iters,
                "NA",
                "NA",
                (t1 - t0) * 1000,
                len(path),
                "NA",
            ],
        )
        results_dijkstra.append((path, cost, iters))

    # Run hill climbing multiple times
    for i in range(num_runs):
        seed = random.randrange(100000)
        random.seed(seed)  # ensure reproducibility
        t0 = time.perf_counter()
        path, cost, iterations, last_improved, evals_total = hill_climbing(
            edge_list, start, goal, avoid, log_path=f"{OUTDIR}/hill_climbing_run_{i}_log.csv"
        )
        t1 = time.perf_counter()
        append_csv(
            summary,
            [
                "hill_climbing",
                i,
                int(bool(path)),
                cost,
                iterations,
                last_improved,
                evals_total,
                (t1 - t0) * 1000,
                len(path),
                seed,
            ],
        )
        results_hill_climbing.append((path, cost, iterations, t1 - t0))

    return results_hill_climbing, results_dijkstra


def main():
    edge_list = load_data("data/route_finding.csv")
    print(f"Loaded {len(edge_list)} routes.")

    START_CITY = "York"
    END_CITY = "Portsmouth"

    hill_climbing_results, dijkstra_results = experiment(edge_list, START_CITY, END_CITY, num_runs=10, avoid="London")

    hill_climbing_results.sort(key=lambda x: x[1])
    print(f"\nBest Hill Climbing Result: Cost = {hill_climbing_results[0][1]}, Iterations = {hill_climbing_results[0][2]}")
    print(f"Worst Hill Climbing Result: Cost = {hill_climbing_results[-1][1]}, Iterations = {hill_climbing_results[-1][2]}")

    plot_route(
        [hill_climbing_results[0][0], hill_climbing_results[-1][0]],
        START_CITY,
        END_CITY,
        [
            {"color": "blue", "width": 2, "alpha": 0.5, "label": "Best Hill Climbing"},
            {"color": "red", "width": 2, "alpha": 0.5, "label": "Worst Hill Climbing"},
        ],
        avoid="London",
        save_filename="hc_route_map.png",
    )

    plot_route(
        [dijkstra_results[0][0]],
        START_CITY,
        END_CITY,
        [{"color": "green", "width": 2, "alpha": 0.5, "label": "Dijkstra"}],
        avoid="London",
        save_filename="dijkstra_route_map.png",
    )


if __name__ == "__main__":
    main()
