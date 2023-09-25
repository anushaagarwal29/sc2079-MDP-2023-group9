package ReedsSheppCurves;


public enum Steering {
    LEFT(1),
    STRAIGHT(0),
    RIGHT(-1);

    private int value;
    private Steering(int value){
        this.value = value;
    }
}

