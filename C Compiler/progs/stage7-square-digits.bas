    # Taken from https://projecteuler.net/problem=92
#8581146

LET C = 0
LET X = 2
WHILE X < 10000000
    LET Z = X
    LET A = 0
    WHILE A = 0
        LET Y = Z
        LET S = 0
        WHILE Y > 0
            LET Z = Y / 10
            LET D = Y - Z * 10
            LET S = S + D * D
            LET Y = Z
        END WHILE
        IF S = 1
            LET A = 1
        ELSE
            IF S = 89
                LET C = C + 1
                LET A = 1
            ELSE
                LET Z = S
            END IF
        END IF
    END WHILE
    LET X = X + 1
END WHILE

PRINT C
