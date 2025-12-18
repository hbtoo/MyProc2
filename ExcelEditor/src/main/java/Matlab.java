import org.apache.commons.collections4.map.SingletonMap;
import org.apache.poi.ss.usermodel.*;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;

import javax.swing.*;
import java.io.*;
import java.net.Socket;
import java.util.AbstractMap;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class Matlab {

    String dir;

    public Matlab(String dir) {
        this.dir = dir;
    }

    private void trySend(String params) throws InterruptedException {
        int retries = 30;
        while (retries-- > 0) {
            try (Socket s = new Socket("localhost", 5001)) {
                PrintWriter out = new PrintWriter(s.getOutputStream(), true);
                BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));

                System.out.println("sending request to matlab");
                out.println(params);
                System.out.println("got " + in.readLine());
                return;
            } catch (Exception e) {
                Thread.sleep(500); // wait 0.5 second
            }
        }
        throw new RuntimeException("MATLAB server did not start");
    }

    public List<List<Double>> runMatlabAndGenExcel(String params, boolean skipMatlab) {
        if (!skipMatlab) {
            try {
                File outputExcel = new File(dir + "/R_t.xlsx");
                outputExcel.delete();
                outputExcel = new File(dir + "/t.xlsx");
                outputExcel.delete();

                // Command to launch MATLAB and run start_km_server
                ProcessBuilder pb = new ProcessBuilder(
                        "matlab",                // or full path to matlab.exe if not in PATH
                        "-nodesktop",
                        "-nosplash",
                        "-minimize",
                        "-r",
                        //"-batch",
                        "run('" + dir + "/start_km_server.m')"   // command inside MATLAB
                );
                pb.redirectErrorStream(true); // merge stdout and stderr
                Process process = pb.start();

                trySend(params);

                // Optional: read MATLAB output
                BufferedReader reader = new BufferedReader(
                        new InputStreamReader(process.getInputStream())
                );
                String line;
                while ((line = reader.readLine()) != null) {
                    System.out.println("matlab output line: " + line);
                }

                // Wait for MATLAB to exit (optional)
                int exitCode = process.waitFor();
                System.out.println("MATLAB exited with code: " + exitCode);
            } catch (Exception e) {
                JOptionPane.showMessageDialog(null, "调用Matlab失败: " + e.getMessage());
                return null;
            }
        }

        List<Map.Entry<Integer, List<Double>>> tValues =
                getColumnValues(dir + "/t.xlsx", List.of(0), 0, false);
        List<Map.Entry<Integer, List<Double>>> r_tValues =
                getColumnValues(dir + "/R_t.xlsx", List.of(0), 0, false);

        if (!updateMatlabExcel(tValues, r_tValues)) {
            return null;
        }

        return getTKLines();
    }

    // Rows of (row number, row column values)
    private List<Map.Entry<Integer, List<Double>>> getColumnValues(
            String filePath, List<Integer> columnIndices, int beginRow, boolean breakAtNull) {
        List<Map.Entry<Integer, List<Double>>> ret = new ArrayList<>();
        try (FileInputStream fis = new FileInputStream(filePath); Workbook workbook = new XSSFWorkbook(fis)) {
            Sheet sheet = workbook.getSheetAt(0);
            int n = 0, e = 0;
            for (Row row : sheet) {
                if (n < beginRow) {
                    n++;
                    continue;
                }
                List<Double> rowValues = new ArrayList<>();
                for (int columnIndex : columnIndices) {
                    Cell cell = row.getCell(columnIndex, Row.MissingCellPolicy.RETURN_BLANK_AS_NULL);
                    if (cell == null) {
                        e++;
                        if (breakAtNull) {
                            System.out.println("Read up to row " + n + " with " + e + " errors from " + filePath);
                            return ret;
                        }
                        rowValues.add(null);
                        throw new Exception("null cell found in column " + columnIndex + ", row " + n + " in " + filePath);
                    } else {
                        if (cell.getCellType() == CellType.NUMERIC || cell.getCellType() == CellType.FORMULA) {
                            rowValues.add(cell.getNumericCellValue());
                        } else {
                            rowValues.add(null);
                            e++;
                        }
                    }
                }
                ret.add(new AbstractMap.SimpleEntry<>(n, rowValues));
                n++;
            }
            System.out.println("Read up to row " + n + " with " + e + " errors from " + filePath);
        } catch (Exception ex) {
            JOptionPane.showMessageDialog(null,
                    "Get column values error: " + ex, "Error", JOptionPane.ERROR_MESSAGE);
            return null;
        }
        return ret;
    }

    private boolean updateMatlabExcel(
            List<Map.Entry<Integer, List<Double>>> tValues, List<Map.Entry<Integer, List<Double>>> r_tValues) {
        try (FileInputStream fis = new FileInputStream(dir + "/matlab.xlsx");
             Workbook workbook = new XSSFWorkbook(fis)) {

            Sheet sheet = workbook.getSheetAt(0);
            int n = 0, e = 0;
            int beginRow = 4;
            for (Row row : sheet) {
                if (n < beginRow) {
                    n++;
                    continue;
                }
                Cell cell = row.getCell(0);
                if (cell == null) cell = row.createCell(0);
                cell.setCellValue(tValues.get(n - beginRow).getValue().get(0));

                cell = row.getCell(1);
                if (cell == null) cell = row.createCell(1);
                cell.setCellValue(r_tValues.get(n - beginRow).getValue().get(0));

                n++;
            }

            //workbook.setForceFormulaRecalculation(true);
            FormulaEvaluator evaluator = workbook.getCreationHelper().createFormulaEvaluator();
            evaluator.evaluateAll();

            try (FileOutputStream fos = new FileOutputStream(dir + "/matlab-out.xlsx")) {
                workbook.write(fos);
            }
            return true;
        } catch (Exception ex) {
            JOptionPane.showMessageDialog(null,
                    "Error generating matlab output excel " + ex, "Error", JOptionPane.ERROR_MESSAGE);
            return false;
        }
    }

    public List<List<Double>> getTKLines() {
        List<Map.Entry<Integer, List<Double>>> tkValues = getColumnValues(dir + "/matlab-out.xlsx",
                ExcelEditorPanel.matlab_out_cols, 4, true);
        int tkIndex = 3;
        Map.Entry<Integer, List<Double>> min = null, max = null;
        for (Map.Entry<Integer, List<Double>> entry : tkValues) {
            if (min == null || min.getValue().get(tkIndex) > entry.getValue().get(tkIndex)) {
                min = entry;
            }
            if (max == null || max.getValue().get(tkIndex) < entry.getValue().get(tkIndex)) {
                max = entry;
            }
        }
        System.out.println("min at " + min.getKey() + ": " + min.getValue());
        System.out.println("max at " + max.getKey() + ": " + max.getValue());

        int mode = 0;  // 0 decreasing, 1 increasing
        Map.Entry<Integer, List<Double>> last = null;
        for (Map.Entry<Integer, List<Double>> entry : tkValues) {
            if (last == null) {
                last = entry;
                continue;
            }

            if (mode == 0) {
                if (last.getValue().get(tkIndex) < entry.getValue().get(tkIndex)) {
                    min = last;
                    mode = 1;   // started increasing
                }
            } else {  // mode = 1
                if (last.getValue().get(tkIndex) > entry.getValue().get(tkIndex)) {
                    max = last;
                    break;      // started decreasing
                }
            }
            last = entry;
        }

        System.out.println("min at " + min.getKey() + ": " + min.getValue());
        System.out.println("max at " + max.getKey() + ": " + max.getValue());

        int n = 8, i = 0;
        double thres = min.getValue().get(tkIndex) + (max.getValue().get(tkIndex) - min.getValue().get(tkIndex)) / n;
        List<List<Double>> selectedRows = new ArrayList<>();
        for (Map.Entry<Integer, List<Double>> entry : tkValues) {
            if (entry.getKey() < min.getKey()) {
                continue;
            }
            if (entry.getValue().get(tkIndex) >= thres) {
                selectedRows.add(entry.getValue());
                System.out.println(entry.getKey() + " - " + entry.getValue());
                i++;
                if (i + 1 == n) {
                    selectedRows.add(max.getValue());
                    System.out.println(max.getKey() + " - " + max.getValue());
                    break;
                } else
                    thres += (max.getValue().get(tkIndex) - min.getValue().get(tkIndex)) / n;
            }
        }
        return selectedRows;
    }
}
