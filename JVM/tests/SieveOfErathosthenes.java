public class SieveOfErathosthenes {
    public static void main(String[] args) {
        // generate primes up until 2500
        int sqrtNum = 50;
        int num = sqrtNum * sqrtNum;

        int[] prime = new int[num];
        // 0 is true, 1 is false for convenience
        for (int i = 2; i < sqrtNum; i++) {
            if (prime[i] == 0) {
                for (int j = i * i; j < num; j = j + i) {
                    prime[j] = 1;
                }
            }
        }
        for (int i = 2; i < prime.length; i++) {
            if (prime[i] == 0) {
                System.out.println(i);
            }
        }
    }
}