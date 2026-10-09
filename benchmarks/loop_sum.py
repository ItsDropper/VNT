# Same nested while-loop workload as the VNT and C benchmark.
round = 0
while round < 20:
    i = 0
    total = 0
    while i < 40000:
        total = total + i
        i = i + 1
    round += 1

print(total)
