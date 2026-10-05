# 1a
def __repr__(self):
    parts = []
    for i in range(self.n):
        parts.append(repr(self.data_arr[i]))
    return "[" + ", ".join(parts) + "]"


# 1b
def __add__(self, other):
    result = ArrayList()
    result.extend(self)
    result.extend(other)
    return result


# 1c
def __iadd__(self, other):
    self.extend(other)
    return self


# 1d
def __getitem__(self, ind):
    if ind < 0:
        ind += self.n
    if not (0 <= ind <= self.n - 1):
        raise IndexError("invalid index")
    return self.data_arr[ind]


def __setitem__(self, ind, val):
    if ind < 0:
        ind += self.n
    if not (0 <= ind <= self.n - 1):
        raise IndexError("invalid index")
    self.data_arr[ind] = val


# 1e.
def __mul__(self, k):
    result = ArrayList()
    for _ in range(k):
        result.extend(self)
    return result


# 1f
def __rmul__(self, k):
    return self * k


# 1g
def __init__(self, iter_collection=None):
    self.data_arr = make_array(1)
    self.capacity = 1
    self.n = 0
    if iter_collection is not None:
        self.extend(iter_collection)


# 1h
def remove(self, val):
    for i in range(self.n):
        if self.data_arr[i] == val:
            for j in range(i, self.n - 1):
                self.data_arr[j] = self.data_arr[j + 1]
            self.data_arr[self.n - 1] = None
            self.n -= 1
            return
    raise ValueError("value not in list")


# 1i
def removeOdds(self):
    write = 0
    for read in range(self.n):
        if self.data_arr[read] % 2 == 0:
            self.data_arr[write] = self.data_arr[read]
            write += 1
    for i in range(write, self.n):
        self.data_arr[i] = None
    self.n = write


# 2a
def find_pivot(lst):
    if len(lst) == 0:
        return None
    left, right = 0, len(lst) - 1
    while left < right:
        mid = (left + right) // 2
        if lst[mid] > lst[right]:
            left = mid + 1
        else:
            right = mid
    return left


# 2b
def binary_search(lst, target, low, high):
    while low <= high:
        mid = (low + high) // 2
        if lst[mid] == target:
            return mid
        elif lst[mid] < target:
            low = mid + 1
        else:
            high = mid - 1
    return None


def shift_binary_search(lst, target):
    if len(lst) == 0:
        return None
    p = find_pivot(lst)
    last = len(lst) - 1
    if lst[p] <= target <= lst[last]:
        return binary_search(lst, target, p, last)  # right piece
    else:
        return binary_search(lst, target, 0, p - 1)  # left piece


# 3
def LSRP(s):
    last_seen = [-1] * 26
    start = 0
    best = 0
    for i in range(len(s)):
        c = ord(s[i]) - ord("a")
        if last_seen[c] >= start:
            start = last_seen[c] + 1
        last_seen[c] = i
        best = max(best, i - start + 1)
    return best
