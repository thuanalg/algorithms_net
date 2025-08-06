def spy_SubarraySumEqualsK_print(nums, pairs):
    print("nums :", nums)
    print("Pairs (start, end):", pairs)
    for start, end in pairs:
        subarray = nums[start:end+1]
        print(f"From {start} to {end} => {subarray}")

def spy_SubarraySumEqualsK(nums, k):
    count = 0
    prefix_sum = 0
    freq = {}  # prefix sum 0 appears once
    pairs = []
    i = 0

    for num in nums:
        prefix_sum += num
        if prefix_sum == k:
            pairs.append((0, i))  # push_back a pair
            #print(f"i = {i}")
            # continue
        # Check if there is a previous prefix that forms a subarray sum = k
        elif prefix_sum - k in freq:
            count += freq[prefix_sum - k]
            #print(f"i = {i}")
            j = i
            sumj = 0
            while j >= 0:
                sumj += nums[j]
                if sumj == k:
                    pairs.append((j, i))
                    break
                j -= 1
        # Update the frequency of the current prefix sum
        freq[prefix_sum] = freq.get(prefix_sum, 0) + 1
        i += 1
    print("Pairs (start, end):", pairs)
    print("freq (key, value):", freq)
    spy_SubarraySumEqualsK_print(nums, pairs)
    return pairs

nums = [1, 2, 3, -1, 1, 2]
k = 6
print(spy_SubarraySumEqualsK(nums, k))  # Output: 4
