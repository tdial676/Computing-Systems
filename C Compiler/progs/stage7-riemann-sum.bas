    # Integrates x^2 from 0 to 1

LET S = 1000000
LET I = 0
LET K = 0
WHILE K < S
    LET K = K + 1
    LET I = I + K * K
END WHILE
    # The numerator and denominator of a fraction that approximates the integral
    # (which evaluates to 1 / 3 exactly)
#333333
#1000000
PRINT I / (S * S)
PRINT S
