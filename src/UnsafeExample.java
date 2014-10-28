import java.lang.reflect.Field;
import sun.misc.Unsafe;

/* thanks / apologies to Mishadoff
 * http://mishadoff.com/blog/java-magic-part-4-sun-dot-misc-dot-unsafe/ 
 */

class UnsafeExample
{
    private Field f;
    private Unsafe unsafe;

    public UnsafeExample() throws NoSuchFieldException, IllegalAccessException
    {
        // Use Java reflection to set up use of sun.misc.Unsafe
        f = Unsafe.class.getDeclaredField("theUnsafe");
        f.setAccessible(true);
        unsafe = (Unsafe) f.get(null);

        tester();
    }

    public void tester() throws NoSuchFieldException, IllegalAccessException
    {
        int s = unsafe.pageSize();
        System.out.println("The page size is " + s);
    }

    public static void main(String[] args) throws NoSuchFieldException, IllegalAccessException
    {
        UnsafeExample x = new UnsafeExample();
    }
}
