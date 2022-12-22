public class Test {
    public static void main(String[] args) {
        int a = 0xC0FFEE;
        for (int i = 0; i < 64; i++) {
            System.out.println(a << i);
        }
    }
}