package jdi.demo;

public class Hello {

    /**
     * java -Xdebug -agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=8800 -cp . jdi.demo.Hello
     * @param args
     */
    public static void main(String[] args) {
        String str = "Hello world!";
        System.out.println(str);
        new Thread(() -> {
        Hello test = new Hello();
        while (true) {
            try {
                Thread.sleep(3000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            test.printHello();
        }
        }).start();
    }

    protected void printHello() {
        System.out.println("hello");
    }

}
