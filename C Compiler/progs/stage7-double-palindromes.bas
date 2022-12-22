    # Taken from https://projecteuler.net/problem=36
#872187

LET R = 0
LET X = 1
WHILE X < 1000000
    LET Y = X
    LET E = 0
    WHILE Y > 0
        LET Z = Y / 10
        LET E = E * 10 + Y - Z * 10
        LET Y = Z
    END WHILE

    IF E = X
        LET Y = X
        LET E = 0
        WHILE Y > 0
            LET Z = Y / 2
            LET E = E * 2 + Y - Z * 2
            LET Y = Z
        END WHILE
        IF E = X
            LET R = R + X
        END IF
    END IF
    LET X = X + 2 # only odd numbers can be palindromic in base 2
END WHILE

PRINT R
