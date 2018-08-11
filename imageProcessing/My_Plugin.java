import ij.*;
import ij.process.*;
import ij.gui.*;
import java.awt.*;
import java.io.*;

import ij.plugin.*;

public class My_Plugin implements PlugIn {

  boolean firstOut;

  public void processImage(String inputPath, String outputPath, String fileName,
      BufferedWriter bw) throws IOException {

    ImagePlus imp = IJ.openImage(inputPath + "/" + fileName);

    IJ.run(imp, "16-bit", "");
    IJ.run(imp, "Subtract Background...", "rolling=20 light");
    IJ.setAutoThreshold(imp, "Default");
    IJ.setRawThreshold(imp, 0, 65520, null);

    Prefs.blackBackground = false;
    IJ.run(imp, "Convert to Mask", "");
    IJ.run(imp, "Watershed", "");
    IJ.run("Set Measurements...",
        "area mean standard min perimeter integrated median area_fraction redirect=None decimal=3");

    IJ.run(imp, "Analyze Particles...",
        "size=100-Infinity show=Overlay include clear summarize");

    IJ.save(imp, outputPath + "/" + fileName + "_overlay.jpeg");

    String summaryFile = outputPath + "/" + fileName + "_summary.csv";
    IJ.saveAs("Results", summaryFile);
    IJ.run("Close");
    IJ.saveAs("Results", outputPath + "/" + fileName + "_results.csv");

    
    IJ.run(imp, "Analyze Particles...",
        "size=100-Infinity show=Outlines include clear");

    //IJ.saveAs("Results", outputPath + "/" + fileName + "_results2.csv");
    IJ.save(outputPath + "/" + fileName + "_outlines.jpeg");
    IJ.run("Close");

    if (bw != null) {
      BufferedReader br =
          new BufferedReader(new FileReader(new File(summaryFile)));
      String line;
      int i = 0;
      while ((line = br.readLine()) != null) {
        if (i > 1) {
          IJ.showMessage(
              "summary file " + summaryFile + " has more than two lines");
        } else if (firstOut || i == 1) {
          bw.write(line);
          bw.newLine();
        }
        i++;
      }
      firstOut = false;
      br.close();
    }
  }

  public void run(String arg) {
    String outputDir = "C:/ImageJ/output/";
    String inputDir = "C:/ImageJ/input/";

    File file = new File(outputDir);
    if (!file.exists()) {
      file.mkdirs();
    }
    file = new File(inputDir);
    if (!file.exists()) {
      file.mkdirs();
    }

    firstOut = true;
    try {
      FileOutputStream fos =
          new FileOutputStream(new File("C:/ImageJ/output/all_summary.csv"));
      BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(fos));

      File folder = new File(inputDir);
      File[] listOfFiles = folder.listFiles();
      for (int i = 0; i < listOfFiles.length; i++) {
        if (listOfFiles[i].isFile()) {
          processImage(inputDir, outputDir, listOfFiles[i].getName(), bw);
        }
      }
      bw.close();
    } catch (Exception e) {
      IJ.showMessage("Exception happened: " + e.getMessage());
    }
  }

}
