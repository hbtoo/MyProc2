import org.apache.poi.ss.usermodel.*;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.*;

public class Main1 extends JFrame {
    private JTextField outputField;
    private JTextField[] valueFields = new JTextField[8];

    private final String INPUT_FILE = "1.xlsx";
    private final int TARGET_ROW = 2;
    private final int[] COLS = {28, 29, 30, 31, 32, 33, 34, 35};
    private final String[] COL_NAMES = {
            "n(H2O)", "n(OH)", "n(H)", "n(O2)", "n(O)", "n(H2)", "n(H2O2)", "n(HO2)"
    };

    public Main1() {
        setTitle("Excel Species Editor");
        setSize(600, 250);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout(10, 10));

        // ==== File Path Panel ====
        JPanel filePanel = new JPanel(new GridLayout(1, 2, 5, 5));
        outputField = new JTextField("D:/2.xlsx");
        filePanel.add(new JLabel("Output File:"));
        filePanel.add(outputField);
        add(filePanel, BorderLayout.NORTH);

        // ==== Species Panel ====
        JPanel centerPanel = new JPanel(new GridLayout(1, 2, 20, 5));
        JPanel leftPanel = new JPanel(new GridBagLayout());
        JPanel rightPanel = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(4, 4, 4, 4);
        gbc.fill = GridBagConstraints.HORIZONTAL;

        for (int i = 0; i < COLS.length; i++) {
            JLabel label = new JLabel(COL_NAMES[i]);
            valueFields[i] = new JTextField();
            valueFields[i].setPreferredSize(new Dimension(150, 25));

            gbc.gridx = 0;
            gbc.gridy = i % 4;
            gbc.weightx = 0.3;
            if (i < 4) {
                leftPanel.add(label, gbc);
            } else {
                rightPanel.add(label, gbc);
            }

            gbc.gridx = 1;
            gbc.weightx = 0.7;
            if (i < 4) {
                leftPanel.add(valueFields[i], gbc);
            } else {
                rightPanel.add(valueFields[i], gbc);
            }
        }

        centerPanel.add(leftPanel);
        centerPanel.add(rightPanel);
        add(centerPanel, BorderLayout.CENTER);

        // ==== Button Panel ====
        JButton runButton = new JButton("Generate Excel File");
        JPanel buttonPanel = new JPanel();
        buttonPanel.add(runButton);
        add(buttonPanel, BorderLayout.SOUTH);

        runButton.addActionListener(this::modifyExcel);

        loadInitialValues();
        setVisible(true);
    }

    private void loadInitialValues() {
        try (FileInputStream fis = new FileInputStream(INPUT_FILE);
             Workbook workbook = new XSSFWorkbook(fis)) {

            Sheet sheet = workbook.getSheetAt(0);
            Row row = sheet.getRow(TARGET_ROW);
            if (row != null) {
                for (int i = 0; i < COLS.length; i++) {
                    Cell cell = row.getCell(COLS[i]);
                    if (cell != null) {
                        switch (cell.getCellType()) {
                            case STRING:
                                valueFields[i].setText(cell.getStringCellValue());
                                break;
                            case NUMERIC:
                                valueFields[i].setText(Double.toString(cell.getNumericCellValue()));
                                break;
                            case BOOLEAN:
                                valueFields[i].setText(Boolean.toString(cell.getBooleanCellValue()));
                                break;
                            default:
                                valueFields[i].setText("");
                                break;
                        }
                    }
                }
            }

        } catch (Exception ex) {
            ex.printStackTrace();
            JOptionPane.showMessageDialog(this, "Error loading values: " + ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    private void modifyExcel(ActionEvent e) {
        try (FileInputStream fis = new FileInputStream(INPUT_FILE);
             Workbook workbook = new XSSFWorkbook(fis)) {

            Sheet sheet = workbook.getSheetAt(0);
            Row row = sheet.getRow(TARGET_ROW);
            if (row == null) row = sheet.createRow(TARGET_ROW);

            for (int i = 0; i < COLS.length; i++) {
                String valStr = valueFields[i].getText().trim();
                Cell cell = row.getCell(COLS[i]);
                if (cell == null) cell = row.createCell(COLS[i]);

                try {
                    double val = Double.parseDouble(valStr);
                    cell.setCellValue(val);
                } catch (NumberFormatException nfe) {
                    cell.setCellValue(valStr);
                }
            }

            workbook.setForceFormulaRecalculation(true);

            try (FileOutputStream fos = new FileOutputStream(outputField.getText().trim())) {
                workbook.write(fos);
            }

            JOptionPane.showMessageDialog(this, "Excel file written:\n" + outputField.getText().trim());

        } catch (Exception ex) {
            ex.printStackTrace();
            JOptionPane.showMessageDialog(this, "Error writing file: " + ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                new Main1();
            }
        });
    }
}
