package ReedsSheppCurves;

public enum Gear {
    FORWARD(1),
    BACKWARD(-1);

    private int value;
    private Gear(int value){
        this.value = value;
    }
}