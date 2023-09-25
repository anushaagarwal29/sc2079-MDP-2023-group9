package ReedsSheppCurves;
import java.lang.Math;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Function;

public class WordsGenerator {
    

    public Words word_cluster_1(double x, double y, double phi){
        //Formula 8.1 for CSC words
        Words word = new Words();
        Util utils = new Util();
        double[] ut = new double[2];
        double t, u, v;
        ut = utils.R(x - Math.sin(phi), y - 1 + Math.cos(phi));
        u = ut[0];
        t = ut[1];
        v = utils.M(phi - t);

        double[] params = {t, u, v};
        Steering[] steerings = {Steering.LEFT, Steering.STRAIGHT, Steering.LEFT};
        Gear[] gears = {Gear.FORWARD, Gear.FORWARD, Gear.FORWARD};

        for (int i = 0; i < params.length; i++){
            word.add_letter(new Letter(params[i], steerings[i], gears[i]));
        }

        return word;
    }

    public Words word_cluster_2(double x, double y, double phi){
        //for mula 8.2 for CSC words
        Words word = new Words();
        Util utils = new Util();
        double[] ut = new double[2];
        double t, u, v;
        ut = utils.R(x + Math.sin(phi), y - 1 - Math.cos(phi));
        u = ut[0];
        t = ut[1];
        if (Math.pow(u,2) >= 4){
            u = Math.sqrt(Math.pow(u, 2) - 4);
            double[] ut2 = utils.R(u, 2);
            double theta = ut2[1];
            t = utils.M(t + theta);
            v = utils.M(t - phi);

            double[] params = {t, u, v};
            Steering[] steerings = {Steering.LEFT, Steering.STRAIGHT, Steering.RIGHT};
            Gear[] gears = {Gear.FORWARD, Gear.FORWARD, Gear.FORWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }
        }
        return word;
    }

    public Words word_cluster_3(double x, double y, double phi){
        //formulate 8.3 for C|C|C words
        Words word = new Words();
        Util utils = new Util();
        double[] rho_theta = new double[2];
        double xi, eta, rho, theta;
        double t, u, v;

        xi = x - Math.sin(phi);
        eta = y - 1 + Math.cos(phi);
        rho_theta = utils.R(xi, eta);
        rho = rho_theta[0];
        theta = rho_theta[1];

        if (rho <= 4){
            double A = Math.acos(rho / 4);
            t = utils.M(theta + Math.PI/2 + A);
            u = utils.M(Math.PI - 2 * A);
            v = utils.M(phi - t - u);

            double[] params = {t, u, v};
            Steering[] steerings = {Steering.LEFT, Steering.RIGHT, Steering.LEFT};
            Gear[] gears = {Gear.FORWARD, Gear.BACKWARD, Gear.FORWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }
        }

        return word;
    }

    public Words word_cluster_4(double x, double y, double phi){
        //formulate 8.4 for C|CC words
        Words word = new Words();
        Util utils = new Util();
        double[] rho_theta = new double[2];
        double xi, eta, rho, theta;
        double t, u, v;

        xi = x - Math.sin(phi);
        eta = y - 1 + Math.cos(phi);
        rho_theta = utils.R(xi, eta);
        rho = rho_theta[0];
        theta = rho_theta[1];

        if (rho <= 4){
            double A = Math.acos(rho / 4);
            t = utils.M(theta + Math.PI/2 + A);
            u = utils.M(Math.PI - 2 * A);
            v = utils.M(t + u - phi);

            double[] params = {t, u, v};
            Steering[] steerings = {Steering.LEFT, Steering.RIGHT, Steering.LEFT};
            Gear[] gears = {Gear.FORWARD, Gear.BACKWARD, Gear.BACKWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }
        }
        return word;
    }

    public Words word_cluster_5(double x, double y, double phi){
        //formulate 8.4 for CC|C words
        Words word = new Words();
        Util utils = new Util();
        double[] rho_theta = new double[2];
        double xi, eta, rho, theta;
        double t, u, v;

        xi = x - Math.sin(phi);
        eta = y - 1 + Math.cos(phi);
        rho_theta = utils.R(xi, eta);
        rho = rho_theta[0];
        theta = rho_theta[1];

        if (rho <= 4){
            u = Math.acos(1 - Math.pow(rho, 2)/8);
            double A = Math.asin(2 * Math.sin(u)/rho);
            t = utils.M(theta + Math.PI/2 - A);
            v = utils.M(t - u - phi);

            double[] params = {t, u, v};
            Steering[] steerings = {Steering.LEFT, Steering.RIGHT, Steering.LEFT};
            Gear[] gears = {Gear.FORWARD, Gear.FORWARD, Gear.BACKWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }
        }
        return word;
    }

    public Words word_cluster_6(double x, double y, double phi){
        //formulate 8.7 for CCu|CuC words
        Words word = new Words();
        Util utils = new Util();
        double[] rho_theta = new double[2];
        double xi, eta, rho, theta;
        double t, u, v;

        xi = x + Math.sin(phi);
        eta = y - 1 - Math.cos(phi);
        rho_theta = utils.R(xi, eta);
        rho = rho_theta[0];
        theta = rho_theta[1];

        if (rho <= 4){
            if (rho <= 2){
                double A = Math.acos((rho + 2)/4);
                t = utils.M(theta + Math.PI/2 + A);
                u = utils.M(A);
                v = utils.M(phi - t + 2 * u);
            } else {
                double A = Math.acos((rho - 2)/4);
                t = utils.M(theta + Math.PI/2 - A);
                u = utils.M(Math.PI - A);
                v = utils.M(phi - t + 2 * u);
            }

            double[] params = {t, u, u, v};
            Steering[] steerings = {Steering.LEFT, Steering.RIGHT, Steering.LEFT, Steering.RIGHT};
            Gear[] gears = {Gear.FORWARD, Gear.FORWARD, Gear.BACKWARD, Gear.BACKWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }
        }
        return word;
    }

    public Words word_cluster_7(double x, double y, double phi){
        //formulate 8.8 C|CuCu|C words
        Words word = new Words();
        Util utils = new Util();
        double[] rho_theta = new double[2];
        double xi, eta, rho, theta;
        double t, u1, v, u;

        xi = x + Math.sin(phi);
        eta = y - 1 - Math.cos(phi);
        rho_theta = utils.R(xi, eta);
        rho = rho_theta[0];
        theta = rho_theta[1];
        u1 = (20 - rho * rho) / 16;

        if (rho <= 6 && 0 <= u1 && u1 <= 1){
            u = Math.acos(u1);
            double A = Math.asin(2 * Math.sin(u) / rho);
            t = utils.M(theta + Math.PI/2 + A);
            v = utils.M(t - phi);

            double[] params = {t, u, u, v};
            Steering[] steerings = {Steering.LEFT, Steering.RIGHT, Steering.LEFT, Steering.RIGHT};
            Gear[] gears = {Gear.FORWARD, Gear.BACKWARD, Gear.BACKWARD, Gear.FORWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }
        }
        return word;
    }

    public Words word_cluster_8(double x, double y, double phi){
        //Formule 8.9 for C|C[pi/2]SC words
        Words word = new Words();
        Util utils = new Util();
        double[] rho_theta = new double[2];
        double xi, eta, rho, theta;
        double t, v, u;

        xi = x - Math.sin(phi);
        eta = y - 1 + Math.cos(phi);
        rho_theta = utils.R(xi, eta);
        rho = rho_theta[0];
        theta = rho_theta[1];

        if (rho >= 2){
            u = Math.sqrt(rho * rho - 4) - 2;
            double A = Math.atan2(2, u + 2);
            t = utils.M(theta + Math.PI/2 + A);
            v = utils.M(t - phi + Math.PI/2);

            double[] params = {t, Math.PI/2, u, v};
            Steering[] steerings = {Steering.LEFT, Steering.RIGHT, Steering.STRAIGHT, Steering.LEFT};
            Gear[] gears = {Gear.FORWARD, Gear.BACKWARD, Gear.BACKWARD, Gear.BACKWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }
        }
        return word;
    }

    public Words word_cluster_9(double x, double y, double phi){
        Words word = new Words();
        Util utils = new Util();
        double[] rho_theta = new double[2];
        double xi, eta, rho, theta;
        double t, v, u;

        xi = x - Math.sin(phi);
        eta = y - 1 + Math.cos(phi);
        rho_theta = utils.R(xi, eta);
        rho = rho_theta[0];
        theta = rho_theta[1];

        if (rho >= 2){
            u = Math.sqrt(rho * rho - 4) - 2;
            double A = Math.atan2(u + 2, u);
            t = utils.M(theta + Math.PI/2 + A);
            v = utils.M(t - phi - Math.PI/2);

            double[] params = {t, u, Math.PI/2, v};
            Steering[] steerings = {Steering.LEFT, Steering.STRAIGHT, Steering.RIGHT, Steering.LEFT};
            Gear[] gears = {Gear.FORWARD, Gear.FORWARD, Gear.FORWARD, Gear.BACKWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }

        }
        return word;
    }

    public Words word_cluster_10(double x, double y, double phi){
        //formulate 8.10 for C|C[pi/2]SC words
        Words word = new Words();
        Util utils = new Util();
        double[] rho_theta = new double[2];
        double xi, eta, rho, theta;
        double t, v, u;
        
        xi = x + Math.sin(phi);
        eta = y - 1 - Math.cos(phi);
        rho_theta = utils.R(xi, eta);
        rho = rho_theta[0];
        theta = rho_theta[1];

        if (rho >= 2){
            t = utils.M(theta + Math.PI/2);
            u = rho - 2;
            v = utils.M(phi - t - Math.PI/2);

            double[] params = {t, Math.PI/2, u, v};
            Steering[] steerings = {Steering.LEFT, Steering.RIGHT, Steering.STRAIGHT, Steering.RIGHT};
            Gear[] gears = {Gear.FORWARD, Gear.BACKWARD, Gear.BACKWARD, Gear.BACKWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }
        }
        return word;
    }

    public Words word_cluster_11(double x, double y, double phi){
        //formulate 8.10 for CSC[pi/2]|C words
        Words word = new Words();
        Util utils = new Util();
        double[] rho_theta = new double[2];
        double xi, eta, rho, theta;
        double t, v, u;

        xi = x + Math.sin(phi);
        eta = y - 1 - Math.cos(phi);
        rho_theta = utils.R(xi, eta);
        rho = rho_theta[0];
        theta = rho_theta[1];

        if (rho >= 2){
            t = utils.M(theta);
            u = rho - 2;
            v = utils.M(phi - t - Math.PI/2);

            double[] params = {t, u, Math.PI/2, v};
            Steering[] steerings = {Steering.LEFT, Steering.STRAIGHT, Steering.LEFT, Steering.RIGHT};
            Gear[] gears = {Gear.FORWARD, Gear.FORWARD, Gear.FORWARD, Gear.BACKWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }
        }
        return word;
    }

    public Words word_cluster_12(double x, double y, double phi){
        //formulate 8.11 for C|C[pi/2]SC[pi/2]|C words
        Words word = new Words();
        Util utils = new Util();
        double[] rho_theta = new double[2];
        double xi, eta, rho, theta;
        double t, v, u;

        xi = x + Math.sin(phi);
        eta = y - 1 + Math.cos(phi);
        rho_theta = utils.R(xi, eta);
        rho = rho_theta[0];
        theta = rho_theta[1];

        if (rho >= 4){
            u = Math.sqrt(rho * rho - 4) / 4;
            double A = Math.atan2(2, u + 4);
            t = utils.M(theta + Math.PI/2 + A);
            v = utils.M(t - phi);

            double[] params = {t, Math.PI/2, u, Math.PI/2, v};
            Steering[] steerings = {Steering.LEFT, Steering.RIGHT, Steering.STRAIGHT, Steering.LEFT, Steering.RIGHT};
            Gear[] gears = {Gear.FORWARD, Gear.BACKWARD, Gear.BACKWARD, Gear.BACKWARD, Gear.FORWARD};

            for (int i = 0; i < params.length; i++){
                word.add_letter(new Letter(params[i], steerings[i], gears[i]));
            }
        }
        return word;
    }

}
