def sp_greedy_recursive(mat, island, i, j, mtrace):
    direction = [ [0, 1], [0, -1],  [1, 0], [-1, 0] ]
    print(direction)

def sp_greedy(mat, ouput):
    m = len(mat)
    n = len(mat[0])
    mtrace = [[False for _ in range(n)] for _ in range(m)]
    for row in mat:
        print(row)

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