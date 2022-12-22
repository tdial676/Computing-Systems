// Alternative take on https://projecteuler.net/problem=31
// Uses recursions + int arrays
public class CoinSumsAlternate {
    public static void main(String[] args) {
        int coins[] = {1, 2, 5, 10, 20, 50, 100, 200};
        System.out.println(count(coins, coins.length, 200));
    }

    static int count(int[] coins, int m, int n) {
        if (n == 0) {
            // Valid coin sum
            return 1;
        } else if (n < 0 || m <= 0 && n >= 1) {
            // Invalid coin sum
            return 0;
        } else {
            // Use the coin or do not use the coin to make change
            return count(coins, m - 1, n) + count(coins, m, n - coins[m - 1]);
        }
    }
}
