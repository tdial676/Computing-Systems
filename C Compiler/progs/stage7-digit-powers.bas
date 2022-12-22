    # Taken from https://projecteuler.net/problem=30
#443839

LET R = 0
LET X = 2
WHILE X < 1000000
    LET S = 0
    LET Y = X
    WHILE Y > 0
        LET Z = Y / 10
        LET D = Y - Z * 10
        LET E = D * D
        LET S = S + E * E * D
        LET Y = Z
    END WHILE
    IF S = X
        LET R = R + X
    END IF
    LET X = X + 1
END WHILE

PRINT R
