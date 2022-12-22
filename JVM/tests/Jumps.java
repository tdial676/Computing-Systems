public class Jumps {
    public static void main(String[] args) {
        for (int i = -5; i <= 5; i++) {
            System.out.println(signum(i));
        }
        for (int i = -3; i <= 3; i++) {
            int j = -3;
            do {
                System.out.println(min(i, j));
                System.out.println(max(i, j));
            } while (j++ < 3);
        }
        for (int i = 0; i != 11; i++) {
            System.out.println(catalan(i));
        }

        // Check overflowing comparisons
        System.out.println(max(-2147483648, 2147483647));
        System.out.println(max(1234112124, -2020202020));
    }

    public static int signum(int n) {
        if (n < 0) return -1;
        if (n > 0) return +1;
        return 0;
    }

    public static int min(int a, int b) {
        return b - a >= 0 ? a : b;
    }
    public static int max(int a, int b) {
        return a > b ? a : b;
    }

    public static int catalan(int n) {
        if (n == 0) return 1;

        int sum = 0, i = 0;
        do {
            sum += catalan(i++) * catalan(--n);
        } while (n > 0);
        return sum;
    }
}