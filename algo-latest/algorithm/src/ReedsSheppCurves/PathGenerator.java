package ReedsSheppCurves;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;

public class PathGenerator {
    public static ArrayList<Words> get_all_words(double[] p1, double[] p2) throws IllegalAccessException, InvocationTargetException, CloneNotSupportedException{
        Util utils = new Util();
        double[] params = new double[3];
        params = utils.normalise_basis(p1, p2);
        System.out.println(params[0]+ ", " +  params[1]+ ", " +  params[2]);
        WordsGenerator wg = new WordsGenerator();
        ArrayList<Words> wordlist = new ArrayList<Words>();

        Words word1 = wg.word_cluster_1(params[0], params[1], params[2]);
        Words word1_toflip = wg.word_cluster_1(-params[0], params[1], -params[2]);
        Words word1_toreflect = wg.word_cluster_1(params[0], -params[1], -params[2]);
        Words word1_toreflectflip = wg.word_cluster_1(-params[0], -params[1], params[2]);
        wordlist.add(word1);
        wordlist.add(word1_toflip.timeflip());
        wordlist.add(word1_toreflect.reflect());
        wordlist.add(word1_toreflectflip.timeflip().reflect());

        Words word2 = wg.word_cluster_2(params[0], params[1], params[2]);
        Words word2_toflip = wg.word_cluster_2(-params[0], params[1], -params[2]);
        Words word2_toreflect = wg.word_cluster_2(params[0], -params[1], -params[2]);
        Words word2_toreflectflip = wg.word_cluster_2(-params[0], -params[1], params[2]);
        wordlist.add(word2);
        wordlist.add(word2_toflip.timeflip());
        wordlist.add(word2_toreflect.reflect());
        wordlist.add(word2_toreflectflip.timeflip().reflect());

        Words word3 = wg.word_cluster_3(params[0], params[1], params[2]);
        Words word3_toflip = wg.word_cluster_3(-params[0], params[1], -params[2]);
        Words word3_toreflect = wg.word_cluster_3(params[0], -params[1], -params[2]);
        Words word3_toreflectflip = wg.word_cluster_3(-params[0], -params[1], params[2]);
        wordlist.add(word3);
        wordlist.add(word3_toflip.timeflip());
        wordlist.add(word3_toreflect.reflect());
        wordlist.add(word3_toreflectflip.timeflip().reflect());

        // Words word2 = wg.word_cluster_2(params[0], params[1], params[2]);
        // Words word3 = wg.word_cluster_3(params[0], params[1], params[2]);
        // Words word4 = wg.word_cluster_4(params[0], params[1], params[2]);
        // Words word5 = wg.word_cluster_5(params[0], params[1], params[2]);
        // Words word6 = wg.word_cluster_6(params[0], params[1], params[2]);
        // Words word7 = wg.word_cluster_7(params[0], params[1], params[2]);
        // Words word8 = wg.word_cluster_8(params[0], params[1], params[2]);
        // Words word9 = wg.word_cluster_9(params[0], params[1], params[2]);
        // Words word10 = wg.word_cluster_10(params[0], params[1], params[2]);
        // Words word11 = wg.word_cluster_11(params[0], params[1], params[2]);
        // Words word12 = wg.word_cluster_12(params[0], params[1], params[2]);
        
        // System.out.println("within path generator");
        // System.out.println(word1.toString()); 
        // System.out.println(" ");
        // System.out.println(word1_toflip.toString()); 
        // System.out.println(" ");
        // System.out.println(word1_toreflect.toString());
        // System.out.println(" ");
        // System.out.println(word1_toreflectflip.toString());
        // System.out.println("end of path generator");
        wordlist.add(word1);
        wordlist.add(word1_toflip.timeflip());
        wordlist.add(word1_toreflect.reflect());
        wordlist.add(word1_toreflectflip.timeflip().reflect());
        System.out.println("out of path gen");
        // wordlist.add(word2);
        // wordlist.add(word2.timeflip());
        // wordlist.add(word2.reflect());
        // wordlist.add(word2.timeflip().reflect());

        // wordlist.add(word3);
        // wordlist.add(word3.timeflip());
        // wordlist.add(word3.reflect());
        // wordlist.add(word3.timeflip().reflect());

        // wordlist.add(word4);
        // wordlist.add(word4.timeflip());
        // wordlist.add(word4.reflect());
        // wordlist.add(word4.timeflip().reflect());

        // wordlist.add(word5);
        // wordlist.add(word5.timeflip());
        // wordlist.add(word5.reflect());
        // wordlist.add(word5.timeflip().reflect());
        
        // wordlist.add(word6);
        // wordlist.add(word6.timeflip());
        // wordlist.add(word6.reflect());
        // wordlist.add(word6.timeflip().reflect());

        // wordlist.add(word7);
        // wordlist.add(word7.timeflip());
        // wordlist.add(word7.reflect());
        // wordlist.add(word7.timeflip().reflect());

        // wordlist.add(word8);
        // wordlist.add(word8.timeflip());
        // wordlist.add(word8.reflect());
        // wordlist.add(word8.timeflip().reflect());

        // wordlist.add(word9);
        // wordlist.add(word9.timeflip());
        // wordlist.add(word9.reflect());
        // wordlist.add(word9.timeflip().reflect());

        // wordlist.add(word10);
        // wordlist.add(word10.timeflip());
        // wordlist.add(word10.reflect());
        // wordlist.add(word10.timeflip().reflect());

        // wordlist.add(word11);
        // wordlist.add(word11.timeflip());
        // wordlist.add(word11.reflect());
        // wordlist.add(word11.timeflip().reflect());

        // wordlist.add(word12);
        // wordlist.add(word12.timeflip());
        // wordlist.add(word12.reflect());
        // wordlist.add(word12.timeflip().reflect());

        for (int i = 0; i < wordlist.size(); i++){
            System.out.println(wordlist.get(i).toString()); 
            System.out.println("\n");
        }

        return wordlist;

    }

    public static Words get_optimal_word(double[] p1, double[] p2) throws IllegalAccessException, InvocationTargetException, CloneNotSupportedException{
        ArrayList<Words> wordlist = get_all_words(p1, p2);
        int i_min = -1;
        Double L_min = Double.POSITIVE_INFINITY;
        Double L;
        int i;
        for (i = 0; i < wordlist.size(); i++){
            L = wordlist.get(i).word_length();
            System.out.println(i + "'s length is " + L);

            if (L == 0.0){
                continue;
            }
            if (L <= L_min){
                i_min = i;
                L_min = L;
            }
        }
        System.out.println("imin is " + i_min);
        System.out.println(wordlist.get(i_min).toString());
        return wordlist.get(i_min);

    }
}
