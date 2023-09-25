package ReedsSheppCurves;
import java.lang.Math;
import java.lang.reflect.InvocationTargetException;

public class ReedsSheppCurves {
    

    public static void main(String args[]) throws IllegalAccessException, InvocationTargetException, CloneNotSupportedException{
        Util utils = new Util();
        Letter L = new Letter(18.2432, Steering.RIGHT, Gear.FORWARD);
        Letter L2 = new Letter(-10, Steering.LEFT, Gear.FORWARD);
        Letter L3 = new Letter(-20, Steering.RIGHT, Gear.BACKWARD);
        L.toString();
        System.out.println(L.toString());
        L.reverse_steering();
        System.out.println(L.toString());
        L.reverse_gear();
        System.out.println(L.toString());
        Words W = new Words();
        W.add_letter(L);
        W.add_letter(L2);
        W.add_letter(L3);
        System.out.println(W.word_length());
        PathGenerator pg = new PathGenerator();
        double[] p1 = {300, 20, 36};
        double[] p2 = {20.11, 13.23, 21.55};

        try {
            pg.get_optimal_word(p1, p2);
        } catch (CloneNotSupportedException cnse){

        }
        // System.out.println(" ");
        // Words testWords = new Words();
        // testWords.add_letter(L);
        // testWords.add_letter(L2);
        // testWords.add_letter(L3);
        // System.out.println("init words: " + testWords.toString());
        // Words flipped = testWords.timeflip();
        // //System.out.println(flipped.toString());
        // Words reflected = testWords.reflect();
        // System.out.println("original " + testWords.toString());
        // System.out.println(" ");
        // System.out.println("flipped " + flipped.toString());
        // System.out.println(" ");
        // System.out.println("reflected " + reflected.toString());
        // System.out.println(" ");
        // System.out.println("test reverse");
        // System.out.println(L.toString());
        // L.reverse_gear();
        // System.out.println(L.toString());        
        
    }

}
