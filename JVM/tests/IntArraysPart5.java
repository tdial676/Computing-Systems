public class IntArraysPart5 {
    public static void main(String[] args) {
        for (int i = 0; i < 5; i++) {
            printArray(makeArray(i));
        }
    }

    public static void printArray(int[] array) {
        for (int i = 0; i < array.length; i++) {
            System.out.println(array[i]);
        }
    }

    public static int[] makeArray(int size) {
        int[] a = new int[size];
        for (int i = 0; i < size; i++) {
            a[i] = i;
        }
        return a;
    }
}
