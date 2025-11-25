import org.apache.poi.ss.usermodel.Cell;
import org.apache.poi.ss.usermodel.Row;
import org.apache.poi.ss.usermodel.Sheet;
import org.apache.poi.ss.usermodel.Workbook;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class ExcelEditorPanel extends JPanel {

    boolean mac = false;  // default windows

    // 1: 1.xlsx
    // 2: 2.xlsx
    int mode = 1;

    private JTextField libreOfficeExecLocation;
    private JTextField inputExcelFile;
    private JTextField outputExcelFile;
    private JTextField[] valueFields;
    private PdfPreviewPanel previewPanel;

    private final int[] TARGET_ROW = new int []{2, 2};
    private final int[] nCols = new int []{8, 15};
    private final int[][] COLS = {{28, 29, 30, 31, 32, 33, 34, 35},
            {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50}};
    private final String[][] COL_NAMES = {
            {"n(H2O)", "n(OH)", "n(H)", "n(O2)", "n(O)", "n(H2)", "n(H2O2)", "n(HO2)"},
            {"n(CH3OH)", "n(CH2OH)", "n(CH3O)", "n(CH2O)", "n(HCO)", "n(CO)", "n(CO2)",
                    "n(O2)", "n(HO2)", "n(H2O2)", "n(H)", "n(H2)", "n(OH)", "n(H2O)", "n(O)"}
    };

    public ExcelEditorPanel(int mode) {
        this.mode = mode;
        setLayout(new BorderLayout(10, 10));

        // ==== Top Panel ====
        JPanel topPanel = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(5, 5, 5, 5);
        gbc.fill = GridBagConstraints.HORIZONTAL;

        String[] options = {"Windows", "Mac"};
        JComboBox<String> osSelector = new JComboBox<>(options);
        osSelector.setSelectedIndex(mac ? 1 : 0);

        String libreOfficePathMac = "/Applications/LibreOffice.app/Contents/MacOS/soffice";
        String libreOfficePathWindows = "D:/Program Files/LibreOffice/program/soffice.exe";

        libreOfficeExecLocation = new JTextField(mac ? libreOfficePathMac : libreOfficePathWindows);
        inputExcelFile = new JTextField(mac ? "/Users/bohuang/Downloads/" + mode + ".xlsx" : mode + ".xlsx");
        outputExcelFile = new JTextField(mac ? "/Users/bohuang/Downloads/" + mode + "-out.xlsx" : mode + "-out.xlsx");

        osSelector.addActionListener(e -> {
            mac = osSelector.getSelectedIndex() == 1;
            libreOfficeExecLocation.setText(mac ? libreOfficePathMac : libreOfficePathWindows);
            inputExcelFile.setText(mac ? "/Users/bohuang/Downloads/" + mode + ".xlsx" : mode + ".xlsx");
            outputExcelFile.setText(mac ? "/Users/bohuang/Downloads/" + mode + "-out.xlsx" : mode + "-out.xlsx");
        });

        // === Row 0 ===
        //gbc.gridx = 0; gbc.gridy = 0; gbc.weightx = 0.1; // narrow
        //topPanel.add(new JLabel("Select OS:", SwingConstants.RIGHT), gbc);

        //gbc.gridx = 1; gbc.gridy = 0; gbc.weightx = 0.1;
        //topPanel.add(osSelector, gbc);

        gbc.gridx = 2; gbc.gridy = 0; //gbc.weightx = 0.3; // wider label
        topPanel.add(new JLabel("Libre Office Install Location:", SwingConstants.RIGHT), gbc);

        gbc.gridx = 3; gbc.gridy = 0; //gbc.weightx = 0.5;
        topPanel.add(libreOfficeExecLocation, gbc);

        // === Row 1 === (equal label widths)
        gbc.gridx = 0; gbc.gridy = 1; gbc.weightx = 0.25;
        topPanel.add(new JLabel("Input Excel File:", SwingConstants.RIGHT), gbc);

        gbc.gridx = 1; gbc.gridy = 1; gbc.weightx = 0.25;
        topPanel.add(inputExcelFile, gbc);

        gbc.gridx = 2; gbc.gridy = 1; gbc.weightx = 0.25;
        topPanel.add(new JLabel("Output Excel File:", SwingConstants.RIGHT), gbc);

        gbc.gridx = 3; gbc.gridy = 1; gbc.weightx = 0.25;
        topPanel.add(outputExcelFile, gbc);

        add(topPanel, BorderLayout.NORTH);

        // ==== Center Panel ====
        JPanel centerPanel = new JPanel(new GridLayout(1, 2, 20, 5));
        JPanel leftPanel = new JPanel(new GridBagLayout());
        JPanel rightPanel = new JPanel(new GridBagLayout());
        gbc = new GridBagConstraints();
        gbc.insets = new Insets(4, 4, 4, 4);
        gbc.fill = GridBagConstraints.HORIZONTAL;

        valueFields = new JTextField[nCols[mode - 1]];
        int half = (nCols[mode - 1] + 1) / 2;
        for (int i = 0; i < nCols[mode - 1]; i++) {
            JLabel label = new JLabel(COL_NAMES[mode-1][i] + ":", SwingConstants.RIGHT);
            valueFields[i] = new JTextField();
            //valueFields[i].setPreferredSize(new Dimension(250, 25));

            gbc.gridx = 0;
            gbc.gridy = i % half;
            gbc.weightx = 0.3;
            if (i < half) {
                leftPanel.add(label, gbc);
            } else {
                rightPanel.add(label, gbc);
            }

            gbc.gridx = 1;
            gbc.weightx = 0.7;
            if (i < half) {
                leftPanel.add(valueFields[i], gbc);
            } else {
                rightPanel.add(valueFields[i], gbc);
            }
        }

        centerPanel.add(leftPanel);
        centerPanel.add(rightPanel);
        //add(centerPanel, BorderLayout.CENTER);

        // ==== Bottom Panel ====

        JButton runButton = new JButton("Generate Excel File");
        previewPanel = new PdfPreviewPanel();

        JPanel bottomPanel = new JPanel(new BorderLayout());
        bottomPanel.add(runButton, BorderLayout.NORTH);
        bottomPanel.add(previewPanel, BorderLayout.CENTER);

        //add(bottomPanel, BorderLayout.SOUTH);

        JSplitPane splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT, centerPanel, bottomPanel);
        splitPane.setResizeWeight(0.1);
        splitPane.setOneTouchExpandable(false);
        add(splitPane, BorderLayout.CENTER);

        // ==== Initialize ====

        runButton.addActionListener(this::modifyExcel);

        loadInitialValues();
        //setLocationRelativeTo(null);
        setVisible(true);
    }

    private void loadInitialValues() {
        try (FileInputStream fis = new FileInputStream(inputExcelFile.getText());
             Workbook workbook = new XSSFWorkbook(fis)) {

            Sheet sheet = workbook.getSheetAt(0);
            Row row = sheet.getRow(TARGET_ROW[mode - 1]);
            if (row != null) {
                for (int i = 0; i < COLS[mode - 1].length; i++) {
                    Cell cell = row.getCell(COLS[mode - 1][i]);
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
        String outputExcel = outputExcelFile.getText().trim();

        try (FileInputStream fis = new FileInputStream(inputExcelFile.getText());
             Workbook workbook = new XSSFWorkbook(fis)) {

            Sheet sheet = workbook.getSheetAt(0);
            Row row = sheet.getRow(TARGET_ROW[mode - 1]);
            if (row == null) row = sheet.createRow(TARGET_ROW[mode - 1]);

            for (int i = 0; i < COLS.length; i++) {
                String valStr = valueFields[i].getText().trim();
                Cell cell = row.getCell(COLS[mode - 1][i]);
                if (cell == null) cell = row.createCell(COLS[mode - 1][i]);

                try {
                    double val = Double.parseDouble(valStr);
                    cell.setCellValue(val);
                } catch (NumberFormatException nfe) {
                    cell.setCellValue(valStr);
                }
            }

            workbook.setForceFormulaRecalculation(true);
            //FormulaEvaluator evaluator = workbook.getCreationHelper().createFormulaEvaluator();
            //evaluator.evaluateAll();

            try (FileOutputStream fos = new FileOutputStream(outputExcel)) {
                workbook.write(fos);
            }

            //JOptionPane.showMessageDialog(this, "Excel file written:\n" + outputExcel);

        } catch (Exception ex) {
            ex.printStackTrace();
            JOptionPane.showMessageDialog(this, "Error writing file: " + ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }

        File outFile = new File(outputExcelFile.getText().trim());
        String dir = outFile.getParent();
        if (dir == null)
            dir = ".";
        String pdfFile = new File(outputExcel).getName().replaceFirst("[.][^.]+$", "") + ".pdf";
        try {
            ProcessBuilder pb = new ProcessBuilder(
                    libreOfficeExecLocation.getText().trim(),
                    "--headless",          // no GUI
                    "--convert-to", "pdf", // convert to PNG format
                    "--outdir", dir,       // pdf output directory
                    outputExcel            // input excel file path
            );
            Process process = pb.start();
            int exitCode = process.waitFor();
            if (exitCode != 0) {
                throw new IOException("Conversion failed. Exit code: " + exitCode);
            }

            previewPanel.loadPdf(dir + "/" + pdfFile);

            JOptionPane.showMessageDialog(this, "Done!");

        } catch (Exception ex) {
            ex.printStackTrace();
            JOptionPane.showMessageDialog(this, "Error exporting excel charts to pdf: " + ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }
}
