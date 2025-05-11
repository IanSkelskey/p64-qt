#include "EmulationViewport.h"

#include <QPainter>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QImage>
#include <QVBoxLayout>

namespace QT_UI {

EmulationViewport::EmulationViewport(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_isRunning(false)
{
    // Setup the frame timer for rendering
    m_frameTimer.setInterval(16); // ~60 FPS
    
    // Use SIGNAL/SLOT macro syntax which works better with MOC
    connect(&m_frameTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    
    // Create the "no game running" label
    m_noGameLabel = new QLabel(this);
    m_noGameLabel->setText("No game running");
    m_noGameLabel->setAlignment(Qt::AlignCenter);
    m_noGameLabel->setStyleSheet("font-size: 24px; color: #888888;");
    
    // Default aspect ratio and stretch mode
    m_aspectRatio = 4.0f / 3.0f; // Default N64 ratio
    m_stretchMode = 0; // Normal mode
    
    // Default viewport size
    m_nativeSize = QSizeF(640, 480);
    
    createPlaceholderContent();
    updateViewArea();
}

EmulationViewport::~EmulationViewport()
{
    stopEmulation();
}

bool EmulationViewport::startEmulation()
{
    m_isRunning = true;
    m_noGameLabel->setVisible(false);
    m_frameTimer.start();
    emit emulationStarted();
    return true;
}

bool EmulationViewport::pauseEmulation()
{
    m_frameTimer.stop();
    emit emulationPaused();
    return true;
}

bool EmulationViewport::stopEmulation()
{
    m_frameTimer.stop();
    m_isRunning = false;
    m_noGameLabel->setVisible(true);
    emit emulationStopped();
    return true;
}

bool EmulationViewport::resetEmulation(bool hard)
{
    // Placeholder implementation
    emit emulationReset();
    return true;
}

void EmulationViewport::updateFrame()
{
    // Simple implementation for now
    if (m_isRunning) {
        update(); // Force a redraw
    }
}

void EmulationViewport::updateFrame(const QImage& image)
{
    // Placeholder - would update the viewport with the provided frame
    update();
}

void EmulationViewport::setEmulationRunning(bool running)
{
    m_isRunning = running;
    m_noGameLabel->setVisible(!running);
    
    if (running) {
        m_frameTimer.start();
    } else {
        m_frameTimer.stop();
    }
}

void* EmulationViewport::getWindowHandle()
{
    // Platform-specific implementation
    #ifdef Q_OS_WIN
        return (void*)winId();
    #else
        return nullptr;
    #endif
}

QSize EmulationViewport::sizeHint() const
{
    // Return a reasonable default size for the viewport
    return QSize(640, 480);
}

void EmulationViewport::setAspectRatio(float ratio)
{
    m_aspectRatio = ratio;
    updateViewArea();
    update();
}

void EmulationViewport::setStretchMode(int mode)
{
    m_stretchMode = mode;
    updateViewArea();
    update();
}

void EmulationViewport::updateViewArea()
{
    // Calculate the viewport rectangle based on widget size, aspect ratio, and stretch mode
    m_viewportRect = calculateViewport(width(), height());
    
    // Update the no-game label position
    if (m_noGameLabel) {
        m_noGameLabel->setGeometry(rect());
    }
    
    // Calculate transform for coordinate mapping
    QRectF srcRect(0, 0, m_nativeSize.width(), m_nativeSize.height());
    m_screenToEmuTransform = QTransform();
    m_screenToEmuTransform.scale(
        srcRect.width() / m_viewportRect.width(),
        srcRect.height() / m_viewportRect.height()
    );
    m_screenToEmuTransform.translate(-m_viewportRect.x(), -m_viewportRect.y());
}

QPointF EmulationViewport::screenToEmulationCoords(const QPointF& point)
{
    return m_screenToEmuTransform.map(point);
}

QRect EmulationViewport::calculateViewport(int width, int height)
{
    if (width <= 0 || height <= 0) {
        return QRect(0, 0, 0, 0);
    }
    
    QRect result;
    
    switch (m_stretchMode) {
        case 1: // Stretch
            result = QRect(0, 0, width, height);
            break;
        
        case 2: // Fullscreen
            // In this mode, we fill the screen and potentially crop
            if (width / (float)height > m_aspectRatio) {
                // Window is wider than the aspect ratio
                int targetWidth = height * m_aspectRatio;
                int xOffset = (width - targetWidth) / 2;
                result = QRect(xOffset, 0, targetWidth, height);
            } else {
                // Window is taller than the aspect ratio
                int targetHeight = width / m_aspectRatio;
                int yOffset = (height - targetHeight) / 2;
                result = QRect(0, yOffset, width, targetHeight);
            }
            break;
            
        case 0: // Normal (default)
        default:
            // Keep aspect ratio, fit inside window
            if (width / (float)height > m_aspectRatio) {
                // Window is wider than the aspect ratio
                int targetWidth = height * m_aspectRatio;
                int xOffset = (width - targetWidth) / 2;
                result = QRect(xOffset, 0, targetWidth, height);
            } else {
                // Window is taller than the aspect ratio
                int targetHeight = width / m_aspectRatio;
                int yOffset = (height - targetHeight) / 2;
                result = QRect(0, yOffset, width, targetHeight);
            }
            break;
    }
    
    return result;
}

void EmulationViewport::createPlaceholderContent()
{
    // Just show the "No game running" label initially
    m_noGameLabel->setVisible(true);
    updateLayout();
}

void EmulationViewport::updateLayout()
{
    if (m_noGameLabel) {
        m_noGameLabel->setGeometry(rect());
    }
}

// Event handlers
void EmulationViewport::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void EmulationViewport::resizeGL(int width, int height)
{
    updateViewArea();
}

void EmulationViewport::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (!m_isRunning) {
        // Nothing to paint if not running
        return;
    }
    
    // Minimal implementation - real code would draw the frame from the emulator
    glViewport(
        m_viewportRect.x(),
        m_viewportRect.y(),
        m_viewportRect.width(),
        m_viewportRect.height()
    );
}

void EmulationViewport::paintEvent(QPaintEvent* event)
{
    // Let QOpenGLWidget handle the painting
    QOpenGLWidget::paintEvent(event);
}

void EmulationViewport::resizeEvent(QResizeEvent* event)
{
    updateViewArea();
    QOpenGLWidget::resizeEvent(event);
}

void EmulationViewport::keyPressEvent(QKeyEvent* event)
{
    // In real implementation, map keyboard input to emulator controls
    QOpenGLWidget::keyPressEvent(event);
}

void EmulationViewport::keyReleaseEvent(QKeyEvent* event)
{
    // In real implementation, map keyboard input to emulator controls
    QOpenGLWidget::keyReleaseEvent(event);
}

void EmulationViewport::mousePressEvent(QMouseEvent* event)
{
    // In real implementation, handle mouse input for emulator
    QOpenGLWidget::mousePressEvent(event);
}

void EmulationViewport::mouseReleaseEvent(QMouseEvent* event)
{
    // In real implementation, handle mouse input for emulator
    QOpenGLWidget::mouseReleaseEvent(event);
}

void EmulationViewport::mouseMoveEvent(QMouseEvent* event)
{
    // In real implementation, handle mouse input for emulator
    QOpenGLWidget::mouseMoveEvent(event);
}

} // namespace QT_UI
