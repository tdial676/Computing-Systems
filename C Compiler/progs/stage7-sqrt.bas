    # Computes the square root of a number
#1834817011

LET A = 3366553463854974121

    # Determine the largest power of 2 whose square fits in a 64-bit integer
LET B = 1
WHILE B * B * 4 > 0 # continue until (B * 2) ** 2 overflows
    LET B = B * 2
END WHILE

    # Add powers of 2 to Z, making sure its square is at most A
LET Z = 0
WHILE B > 0
    LET Y = Z + B
    IF Y * Y > A
    ELSE
        LET Z = Y
    END IF
    LET B = B / 2
END WHILE

PRINT Z
