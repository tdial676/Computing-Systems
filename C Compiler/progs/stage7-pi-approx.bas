    # Approximates pi to 16 decimal digits.
    # Uses the BPB formula: https://en.wikipedia.org/wiki/Bailey%E2%80%93Borwein%E2%80%93Plouffe_formula.

LET N = 0 # we represent the result as a fraction N / D
LET D = 1000000000000000000 # D is a large power of 10
LET S = D * 4
LET K = 0
WHILE K < 14 # we only use the first 14 terms of this infinite series
    LET E = K * 8 + 1
    LET F = K * 16 + 8
    LET G = K * 32 + 20
    LET H = K * 32 + 24
    LET N = N + S / E - S / F - S / G - S / H
    LET S = S / 16
    LET K = K + 1
END WHILE

    # Extract the digits from the fraction
WHILE D > 0
    LET A = N / D
    PRINT A
    LET N = N - A * D
    LET D = D / 10
END WHILE

#3
#1
#4
#1
#5
#9
#2
#6
#5
#3
#5
#8
#9
#7
#9
#3
#2
#4
#8
