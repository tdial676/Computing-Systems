public class MergeSort {
    public static void main(String[] args) {
        int[] x = { 5, 1, 6, 2, 3, 4 };
        mergeSort(x, x.length);

        for (int i = 0; i < x.length; i++) {
            System.out.println(x[i]);
        }
    }

    public static void merge(int[] x, int[] l, int[] r) {
        int i = 0;
        int j = 0;
        int k = 0;
        while (i < l.length && j < r.length) {
            if (l[i] <= r[j]) {
                x[k] = l[i];
                i++;
            } else {
                x[k] = r[j];
                j++;
            }
            k++;
        }
        while (i < l.length) {
            x[k] = l[i];
            k++;
            i++;
        }
        while (j < r.length) {
            x[k] = r[j];
            k++;
            j++;
        }
    }

    public static void mergeSort(int[] x, int n) {
        if (n < 2) {
            return;
        }
        int m = n / 2;
        int[] l = new int[m];
        int[] r = new int[n - m];

        for (int i = 0; i < m; i++) {
            l[i] = x[i];
        }
        for (int i = m; i < n; i++) {
            r[i - m] = x[i];
        }
        mergeSort(l, m);
        mergeSort(r, n - m);

        merge(x, l, r);
    }
}
