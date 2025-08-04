def sp_greedy_recursive(mat, island, i, j, mtrace):
    direction = [ [0, 1], [0, -1],  [1, 0], [-1, 0] ]
    print(direction)
    for x, y in direction:
        u = x + i
        v = y + j
        if u < 0 or v < 0:
            continue
        if u >= len(mat) or v >= len(mat[0]):
            continue
        if mat[u][v] == 0:
            continue
        if mtrace[u][v]:
            continue        
        island.append((u,v))
        mtrace[u][v] = True
        sp_greedy_recursive(mat, island, u, v, mtrace)

def sp_greedy(mat, ouput):
    m = len(mat)
    n = len(mat[0])
    mtrace = [[False for _ in range(n)] for _ in range(m)]
    for i in range(m):
        for j in range(n):
            if mat[i][j] == 0:
                continue
            if mtrace[i][j] :
                continue
            mtrace[i][j] = True
            island = []
            island.append((i,j))
            sp_greedy_recursive(mat, island, i, j, mtrace);
            output.append(island)
    sorted_output = sorted(output, key=len)
    for row in sorted_output:
        print(row)
    print("hello")


mat = [
    [0, 1, 0, 0, 1],
    [1, 0, 1, 0, 0],
    [0, 0, 1, 1, 0],
    [1, 0, 0, 1, 0],
    [0, 1, 1, 0, 0]]

#island = []
output = []
sp_greedy(mat, output)

#for row in mat:
#    print(row)