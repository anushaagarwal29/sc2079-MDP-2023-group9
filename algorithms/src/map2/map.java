package map2;

import java.util.ArrayList;

public class map {
    private ArrayList<double[]> nodes;

    public ArrayList<double[]> getNodeList(){
        return this.nodes;
    }

    public ArrayList<double[]> generatePathNodes(){
        ArrayList<double[]> nodes = new ArrayList<double[]>();
        double[] node = {-6, -7, Math.PI * 2 * 0.25};
        double[] node2 = {-6, 0, Math.PI * 2 * 0.5};
        double[] node3 = {-4, -6, Math.PI * 2 * 0.75};
        double[] node4 = {0, 5, Math.PI * 2};
        double[] node5 = {0, -2, Math.PI * 0.25};
        double[] node6 = {-2, -6, Math.PI * 2 * 0.5};
        double[] node7 = {3, -5, Math.PI * 2 * 0.75};
        nodes.add(node);
        nodes.add(node2);
        nodes.add(node3);
        nodes.add(node4);
        nodes.add(node5);
        nodes.add(node);
        nodes.add(node2);
        
        this.nodes = nodes;
        return nodes;
    }
}
