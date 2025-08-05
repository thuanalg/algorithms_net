import sys
import heapq
from queue import PriorityQueue

INF = sys.maxsize
def spy_dijkstra(grid, s, parent, output):
    print("--")
    n = len(grid)
    dist = [INF] * n
    visited = [False] * n
    pq = PriorityQueue()
    dist[s] = 0
    parent[s] = -1
    pq.put((0, s)) 
    while not pq.empty():
        d, u = pq.get()
        print(d, u)
        if visited[u]:
            continue
        visited[u] = True
        for v in range(n):
            if grid[u][v] == INF:
                continue
            if dist[v] > dist[u] + grid[u][v]:
                dist[v] = dist[u] + grid[u][v]
                parent[v] = u
                pq.put((dist[v], v))
    output = dist
    print(parent)
    print(output)

def spy_dijkstra_test():
    grid = [
        [0, 1.2, 4, INF, INF, 10],     # 0
        [INF, 0, 1, INF, 1, INF],      # 1
        [INF, INF, 0, 1, 1, 12],       # 2
        [INF, INF, INF, 0, INF, 2],    # 3
        [INF, INF, INF, 10, 0, 6],     # 4
        [INF, INF, INF, INF, INF, 0]   # 5
    ]

    parent = [-1] * len(grid)
    output = [INF] * len(grid)
    spy_dijkstra(grid, 0, parent, output)

spy_dijkstra_test()
