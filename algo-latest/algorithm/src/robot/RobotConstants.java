package robot;

import map.MapConstants;

import java.awt.*;

public class RobotConstants {
    public static final int ROBOT_VIRTUAL_WIDTH = 30;
    public static final Point ROBOT_INITIAL_CENTER_COORDINATES = new Point(1 + MapConstants.ARENA_BORDER_SIZE, 18 + MapConstants.ARENA_BORDER_SIZE);
    //
    //public static final Point ROBOT_INITIAL_CENTER_COORDINATES = new Point(1, 1);
    public static final int MOVE_COST = 10;
    public static final int REVERSE_COST = 10;
    public static final int TURN_COST_90 = 60;
    public static final int MAX_COST = Integer.MAX_VALUE;

    //Indoor
    public static final double LEFT_TURN_RADIUS_Y = 12; //17; //12
    public static final double LEFT_TURN_RADIUS_X = 26; //28; //26
    public static final double RIGHT_TURN_RADIUS_Y = 3; //17; //2.5
    public static final double RIGHT_TURN_RADIUS_X = 20; //28; //21

    //Outdoor
    /*public static final double LEFT_TURN_RADIUS_Y = 13; //17; //12
    public static final double LEFT_TURN_RADIUS_X = 28; //28; //26
    public static final double RIGHT_TURN_RADIUS_Y = 2; //17; //2
    public static final double RIGHT_TURN_RADIUS_X = 22; //28; //21*/

    public static final double MOVE_SPEED = 21; // in cm per second

    public enum ROBOT_DIRECTION {
        NORTH, EAST, SOUTH, WEST;
    }
}
//Left Turn
//X: 11
//Y: 23

//RIght Turn
//X: 9
//Y: 19


//New Turning Radius
//Right Turn: X:37 Y:23
//Left Turn: X:25 Y:11
