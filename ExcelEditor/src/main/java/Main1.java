import org.apache.poi.ss.usermodel.*;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseWheelEvent;
import java.awt.image.BufferedImage;
import java.io.*;

import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.rendering.PDFRenderer;

public class Main1 extends JFrame {
    boolean mac = false;  // default windows
    private JTextField libreOfficeExecLocation;
    private JTextField inputExcelFile;
    private JTextField outputExcelFile;
    private JTextField[] valueFields = new JTextField[8];
    private PdfPreviewPanel previewPanel;

    private final int TARGET_ROW = 2;
    private final int[] COLS = {28, 29, 30, 31, 32, 33, 34, 35};
    private final String[] COL_NAMES = {
            "n(H2O)", "n(OH)", "n(H)", "n(O2)", "n(O)", "n(H2)", "n(H2O2)", "n(HO2)"
    };

    // PDF Preview Panel
    public class PdfPreviewPanel extends JPanel {
        private JLabel pdfLabel;
        private BufferedImage originalImage;
        private double zoomFactor;

        public PdfPreviewPanel() {
            setLayout(new BorderLayout());
            //setPreferredSize(new Dimension(700, 540));

            pdfLabel = new JLabel();
            pdfLabel.setHorizontalAlignment(JLabel.LEFT);
            pdfLabel.setVerticalAlignment(JLabel.TOP);

            JScrollPane scrollPane = new JScrollPane(pdfLabel);
            add(scrollPane, BorderLayout.CENTER);

            // Mouse wheel zooming
            scrollPane.addMouseWheelListener(new MouseAdapter() {
                @Override
                public void mouseWheelMoved(MouseWheelEvent e) {
                    if (e.isControlDown()) { // Zoom when Ctrl+wheel
                        if (e.getWheelRotation() < 0) {
                            zoomIn();
                        } else {
                            zoomOut();
                        }
                        e.consume();
                    }
                }
            });

            // Zoom controls panel
            JPanel controls = new JPanel();
            JButton zoomInBtn = new JButton("+");
            JButton zoomOutBtn = new JButton("-");
            JButton resetBtn = new JButton("Reset");

            zoomInBtn.addActionListener(e -> zoomIn());
            zoomOutBtn.addActionListener(e -> zoomOut());
            resetBtn.addActionListener(e -> reset());

            controls.add(zoomInBtn);
            controls.add(zoomOutBtn);
            controls.add(resetBtn);

            add(controls, BorderLayout.SOUTH);
        }

        public void loadPdf(String pdfPath) throws IOException {
            PDDocument document = PDDocument.load(new File(pdfPath));
            PDFRenderer renderer = new PDFRenderer(document);
            originalImage = renderer.renderImageWithDPI(0, 300); // First page, good DPI
            zoomFactor = (double) getWidth() / originalImage.getWidth();
            updateImage();
        }

        private void updateImage() {
            if (originalImage != null) {
                int newW = (int) (originalImage.getWidth() * zoomFactor);
                int newH = (int) (originalImage.getHeight() * zoomFactor);
                if (newW <=0 || newH <= 0) {
                    return;
                }
                Image scaled = originalImage.getScaledInstance(newW, newH, Image.SCALE_SMOOTH);
                pdfLabel.setIcon(new ImageIcon(scaled));
                pdfLabel.revalidate();
            }
        }

        private void zoomIn() {
            zoomFactor *= 1.2;
            updateImage();
        }

        private void zoomOut() {
            zoomFactor /= 1.2;
            updateImage();
        }

        private void reset() {
            zoomFactor = (double) getWidth() / originalImage.getWidth();
            updateImage();
        }
    }

    public Main1() {
        setTitle("Excel Species Editor");
        setSize(1000, 800);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
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
        inputExcelFile = new JTextField(mac ? "/Users/bohuang/Downloads/1.xlsx" : "1.xlsx");
        outputExcelFile = new JTextField(mac ? "/Users/bohuang/Downloads/2.xlsx" : "2.xlsx");

        // === Row 0 ===
        gbc.gridx = 0; gbc.gridy = 0; gbc.weightx = 0.1; // narrow
        //topPanel.add(new JLabel("Select OS:", SwingConstants.RIGHT), gbc);

        gbc.gridx = 1; gbc.gridy = 0; gbc.weightx = 0.1;
        //topPanel.add(osSelector, gbc);

        gbc.gridx = 2; gbc.gridy = 0; gbc.weightx = 0.3; // wider label
        topPanel.add(new JLabel("Libre Office Install Location:", SwingConstants.RIGHT), gbc);

        gbc.gridx = 3; gbc.gridy = 0; gbc.weightx = 0.5;
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

        osSelector.addActionListener(e -> {
            mac = osSelector.getSelectedIndex() == 1;
            libreOfficeExecLocation.setText(mac ? libreOfficePathMac : libreOfficePathWindows);
            inputExcelFile.setText(mac ? "/Users/bohuang/Downloads/1.xlsx" : "1.xlsx");
            outputExcelFile.setText(mac ? "/Users/bohuang/Downloads/2.xlsx" : "2.xlsx");
        });

        // ==== Center Panel ====
        JPanel centerPanel = new JPanel(new GridLayout(1, 2, 20, 5));
        JPanel leftPanel = new JPanel(new GridBagLayout());
        JPanel rightPanel = new JPanel(new GridBagLayout());
        gbc = new GridBagConstraints();
        gbc.insets = new Insets(4, 4, 4, 4);
        gbc.fill = GridBagConstraints.HORIZONTAL;

        for (int i = 0; i < COLS.length; i++) {
            JLabel label = new JLabel(COL_NAMES[i] + ":", SwingConstants.RIGHT);
            valueFields[i] = new JTextField();
            //valueFields[i].setPreferredSize(new Dimension(250, 25));

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
        setLocationRelativeTo(null);
        setVisible(true);
    }

    private void loadInitialValues() {
        try (FileInputStream fis = new FileInputStream(inputExcelFile.getText());
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
        String outputExcel = outputExcelFile.getText().trim();

        try (FileInputStream fis = new FileInputStream(inputExcelFile.getText());
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

            // workbook.setForceFormulaRecalculation(true);
            FormulaEvaluator evaluator = workbook.getCreationHelper().createFormulaEvaluator();
            evaluator.evaluateAll();

            try (FileOutputStream fos = new FileOutputStream(outputExcel)) {
                workbook.write(fos);
            }

            //JOptionPane.showMessageDialog(this, "Excel file written:\n" + outputExcel);

        } catch (Exception ex) {
            ex.printStackTrace();
            JOptionPane.showMessageDialog(this, "Error writing file: " + ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }

        String dir = mac ? "/Users/bohuang/Downloads/" : ".";
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

    public static void main(String[] args) throws Exception {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                new Main1();
            }
        });

    }
}
