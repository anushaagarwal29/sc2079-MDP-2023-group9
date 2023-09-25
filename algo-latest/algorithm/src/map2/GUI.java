package map2;

import java.awt.*;
import javax.swing.*;

public class GUI {
    
    public static void main(String[] args) {
        new GUI();
    }

    public GUI() {
        EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {
                try {
                    UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
                } catch (ClassNotFoundException | InstantiationException | IllegalAccessException | UnsupportedLookAndFeelException ex) {
                    ex.printStackTrace();
                }
                JFrame frame = new JFrame("Simulator");
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                frame.setLocationRelativeTo(null);
                frame.setVisible(true);
                frame.setSize(450,900);

                frame.setLayout(new GridLayout(2,1));

                JPanel gridJPanel = new JPanel();
                gridJPanel.add(new TestPane());
                //gridJPanel.setBackground(Color.yellow);

                JPanel controPanel = new JPanel();
                //controPanel.setBackground(Color.black);

                frame.add(gridJPanel);
                frame.add(controPanel);
                
            }
        });
    }

    public class TestPane extends JPanel {
        @Override
        public Dimension getPreferredSize() {
            return new Dimension(800, 400);
        }

        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            Graphics2D g2d = (Graphics2D) g.create();
            int size = Math.min(getWidth() - 4, getHeight() - 4) / 20;
            int width = getWidth() - (size * 2);
            int height = getHeight() - (size * 2);

            int y = (getHeight() - (size * 20)) / 2;
            for (int horz = 0; horz < 20; horz++) {
                int x = (getWidth() - (size * 20)) / 2;
                for (int vert = 0; vert < 20; vert++) {
                    g.drawRect(x, y, size, size);
                    x += size;
                }
                y += size;
            }
            g2d.dispose();
        }

    }
}
