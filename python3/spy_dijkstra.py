import sys
import heapq

INF = sys.maxsize
def spy_dijkstra(grid, i, parent, output):
    print("--")
    n = len(grid)
    dist = [INF] * n
    pq = []

def spy_dijkstra_test():
    grid = [
        [0, 1.2, 4, INF, INF, 10],     # 0
        [INF, 0, 1, INF, 1, INF],      # 1
        [INF, INF, 0, 1, 1, 12],       # 2
        [INF, INF, INF, 0, INF, 2],    # 3
        [INF, INF, INF, 10, 0, 6],     # 4
        [INF, INF, INF, INF, INF, 0]   # 5
    ]
    
    for row in grid:
        print(row)
    parent = []
    output = []
    spy_dijkstra(grid, 0, parent, output)

spy_dijkstra_test()
