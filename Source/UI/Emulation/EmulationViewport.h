#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QLabel>

namespace QT_UI {

/**
 * @brief The EmulationViewport class provides a display surface for N64 emulation.
 * 
 * This widget serves as the rendering target for the graphics plugin, displaying
 * the game content and handling input events.
 */
class EmulationViewport : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    
public:
    explicit EmulationViewport(QWidget* parent = nullptr);
    ~EmulationViewport();
    
    /**
     * @brief Returns a handle to the widget that can be used by plugins.
     * @return Platform-specific window handle (HWND on Windows)
     */
    void* getWindowHandle();
    
    /**
     * @brief Updates the viewport with a new frame.
     * @param image The frame to display
     */
    void updateFrame(const QImage& image);
    
    /**
     * @brief Sets whether the emulation is running.
     * @param running True if emulation is running, false otherwise
     */
    void setEmulationRunning(bool running);
    
    bool startEmulation();
    bool pauseEmulation();
    bool stopEmulation();
    bool resetEmulation(bool hard = false);

    // Add the missing method declarations
    QSize sizeHint() const override;
    void setAspectRatio(float ratio);
    void setStretchMode(int mode);
    
signals:
    void emulationStarted();
    void emulationPaused();
    void emulationStopped();
    void emulationReset();
    
protected:
    // Event handlers
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

public slots:
    void updateFrame(); // Make sure this exists
    
private:
    // Update the view area based on current stretch mode and aspect ratio
    void updateViewArea();
    
    // Convert screen coordinates to emulation coordinates
    QPointF screenToEmulationCoords(const QPointF& point);
    
    // Calculate optimal viewport dimensions
    QRect calculateViewport(int width, int height);

    // Frame timer for rendering
    QTimer m_frameTimer;
    
    // Emulation state
    bool m_isRunning;
    
    // Display settings
    float m_aspectRatio;
    int m_stretchMode; // 0 = normal, 1 = stretch, 2 = fullscreen
    
    // Viewport dimensions
    QRect m_viewportRect;
    QSizeF m_nativeSize;
    
    // Transform for coordinate mapping
    QTransform m_screenToEmuTransform;
    
    // "No game running" label
    QLabel* m_noGameLabel;

    void createPlaceholderContent();
    void updateLayout();
};

} // namespace QT_UI
