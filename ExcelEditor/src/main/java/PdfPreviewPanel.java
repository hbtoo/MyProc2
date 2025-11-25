import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.rendering.PDFRenderer;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseWheelEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

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
        try (PDDocument document = PDDocument.load(new File(pdfPath))) {
            PDFRenderer renderer = new PDFRenderer(document);
            originalImage = renderer.renderImageWithDPI(0, 300); // First page, good DPI
        }
        reset();
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
        //double z2 = (double) getHeight() / originalImage.getHeight();
        //zoomFactor = Math.min(z1, z2);
        updateImage();
    }
}
