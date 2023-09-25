package map2;

import java.awt.Color;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import ReedsSheppCurves.Gear;
import ReedsSheppCurves.Letter;
import ReedsSheppCurves.Steering;
import ReedsSheppCurves.Util;
import ReedsSheppCurves.Words;

public class draw {
    // public double scale(){};
    public static double scale(double x) {
        double scale = 40;
        return x * scale;
    }

    public static void drawVector(Turtle turtle) {
        turtle.down();
        turtle.width(3);
        turtle.forward(scale(1.2));
        turtle.right(25);
        turtle.backward(scale(0.4));
        turtle.forward(scale(0.4));
        turtle.left(50);
        turtle.backward(scale(0.4));
        turtle.forward(scale(0.4));
        turtle.right(25);
        turtle.width(1);
        turtle.up();
    }

    public static void goTo(Turtle turtle, double[] pos, boolean scale_pos) {
        Util utils = new Util();
        turtle.up();
        if (scale_pos) {
            turtle.setPosition(scale(pos[0]), scale(pos[1]));
        } else {
            turtle.setPosition(pos[0], pos[1]);
        }
        turtle.setDirection(utils.rad2deg(pos[2]));
        turtle.down();
    }

    // path is a word
    public static void draw_path(Turtle turtle, Words word) {
        ArrayList<Letter> lettersArray = word.get_lettersArray();
        int gear;

        System.out.println("what im plotting: \n");
        System.out.println(word.toString());
        for (int i = 0; i < lettersArray.size(); i++) {
            Letter temp = lettersArray.get(i);
            Util utils = new Util();
            // if (temp.getGear() == Gear.FORWARD){
            // gear = 1;
            // } else {
            // gear = -1;
            // }

            // if (temp.getSteering() == Steering.LEFT){
            // draw_circle(turtle, scale(1), gear * utils.rad2deg(temp.getParam()));
            // } else if (temp.getSteering() == Steering.RIGHT){
            // draw_circle(turtle, - scale(1), gear * utils.rad2deg(temp.getParam()));
            // } else if (temp.getSteering() == Steering.STRAIGHT){
            // turtle.forward(gear * scale(temp.getParam()));
            // }
            Gear tempGear = temp.getGear();
            Steering tempSteering = temp.getSteering();
            if (tempGear == Gear.FORWARD && tempSteering == Steering.LEFT) {
                draw_circle(turtle, scale(1), utils.rad2deg(temp.getParam()), Steering.LEFT, Gear.FORWARD);
            } else if (tempGear == Gear.FORWARD && tempSteering == Steering.RIGHT) {
                draw_circle(turtle, scale(1), utils.rad2deg(temp.getParam()), Steering.RIGHT, Gear.FORWARD);
            } else if (tempGear == Gear.BACKWARD && tempSteering == Steering.LEFT) { // back left
                draw_circle(turtle, scale(1), utils.rad2deg(temp.getParam()), Steering.LEFT, Gear.BACKWARD);
            } else if (tempGear == Gear.BACKWARD && tempSteering == Steering.RIGHT) {
                draw_circle(turtle, scale(1), utils.rad2deg(temp.getParam()), Steering.RIGHT, Gear.BACKWARD);
            } else if (tempGear == Gear.FORWARD && tempSteering == Steering.STRAIGHT) {
                turtle.forward(scale(temp.getParam()));
            } else {
                turtle.backward(scale(temp.getParam()));
            }
        }
    }

    public static void setRandomColour(Turtle turtle) {
        double r, g, b;
        Random rd = new Random();
        r = g = b = 1;
        while (r + b + g > 2.5) {
            r = rd.nextDouble();
            g = rd.nextDouble();
            b = rd.nextDouble();
        }
        turtle.penColor("lightGray");
    }

    public static void draw_circle(Turtle turtle, double radius, double degrees, Steering steering, Gear gear) {
        Util utils = new Util();
        double nParts = 314;
        double radian = utils.deg2rad(degrees);
        double eachLength = radius * radian / nParts;
        double eachAngle = degrees / nParts;
        if (steering == Steering.LEFT) {
            if (gear == Gear.FORWARD) {
                for (int i = 0; i < nParts; i++) {
                    turtle.forward(eachLength);
                    turtle.left(eachAngle);
                }
            } else if (gear == Gear.BACKWARD) {
                for (int i = 0; i < nParts; i++) {
                    turtle.backward(eachLength);
                    turtle.right(eachAngle);
                }
            }
        } else if (steering == Steering.RIGHT) {
            if (gear == Gear.FORWARD) {
                for (int i = 0; i < nParts; i++) {
                    turtle.forward(eachLength);
                    turtle.right(eachAngle);
                }
            } else if (gear == Gear.BACKWARD) {
                for (int i = 0; i < nParts; i++) {
                    turtle.backward(eachLength);
                    turtle.left(eachAngle);
                }
            }
        }

    }

    public static void drawOutline(Turtle turtle) {
        int height = 800; // the height of the rectangle
        int width = 800; // the width of the rectangle
        for (int i = 0; i < 2; i++) // repeat the following twice
        {
            turtle.forward(height); // go forward the height of the rectangle
            turtle.left(90); // turn 90 degrees
            turtle.forward(width); // go forward the width of the rectangle
            turtle.left(90); // turn 90 degrees
        } // end loop
    }

    public static void draw_circle(Turtle turtle, double radius, double degrees) {
        Util utils = new Util();
        double Xstart = turtle.getX();
        double Ystart = turtle.getY();
        double H = turtle.getDirection();
        double nParts = 314;
        double radian = utils.deg2rad(degrees);
        double eachLength = radius * radian / nParts;
        double eachAngle = degrees / nParts;
        if (radius > 0) {
            if (degrees > 0) {
                for (int i = 0; i < nParts; i++) {
                    turtle.backward(eachLength);
                    turtle.right(eachAngle);
                }
            } else {
                for (int i = 0; i < nParts; i++) {
                    turtle.backward(eachLength);
                    turtle.left(eachAngle);
                }
            }
        } else {
            if (degrees > 0) {
                for (int i = 0; i < nParts; i++) {
                    turtle.forward(eachLength);
                    turtle.left(eachAngle);
                }
            } else {
                for (int i = 0; i < nParts; i++) {
                    turtle.forward(eachLength);
                    turtle.right(eachAngle);
                }
            }
        }
    }
    

}
