package ReedsSheppCurves;
import java.lang.Math;

public class Util implements Cloneable{

    public double[] R(double x, double y){
        //polar transform (r, theta) for (x,y)
        double r = Math.sqrt(Math.pow(x, 2)+ Math.pow(y, 2));
        double theta = Math.atan2(y, x);
        return new double[] {r, theta};
    }

    public double M(double theta){
        //Map theta to [-pi, pi)
        theta %= 2 * Math.PI;
        if (theta < -Math.PI){
            theta += 2 * Math.PI;
        } else if (theta >= Math.PI){
            theta -= 2 * Math.PI;
        }
        return theta;
    }

    public double[] normalise_basis(double[] p1, double[] p2){
        //normalise the basis so that p1 equals (0,0,0)
        double dx = p2[0] - p1[0];
        double dy = p2[1] - p1[1];
        double phi1 = p1[2];
        //rotate matrix with -phi
        double x_new = Math.cos(phi1) * dx + Math.sin(phi1) * dy;
        double y_new = -Math.sin(phi1) * dx + Math.cos(phi1) * dy;
        double phi_new = p2[2] - p1[2];
        return new double[] {x_new, y_new, phi_new};
    }

    public double rad2deg(double rad){
        return rad/Math.PI * 180;
    }

    public double deg2rad(double deg){
        return Math.PI*deg/180;
    }

    // public Words timeflip(Words w) throws CloneNotSupportedException{
    //     Words new_words = new Words();
    //     try {
    //         new_words = (Words) w.clone();
    //     }
    // }
}
