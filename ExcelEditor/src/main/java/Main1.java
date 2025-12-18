import javax.swing.*;
import java.awt.*;
import java.io.*;

public class Main1 extends JFrame {

    public Main1() {
        setTitle("溶液计算");
        setSize(1000, 750);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout(10, 10));

        JTabbedPane tabs = new JTabbedPane();

        // === Tab 1: mode = 1 ===
        ExcelEditorPanel panel1 = new ExcelEditorPanel(1);
        tabs.addTab("纯水", panel1);

        // === Tab 2: mode = 2 ===
        ExcelEditorPanel panel2 = new ExcelEditorPanel(2);
        tabs.addTab("甲醇-水", panel2);

        add(tabs, BorderLayout.CENTER);
        setLocationRelativeTo(null);
        setVisible(true);
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(Main1::new);

    }
}
