def SubarraySumEqualsK(nums, k):
    count = 0
    prefix_sum = 0
    freq = {0: 1}  # prefix sum 0 appears once
    pairs = []
    i = 0

    for num in nums:
        prefix_sum += num
        if prefix_sum == k:
            pairs.append((0, i))  # push_back a pair
            print(f"i = {i}")
            # continue
        # Check if there is a previous prefix that forms a subarray sum = k
        elif prefix_sum - k in freq:
            count += freq[prefix_sum - k]
            print(f"i = {i}")
            pairs.append((0, i))
        # Update the frequency of the current prefix sum
        freq[prefix_sum] = freq.get(prefix_sum, 0) + 1
        i += 1
    return count

nums = [1, 2, 3, -1, 1, 2]
k = 3
print(SubarraySumEqualsK(nums, k))  # Output: 4
