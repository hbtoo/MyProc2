import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.rendering.PDFRenderer;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

public class PdfPreviewPanel extends JPanel {

    private JLabel pdfLabel;
    private BufferedImage originalImage;
    private double zoomFactor;

    private JLayeredPane layeredPane;
    private JScrollPane scrollPane;
    private JPanel controls;

    public PdfPreviewPanel() {
        setLayout(new BorderLayout());

        // ===== Layered pane =====
        layeredPane = new JLayeredPane();
        add(layeredPane, BorderLayout.CENTER);

        // ===== PDF label + scroll pane (base layer) =====
        pdfLabel = new JLabel();
        pdfLabel.setHorizontalAlignment(JLabel.LEFT);
        pdfLabel.setVerticalAlignment(JLabel.TOP);

        scrollPane = new JScrollPane(pdfLabel);
        layeredPane.add(scrollPane, JLayeredPane.DEFAULT_LAYER);

        // Mouse wheel zooming
        scrollPane.addMouseWheelListener(new MouseAdapter() {
            @Override
            public void mouseWheelMoved(MouseWheelEvent e) {
                if (e.isControlDown()) {
                    if (e.getWheelRotation() < 0) {
                        zoomIn();
                    } else {
                        zoomOut();
                    }
                    e.consume();
                }
            }
        });

        // ===== Overlay controls (top layer) =====
        controls = new JPanel();
        controls.setLayout(new BoxLayout(controls, BoxLayout.Y_AXIS));
        controls.setOpaque(false); // transparent overlay

        JButton resetBtn = new JButton("Reset");
        JButton zoomInBtn = new JButton("+");
        JButton zoomOutBtn = new JButton("-");

        resetBtn.addActionListener(e -> reset());
        zoomInBtn.addActionListener(e -> zoomIn());
        zoomOutBtn.addActionListener(e -> zoomOut());

        zoomInBtn.setAlignmentX(Component.CENTER_ALIGNMENT);
        zoomOutBtn.setAlignmentX(Component.CENTER_ALIGNMENT);
        resetBtn.setAlignmentX(Component.CENTER_ALIGNMENT);

        controls.add(resetBtn);
        controls.add(Box.createVerticalStrut(8));
        controls.add(zoomInBtn);
        controls.add(Box.createVerticalStrut(8));
        controls.add(zoomOutBtn);


        layeredPane.add(controls, JLayeredPane.PALETTE_LAYER);

        // ===== Handle resizing =====
        addComponentListener(new ComponentAdapter() {
            @Override
            public void componentResized(ComponentEvent e) {
                layoutLayers();
            }
        });
    }

    // Position layers manually
    private void layoutLayers() {
        int w = getWidth();
        int h = getHeight();

        scrollPane.setBounds(0, 0, w, h);

        int controlsWidth = 70;
        int controlsHeight = 120;
        controls.setBounds(
                w - controlsWidth - 20, // right margin
                5,                     // top margin
                controlsWidth,
                controlsHeight
        );
    }

    public void loadPdf(String pdfPath) throws IOException {
        try (PDDocument document = PDDocument.load(new File(pdfPath))) {
            PDFRenderer renderer = new PDFRenderer(document);
            originalImage = renderer.renderImageWithDPI(0, 300);
        }
        reset();
    }

    private void updateImage() {
        if (originalImage == null) return;

        int newW = (int) (originalImage.getWidth() * zoomFactor);
        int newH = (int) (originalImage.getHeight() * zoomFactor);

        if (newW <= 0 || newH <= 0) return;

        Image scaled = originalImage.getScaledInstance(
                newW, newH, Image.SCALE_SMOOTH
        );

        pdfLabel.setIcon(new ImageIcon(scaled));
        pdfLabel.revalidate();
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
        if (originalImage == null) return;

        zoomFactor = (double) getWidth() / originalImage.getWidth();
        updateImage();
    }
}
