package ReedsSheppCurves;

public class Letter implements Cloneable{


    private double param; // t, u, or v, length of the letter curve
    private Steering steering;
    private Gear gear;
    
    public Letter(){}
    public Letter(double param, Steering steering, Gear gear){
        this.param = param;
        this.steering = steering;
        this.gear = gear;
    }

    public double getParam(){
        return this.param;
    }

    public Steering getSteering(){
        return this.steering;
    }

    public Gear getGear(){
        return this.gear;
    }
    
    @Override
    protected Object clone() throws CloneNotSupportedException {
        return super.clone();
    }

    public String toString(){
        String steering_str, gear_str, info_str;
        if (this.steering == Steering.LEFT){
            steering_str = "left";
        } else if (this.steering == Steering.RIGHT) {
            steering_str = "right";
        } else {
            steering_str = "straight";
        }

        if (this.gear == Gear.FORWARD){
            gear_str = "forward";
        } else {
            gear_str = "backward";
        }

        info_str = "{ Steering: " + steering_str + "\tGear: " + gear_str 
                    + "\tLength: " + Double.toString(Math.round(param * 100)/100d) + " }";

        return info_str;
    }

    public void reverse_steering(){
        if (this.steering == Steering.LEFT){
            this.steering = Steering.RIGHT;
        } else if (this.steering == Steering.RIGHT){
            this.steering = Steering.LEFT;
        }
    }

    public void reverse_gear(){
        if (this.gear == Gear.BACKWARD){
            this.gear = Gear.FORWARD;
        } else if (this.gear == Gear.FORWARD){
            this.gear = Gear.BACKWARD;
        }
    }

    public double get_param(){
        return this.param;
    }
}
