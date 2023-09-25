package utils;

import algorithms.*;
import map.Arena;
import map.MapConstants;
import map.Node;
import map.PictureObstacle;
import robot.Robot;
import robot.RobotConstants;
import utils.CommConstants.INSTRUCTION_TYPE;

import java.util.ArrayList;

public class PathToCommand {
    static Robot bot = new Robot(RobotConstants.ROBOT_INITIAL_CENTER_COORDINATES, RobotConstants.ROBOT_DIRECTION.NORTH, false);
    static Arena arena = new Arena(bot);
    static CommMgr comm = CommMgr.getCommMgr();

    static FastestPathAlgo fast = new FastestPathAlgo(arena);
    static TripPlannerAlgo algo = new TripPlannerAlgo(arena);


    /**
     * main module to start the algo and establish communication with rpi
     * @param args
     */
    public static void main(String[] args) {
        // establish connection to RPI
        comm.connectToRPi();
        // receive obstacles from android
        recvObstacles();
        int[] path = fast.planFastestPath();

        doThePath(path);
        System.out.println("No more possible nodes to visit. Pathing finished");
        comm.endConnection();
    }

    /**
     * receive obstacle info from android, plan path, and send instructions to robot
     * @param path
     */
    private static void doThePath(int[] path) {
        algo.constructMap();
        ArrayList<PictureObstacle> map = Arena.getObstacles();
        Robot r = arena.getRobot();
        int startX = r.getX();
        int startY = r.getY();
        int startAngle = r.getRobotDirectionAngle();
        PictureObstacle next;
        ArrayList<MoveType> arrayList;
        int count = 0;
        for (int i : path) {
            next = map.get(i);
            System.out.println("---------------Path " + count + "---------------");
            System.out.println(next.getX() + ", " + next.getY());
            comm.sendMsg("OBJ|" + (i+1) + "|");
            System.out.println("Obstacle visiting: " + (i+1) + "\n");
            arrayList = algo.planPath(startX, startY, startAngle, next.getX(), next.getY(), next.getImadeDirectionAngle(), true, true, true);
            if (arrayList != null) {// if there is a path
                sendMovesToRobot(arrayList, i);
                int[] coords = algo.getEndPosition();
                startX = coords[0];
                startY = coords[1];
                startAngle = coords[2];
                count++;
            } else {
                System.out.println("No path found, trying to path to the next obstacle");
            }
        }
    }

    /**
     * send instructions to robot; reverse and retake image if img recognition is null; send img result to android
     * @param moveList
     * @param i
     */
    private static void sendMovesToRobot(ArrayList<MoveType> moveList, int i) {
        int tryCount = 4;
        ArrayList<MoveType> backwardMoveList;
        int[] coords;

        int[] backwardCoords;

        String commandsToSend = encodeMoves(moveList);

        sendToRobot(commandsToSend);
        String str = takeImage();
        //retry if image taken is null
        while ((str.equals("null") || str.equals("bulls")) && tryCount > 0) {
            tryCount--;
            // try to go backwards by 1.
            coords = algo.getEndPosition();
            backwardCoords = algo.getReversePos(coords[0], coords[1], coords[2] / 90);
            if (backwardCoords == null) break; // if no backwards position available, just break.
            System.out.println("Reversing to retake picture...");
            backwardMoveList = algo.planPath(coords[0], coords[1], coords[2], backwardCoords[0], backwardCoords[1], backwardCoords[2] * 90, false, true, true);
            if (backwardMoveList != null) { // if we can't reverse, just break from the loop.
                commandsToSend = encodeMoves(backwardMoveList);
                sendToRobot(commandsToSend);
                str = takeImage();
            } else {
                break;
            }
        }
        if(tryCount==4);
        
        //sendImageToAndroid(i, str);
    }

    /**
     * encode each robot move to string instructions
     * @param moveList
     * @return
     */
    private static String encodeMoves(ArrayList<MoveType> moveList) {
        String commandsToSend = ":STM:";
        INSTRUCTION_TYPE instructionType;
        String formatted;

        for (MoveType move : moveList) {
            int measure = 0;
            if (move.isLine()) {
                measure = (int) move.getLength();
                formatted = String.format("%03d", measure);
                if (move.isReverse()) {
                    instructionType = INSTRUCTION_TYPE.BACKWARD;
                } else {
                    instructionType = INSTRUCTION_TYPE.FORWARD;
                }
                commandsToSend += INSTRUCTION_TYPE.encode(instructionType) + formatted + ",";
            } else {
                ArcMove moveConverted = (ArcMove) move;
                if (moveConverted.isTurnLeft()) {
                    instructionType = INSTRUCTION_TYPE.FORWARD_LEFT;
                } else {
                    instructionType = INSTRUCTION_TYPE.FORWARD_RIGHT;
                }
                commandsToSend +=  INSTRUCTION_TYPE.encode(instructionType) + "090" + ",";
            }
        }
        return commandsToSend.substring(0, commandsToSend.length() - 1);
    }

    /**
     * send to robot the completed list of paths.
     * @param cmd
     */
    private static void sendToRobot(String cmd) {
        sendPathToAndroid();
        comm.sendMsg(cmd);
        String receiveMsg = null;

        // buffer so we can space commands
        try {
            Thread.sleep(500);//time is in ms (1000 ms = 1 second)
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        //sendPathToAndroid();
        // while (receiveMsg == null || !receiveMsg.equals("Done")) {
        //     receiveMsg = comm.recieveMsg();
        // }

        System.out.println("Message: " + receiveMsg + "\n");
        try {
            Thread.sleep(500); //time is in ms (1000 ms = 1 second)
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * send img recognition result to android
     * @param obstacleID
     * @param image
     */
    private static void sendImageToAndroid(int obstacleID, String image) {
        String msg;
        msg = ":AND:TARGET," + (obstacleID + 1) + "," + image;
        comm.sendMsg(msg);
        try { // try to wait
            Thread.sleep(500);//time is in ms (1000 ms = 1 second)
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * send instruction to to rpi to take the image
     */
    private static String takeImage() {
        //comm.sendMsg(":IMG:scan");
        //System.out.println("Scanning...");
        String receiveMsg = null;
        System.out.println("Waiting for image taken:");
        while (receiveMsg == null || receiveMsg.isEmpty()) {
            receiveMsg = comm.recieveMsg();
        }
        System.out.println("Message: " + receiveMsg + "\n");
        return receiveMsg;
    }

    /**
     * send the path to android to provide real time location update
     */
    private static void sendPathToAndroid() {
        ArrayList<Node> path = algo.getNodePath();
        String pathString = ":AND:PATH,";
        // PATH|x,y,0-270|
        for (Node n : path) {
            pathString += "|" + (n.getX() - MapConstants.ARENA_BORDER_SIZE) + "," + (n.getY() - MapConstants.ARENA_BORDER_SIZE) + "," + n.getDim() * 90;
        }
        comm.sendMsg(pathString);
    }

    /**
     * receive obstacle information (coord and facing) from Android -> RPI
     */
    private static void recvObstacles() {
        String receiveMsg;
        System.out.println("Waiting to receive obstacle list...");
        receiveMsg = comm.recieveMsg();
        System.out.println("Received Obstacles String: " + receiveMsg + "\n");

        // "ADD|1,3,4,N|2,4,5,E|3,5,6,S|4,9,4,N|5,9,10,E|"
        String[] positions = receiveMsg.split("\\|");
        System.out.println(positions.length);

        for (int i = 1; i < positions.length; i++) {
            String[] obs = positions[i].split(",");
            arena.addPictureObstacle(Integer.parseInt(obs[1]), Integer.parseInt(obs[2]), MapConstants.IMAGE_DIRECTION.getImageDirection(obs[3]));
            
        }
        // arena.addPictureObstacle(10, 10, MapConstants.IMAGE_DIRECTION.getImageDirection("N"));
        // arena.addPictureObstacle(8, 8, MapConstants.IMAGE_DIRECTION.getImageDirection("S"));
        // arena.addPictureObstacle(15, 15, MapConstants.IMAGE_DIRECTION.getImageDirection("W"));
    }
}
