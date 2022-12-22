public class IntArraysPart3 {
    public static void main(String[] args) {
        int[] array = new int[4];

        System.out.println(array[0]);
        System.out.println(array[1]);
        System.out.println(array[2]);
        System.out.println(array[3]);

        System.out.println(array[0]);
        array[1] = 5;
        System.out.println(array[1]);
        System.out.println(array[2]);
        array[3] = 60;
        System.out.println(array[3]);

        array[0] = 90;
        array[2] = 150;
        System.out.println(array[0]);
        System.out.println(array[1]);
        System.out.println(array[2]);
        System.out.println(array[3]);
    }
}
