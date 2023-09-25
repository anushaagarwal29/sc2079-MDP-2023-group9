package map2;

import java.util.ArrayList;

public class Obstacle {
    private ArrayList<double[]> nodes;

    public ArrayList<double[]> getNodeList(){
        return this.nodes;
    }

    public ArrayList<double[]> generatePathNodes(){
        ArrayList<double[]> nodes = new ArrayList<double[]>();
        double[] node = {4, 5, Math.PI * 2 * 0.25};
        double[] node2 = {10, 12, Math.PI * 2 * 0.5};
        double[] node3 = {5, 6, Math.PI * 2 * 0.75};
        nodes.add(node);
        nodes.add(node2);
        nodes.add(node3);
    
        this.nodes = nodes;
        return nodes;
    }
}
