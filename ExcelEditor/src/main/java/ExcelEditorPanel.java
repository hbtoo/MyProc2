import org.apache.poi.ss.usermodel.*;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.AbstractMap;
import java.util.List;

public class ExcelEditorPanel extends JPanel {

    boolean mac = false;  // default windows

    // 1: 1.xlsx
    // 2: 2.xlsx
    int mode = 1;
    private String dir;

    private JTextField libreOfficeExecLocation;
    //private JTextField inputExcelFile;
    //private JTextField outputExcelFile;
    private PdfPreviewPanel previewPanel1, previewPanel2;

    private JTextField[] valueFieldsForExcel;
    private final int[] TARGET_ROW = new int []{2, 2};
    private final int[][] COLS = {{28, 29, 30, 31, 32, 33, 34, 35},
            {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50}};
    private final String[][] COL_NAMES = {
            {"n(H2O)", "n(OH)", "n(H)", "n(O2)", "n(O)", "n(H2)", "n(H2O2)", "n(HO2)"},
            {"n(CH3OH)", "n(CH2OH)", "n(CH3O)", "n(CH2O)", "n(HCO)", "n(CO)", "n(CO2)",
                    "n(O2)", "n(HO2)", "n(H2O2)", "n(H)", "n(H2)", "n(OH)", "n(H2O)", "n(O)"}
    };

    private JTextField[] fieldsForMatlab;
    private final String[] MATLAB_INPUT_VARS = {
            "R0", "P0", "PA", "f", "Pv", "n", "rho", "sigma", "miu", "c"
    };
    public static final double[] MATLAB_INPUT_DEFAULT = {
            Math.pow(10, -5), 101325, -7.041 * 101325, 20000,
            5522, 1.388, 930.33, 0.05746,
            0.0008703, 1363.8
    };

    // t(s), t(us), R(m), TK    V(m3)?
    public static final List<Integer> matlab_out_cols = List.of(0, 3, 2, 4);
    private final int[][] excel_out_cols = {{5, 1, 2, 7}, {1, 0, 4, 3}};

    public ExcelEditorPanel(int mode) {
        this.mode = mode;
        setLayout(new BorderLayout(10, 10));

        String[] options = {"Windows", "Mac"};
        JComboBox<String> osSelector = new JComboBox<>(options);
        osSelector.setSelectedIndex(mac ? 1 : 0);

        String libreOfficePathMac = "/Applications/LibreOffice.app/Contents/MacOS/soffice";
        String libreOfficePathWindows = "D:/Program Files/LibreOffice/program/soffice.exe";

        libreOfficeExecLocation = new JTextField(mac ? libreOfficePathMac : libreOfficePathWindows);
        //inputExcelFile = new JTextField(mac ? "/Users/bohuang/Downloads/" + mode + ".xlsx" : "resource/" + mode + ".xlsx");
        //outputExcelFile = new JTextField(mac ? "/Users/bohuang/Downloads/" + mode + "-out.xlsx" : "resource/" + mode + "-out.xlsx");

        osSelector.addActionListener(e -> {
            mac = osSelector.getSelectedIndex() == 1;
            libreOfficeExecLocation.setText(mac ? libreOfficePathMac : libreOfficePathWindows);
            //inputExcelFile.setText(mac ? "/Users/bohuang/Downloads/" + mode + ".xlsx" : mode + ".xlsx");
            //outputExcelFile.setText(mac ? "/Users/bohuang/Downloads/" + mode + "-out.xlsx" : mode + "-out.xlsx");
        });

        // ==== Left Panel ====


        JPanel left = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();

        // Common constraints (set once)
        gbc.insets = new Insets(3, 3, 3, 3);
        gbc.fill = GridBagConstraints.HORIZONTAL;

        gbc.gridy = 0;
        gbc.gridx = 1;
        JButton runButton = new JButton("Run");
        left.add(runButton, gbc);

        //left.add(new JLabel("Select OS:", SwingConstants.RIGHT));
        //left.add(osSelector);

        gbc.gridy++;
        gbc.gridx = 0; gbc.weightx = 0.01;
        left.add(new JLabel("Libre Office:", SwingConstants.RIGHT), gbc);
        gbc.gridx = 1; gbc.weightx = 1;
        libreOfficeExecLocation.setPreferredSize(new Dimension(135, libreOfficeExecLocation.getPreferredSize().height));
        left.add(libreOfficeExecLocation, gbc);
        gbc.weightx = 0;

        /*gbc.gridy++;
        gbc.gridx = 0;
        left.add(new JLabel("Input Excel File:", SwingConstants.RIGHT), gbc);
        gbc.gridx = 1;
        left.add(inputExcelFile, gbc);

        gbc.gridy++;
        gbc.gridx = 0;
        left.add(new JLabel("Output Excel File:", SwingConstants.RIGHT), gbc);
        gbc.gridx = 1;
        left.add(outputExcelFile, gbc);*/

        int n = MATLAB_INPUT_VARS.length;
        if (n != MATLAB_INPUT_DEFAULT.length) throw new RuntimeException("what");
        fieldsForMatlab = new JTextField[n];
        for (int i = 0; i < n; i++) {
            gbc.gridy++;
            gbc.gridx = 0;
            left.add(new JLabel(MATLAB_INPUT_VARS[i] + ":", SwingConstants.RIGHT), gbc);
            gbc.gridx = 1;
            fieldsForMatlab[i] = new JTextField();
            fieldsForMatlab[i].setText(String.valueOf(MATLAB_INPUT_DEFAULT[i]));
            left.add(fieldsForMatlab[i], gbc);
        }

        valueFieldsForExcel = new JTextField[COL_NAMES[mode - 1].length];
        for (int i = 0; i < COL_NAMES[mode - 1].length; i++) {
            gbc.gridy++;
            gbc.gridx = 0;
            left.add(new JLabel(COL_NAMES[mode-1][i] + ":", SwingConstants.RIGHT), gbc);
            gbc.gridx = 1;
            valueFieldsForExcel[i] = new JTextField();
            left.add(valueFieldsForExcel[i], gbc);
        }

        // Wrap the left panel in a scroll pane
        JScrollPane leftScroll = new JScrollPane(left,
                JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);

        // ==== Right Panel ====
        previewPanel1 = new PdfPreviewPanel();
        previewPanel2 = new PdfPreviewPanel();
        JSplitPane right = new JSplitPane(JSplitPane.VERTICAL_SPLIT, previewPanel1, previewPanel2);
        right.setResizeWeight(0.5);
        right.setOneTouchExpandable(false);
        //add(right, BorderLayout.CENTER);

        // ==== Whole window =====
        JSplitPane whole = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, leftScroll, right);
        whole.setResizeWeight(0.01);
        whole.setOneTouchExpandable(false);
        add(whole);

        // ==== Initialize ====
        dir = System.getProperty("user.dir").replace("\\", "/") + "/resource";
        System.out.println("Current working directory: " + dir);

        loadInitialValues();
        runButton.addActionListener(this::execute);

        setVisible(true);
    }

    private void loadInitialValues() {
        try (FileInputStream fis = new FileInputStream(dir + "/" + mode + ".xlsx");
             Workbook workbook = new XSSFWorkbook(fis)) {

            Sheet sheet = workbook.getSheetAt(0);
            Row row = sheet.getRow(TARGET_ROW[mode - 1]);
            if (row != null) {
                for (int i = 0; i < COLS[mode - 1].length; i++) {
                    Cell cell = row.getCell(COLS[mode - 1][i]);
                    if (cell != null) {
                        switch (cell.getCellType()) {
                            case STRING:
                                valueFieldsForExcel[i].setText(cell.getStringCellValue());
                                break;
                            case NUMERIC:
                                valueFieldsForExcel[i].setText(Double.toString(cell.getNumericCellValue()));
                                break;
                            case BOOLEAN:
                                valueFieldsForExcel[i].setText(Boolean.toString(cell.getBooleanCellValue()));
                                break;
                            default:
                                valueFieldsForExcel[i].setText("");
                                break;
                        }
                    }
                }
            }
        } catch (Exception ex) {
            JOptionPane.showMessageDialog(this,
                    "Error loading values: " + ex, "Error", JOptionPane.ERROR_MESSAGE);
        }


    }

    private void execute(ActionEvent e) {
        List<List<Double>> values = updateMatlabExcel();
        if (values == null) {
            return;
        }

        // 1. output modified excel
        String outputExcel = dir + "/" + mode + "-out.xlsx";
        try (FileInputStream fis = new FileInputStream(dir + "/" + mode + ".xlsx");
             Workbook workbook = new XSSFWorkbook(fis)) {

            Sheet sheet = workbook.getSheetAt(0);

            // update values from matlab-out.xlsx
            if (!updateValuesFromMatlabExcel(sheet, values)) {
                return;
            }

            // Update values from UI
            Row row = sheet.getRow(TARGET_ROW[mode - 1]);
            for (int i = 0; i < COLS.length; i++) {
                String valStr = valueFieldsForExcel[i].getText().trim();
                Cell cell = row.getCell(COLS[mode - 1][i]);
                try {
                    double val = Double.parseDouble(valStr);
                    cell.setCellValue(val);
                } catch (NumberFormatException nfe) {
                    JOptionPane.showMessageDialog(this, "填入的值不是数字:" + valStr);
                    return;
                }
            }


            workbook.setForceFormulaRecalculation(true);
            //FormulaEvaluator evaluator = workbook.getCreationHelper().createFormulaEvaluator();
            //evaluator.evaluateAll();

            try (FileOutputStream fos = new FileOutputStream(outputExcel)) {
                workbook.write(fos);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
            System.out.println("haha " + ex.getMessage());
            JOptionPane.showMessageDialog(this,
                    "Error during execution: " + ex, "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }

        // Generate two pdf file
        excelToPdf(outputExcel, previewPanel1);
        excelToPdf(dir + "/matlab-out.xlsx", previewPanel2);

        JOptionPane.showMessageDialog(this, "Done!");
    }

    private List<List<Double>> updateMatlabExcel() {
        StringBuilder params = new StringBuilder();
        for (JTextField jTextField : fieldsForMatlab) {
            String valStr = jTextField.getText().trim();
            try {
                Double.parseDouble(valStr);
                params.append(valStr).append(" ");
            } catch (NumberFormatException nfe) {
                JOptionPane.showMessageDialog(this, "填入的值不是数字:" + valStr);
                return null;
            }
        }

        Matlab m = new Matlab(dir);
        return m.runMatlabAndGenExcel(params.toString(), false);
    }

    private boolean updateValuesFromMatlabExcel(Sheet outputSheet, List<List<Double>> values) throws Exception {
        int baseRow = 2;
        for (int i = 0; i < values.size(); i++) {
            List<Double> valuesForRow = values.get(i);
            Row row = outputSheet.getRow(baseRow + i);

            int[] destColumns = excel_out_cols[mode - 1];
            if (valuesForRow.size() != destColumns.length) {
                throw new Exception("array length does not match " + valuesForRow.size() + " vs " + destColumns.length);
            }
            for (int j = 0; j < valuesForRow.size(); j++) {
                Cell cell = row.getCell(destColumns[j]);
                if (cell == null) {
                    throw new Exception("empty cell found in " + mode + ".xlsx at column " + destColumns[j]);
                } else {
                    cell.setCellValue(valuesForRow.get(j));
                }
            }
        }
        return true;
    }

    private void excelToPdf(String excelFilePath, PdfPreviewPanel pdfPanel) {
        File excelFile = new File(excelFilePath);
        String dir = excelFile.getParent();
        if (dir == null)
            dir = ".";
        String pdfFile = excelFile.getName().replaceFirst("[.][^.]+$", "") + ".pdf";

        try {
            ProcessBuilder pb = new ProcessBuilder(
                    libreOfficeExecLocation.getText().trim(),
                    "--headless",          // no GUI
                    "--convert-to", "pdf", // convert to PNG format
                    "--outdir", dir,       // pdf output directory
                    excelFilePath          // input excel file path
            );
            Process process = pb.start();
            int exitCode = process.waitFor();
            if (exitCode != 0) {
                throw new IOException("Conversion failed. Exit code: " + exitCode);
            }

            //JOptionPane.showMessageDialog(this, "Done!");

            pdfPanel.loadPdf(dir + "/" + pdfFile);

        } catch (Exception ex) {
            JOptionPane.showMessageDialog(this,
                    "Error exporting excel charts to pdf: " + ex, "Error", JOptionPane.ERROR_MESSAGE);
        }
    }
}
