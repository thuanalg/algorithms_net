spy_freq = {}  # prefix sum 0 appears once
def spy_FindDuplicatesArray_Hash(vec):
    for v in vec:
        print(v)
        spy_freq[v] = spy_freq.get(v, 0) + 1
    print(spy_freq)

vec = [10, 20, 30, 40, 10, 1, 0]
spy_FindDuplicatesArray_Hash(vec)