package map2;

import java.awt.*;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;

import ReedsSheppCurves.PathGenerator;
import ReedsSheppCurves.Words;



public class demo {


     public static Turtle initTurtle(){
        Turtle.setCanvasSize(200, 200);
        Turtle turtle = new Turtle();
        turtle.speed(0);
        turtle.shape("arrow");
        turtle.shapeSize(3, 9);

        return turtle;
     }


     public static void drawNodes(map map, Turtle turtle){
        for (double[] node:map.getNodeList()){
            draw.goTo(turtle, node, true);
            draw.drawVector(turtle);
        }
     }

     public static void drawAllPaths(map map, Turtle turtle) throws IllegalAccessException, InvocationTargetException, CloneNotSupportedException{
        turtle.speed(5);
        int j = 1;
        ArrayList<Words> paths = new ArrayList<Words>();
        for (int i = 0; i < map.getNodeList().size() - 1; i++){
            try {
                paths = PathGenerator.get_all_words(map.getNodeList().get(i), map.getNodeList().get(i + 1));
            } catch (IllegalAccessException iae){}
            catch (InvocationTargetException ite){}
            catch (CloneNotSupportedException cnse){}

            for (Words w:paths){
                System.out.println("drawing word " + j);
                j++;
                draw.setRandomColour(turtle);
                draw.goTo(turtle, map.getNodeList().get(i), true);
                draw.draw_path(turtle, w);
            }
        }
    }
     

     public static ArrayList<Words> drawOptimalPath(Turtle turtle, map map){
        ArrayList<Words> optimalPath = new ArrayList<Words>();
        Words path = new Words();
        turtle.penColor("red");
        turtle.width(3);
        turtle.speed(10);
        draw.goTo(turtle, map.getNodeList().get(0), true);
        double path_length = 0;
        for (int i = 0; i < map.getNodeList().size() - 1; i++){
            try {
                path = PathGenerator.get_optimal_word(map.getNodeList().get(i), map.getNodeList().get(i+1));
            } catch (IllegalAccessException iae){}
            catch (InvocationTargetException ite){}
            catch (CloneNotSupportedException cnse){}
            optimalPath.add(path);
            path_length += path.word_length();
            draw.draw_path(turtle, path);
        }
        return optimalPath;
     }

     public static void main(String args[]) throws IllegalAccessException, InvocationTargetException, CloneNotSupportedException{
        map map = new map();
        Turtle turtle = initTurtle();
        // try {
        ArrayList<double[]> paths = map.generatePathNodes();
        // } catch (InvocationTargetException ite){}
        // catch (IllegalAccessException iae){}
        // catch (CloneNotSupportedException cnse){}
        map.generatePathNodes();
        draw.drawOutline(turtle);
        drawNodes(map, turtle);
        drawAllPaths(map, turtle);
        drawOptimalPath(turtle, map);
        //turtle.exit();
     } 
    
}
