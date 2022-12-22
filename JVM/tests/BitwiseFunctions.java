public class BitwiseFunctions {
    public static void main(String[] args) {
        for (int a = -100; a <= 100; a++) {
            for (int b = -100; b <= 100; b++) {
                System.out.println(add(a, b));
            }
        }

        for (int x = -100; x <= 100; x++) {
            System.out.println(oneBits(x));
        }
    }

    public static int add(int a, int b) {
        while (b != 0) {
            int carry = a & b;
            a ^= b;
            b = carry << 1;
        }
        return a;
    }

    public static int oneBits(int x) {
        int bits = 0;
        while (x != 0) {
            bits += x & 1;
            x >>>= 1;
        }
        return bits;
    }
}
