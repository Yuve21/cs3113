# 1
def reverse_list(lst):
    low = 0
    high = len(lst) - 1
    while low < high:
        lst[low], lst[high] = lst[high], lst[low]
        low += 1
        high -= 1


# 2
def reverse_list(lst, low=None, high=None):
    if low is None:
        low = 0
    if high is None:
        high = len(lst) - 1
    while low < high:
        lst[low], lst[high] = lst[high], lst[low]
        low += 1
        high -= 1


# 3a
def move_zeros(nums):
    write = 0
    for read in range(len(nums)):
        if nums[read] != 0:
            nums[write], nums[read] = nums[read], nums[write]
            write += 1


# 3b
def move_zeros(nums, target=0):
    write = 0
    for read in range(len(nums)):
        if nums[read] != target:
            nums[write], nums[read] = nums[read], nums[write]
            write += 1


# 3c
def remove_target_elem(lst, target):
    write = 0
    for read in range(len(lst)):
        if lst[read] != target:
            lst[write] = lst[read]
            write += 1
    while len(lst) > write:
        lst.pop()


# 4
def search_range(lst, target):
    first = find_bound(lst, target, True)
    if first == -1:
        return (-1, -1)
    last = find_bound(lst, target, False)
    return (first, last)


def find_bound(lst, target, find_first):
    low = 0
    high = len(lst) - 1
    result = -1
    while low <= high:
        mid = (low + high) // 2
        if lst[mid] == target:
            result = mid
            if find_first:
                high = mid - 1
            else:
                low = mid + 1
        elif lst[mid] < target:
            low = mid + 1
        else:
            high = mid - 1
    return result
