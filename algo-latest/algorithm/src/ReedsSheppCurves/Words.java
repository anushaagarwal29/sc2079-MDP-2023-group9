package ReedsSheppCurves;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.lang.Math;

public class Words implements Cloneable{
    private ArrayList<Letter> lettersArray;
    private int length;

    public Words(){
        this.lettersArray = new ArrayList<Letter>();
    }

    public Words(Words w){
        this.lettersArray = w.get_lettersArray();
    }

    public void add_letter(Letter L){
        this.lettersArray.add(L);
        this.length++;
    }

    public double word_length(){
        if (this.lettersArray.size() == 0){
            return 0;
        } else {
            double temp = 0;
            for (int i = 0; i < this.lettersArray.size(); i++){
                temp += Math.abs(this.lettersArray.get(i).get_param());
            }
            return temp;
        }
    }

    public ArrayList<Letter> get_lettersArray(){
        return this.lettersArray;
    }

    public void set_lettersArray(ArrayList<Letter> l){
        this.lettersArray = l;
    }

    public Words timeflip() throws CloneNotSupportedException{
        //interchanges + and - in a word, which results in (x, y, phi) to (-x, y, -phi)\
        Words new_words = new Words();
        //System.out.println("within timeflip --------");
        //System.out.println("original content: " + this.toString());
        ArrayList<Letter> clonedlist = new ArrayList<Letter>();
        //Collections.copy(clonedlist, this.lettersArray);
        try {
            for (Letter l:this.lettersArray){
                clonedlist.add((Letter) l.clone());
            }
        } catch (CloneNotSupportedException cnse){

        }
        // try {
            //new_words = (Words)this.clone();
        new_words.set_lettersArray(clonedlist);
        for (int i = 0; i < new_words.get_lettersArray().size(); i++){
            //System.out.println("Before flipping: " + new_words.get_lettersArray().get(i).toString());
            new_words.get_lettersArray().get(i).reverse_gear();
            //System.out.println("After flipping: " + new_words.get_lettersArray().get(i).toString());

        }
        // System.out.println(new_words.toString());
        // System.out.println("original content: " + this.toString());
        // System.out.println("end of flipping --------");
        return new_words;
        // } catch (CloneNotSupportedException cnse){

        // }
        // for (int i = 0; i < new_words.get_lettersArray().size(); i++){
        //     new_words.get_lettersArray().get(i).reverse_gear();
        // }
        // return new_words;
        //return new_words;
    }

    public Words copy_constructor(Words word) throws CloneNotSupportedException{
        Words new_words = new Words();
        new_words = (Words)this.clone();
        return new_words;
    }



    public Words reflect() throws CloneNotSupportedException{
        Words new_words = new Words();
        ArrayList<Letter> clonedlist = new ArrayList<Letter>();
        
        try {
            for (Letter l:this.lettersArray){
                clonedlist.add((Letter) l.clone());
            }
        } catch (CloneNotSupportedException cnse){

        }
        new_words.set_lettersArray(clonedlist);
        for (int i = 0; i < new_words.get_lettersArray().size(); i++){
            new_words.get_lettersArray().get(i).reverse_steering();
        }
        return new_words;

        
    }

    public String toString(){
        String info_str = "";
        int length = this.get_lettersArray().size();
        for (int i = 0; i < length; i++){
            info_str += this.get_lettersArray().get(i).toString();
            info_str += "\n";
            //System.out.println(this.get_words().get(i).toString());
        }
        return info_str;
    }
}
