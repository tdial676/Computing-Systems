    # Try all combinations of 6 lowercase letters
LET A = 97
WHILE A < 123
    LET B = 97
    WHILE B < 123
        LET C = 97
        WHILE C < 123
            LET D = 97
            WHILE D < 123
                LET E = 97
                WHILE E < 123
                    LET F = 97
                    WHILE F < 123
                        # Compute the hash
                        LET O = 0
                        LET P = O * 32 - O + A
                        LET Q = P * 32 - P + B
                        LET R = Q * 32 - Q + C
                        LET S = R * 32 - R + D
                        LET T = S * 32 - S + E
                        LET U = T * 32 - T + F
                        # Convert to a 32-bit integer
                        LET V = U * 4294967296 / 4294967296
                        # If the hash is correct, print the character values and exit
                        IF V = -1224424666 # hash computed using String.hashCode() in Java
                            PRINT A
                            PRINT B
                            PRINT C
                            PRINT D
                            PRINT E
                            PRINT F
                            LET A = 1000
                            LET B = 1000
                            LET C = 1000
                            LET D = 1000
                            LET E = 1000
                            LET F = 1000
                        END IF
                        LET F = F + 1
                    END WHILE
                    LET E = E + 1
                END WHILE
                LET D = D + 1
            END WHILE
            LET C = C + 1
        END WHILE
        LET B = B + 1
    END WHILE
    LET A = A + 1
END WHILE

#104
#97
#115
#104
#109
#101
