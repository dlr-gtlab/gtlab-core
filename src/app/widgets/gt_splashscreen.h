#ifndef GTSPLASHSCREEN_H
#define GTSPLASHSCREEN_H

#include <QWidget>

class QMovie;
class SplashLabel;

/**
 * @brief The GtSplashScreen class is a custom implementation of a splash screen
 *  based on the logic of QSplashScreen. It gives the possibility to display
 *  animated graphics.
 */
class GtSplashScreen : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constrcutor
     * @param showBorder Indicator to activate/deactivate the border around the
     * splash screen image
     */
    GtSplashScreen(bool showBorder = false, bool showMessage = false);

    /**
     * @brief Shows given message in the header of the splash screen.
     * application events are performed afterwards.
     * @param msg
     */
    void showMessage(const QString& msg);

    /**
     * @brief Should be called after given widget (main application window) is
     * constructed. Handles some events and closes automatically the splash
     * screen widget properly.
     * @param wid Main application window
     */
    void finish(QWidget* wid);

    /**
     * @brief Should be called to process events and update the animated
     * graphic. Can also be used to perform a small delay in the execution of
     * the application. During the delay, the splash screen animation will
     * continue to be displayed.
     * @param msecs Delay in milliseconds
     */
    void processEvents(int msecs = 100);

    /**
     * @brief Processes a task and adds some delay
     * @param f
     * @param delayMsecs
     */
    template <typename Func>
    void process(Func&& f, int delayMsecs = 100)
    {
        processEvents();
        f();
        processEvents(delayMsecs);
    }

    /**
     * @brief Processes a task, prints a message and adds some delay
     * @param message Message to be printed in splash screen
     * @param f
     * @param delayMsecs
     */
    template <typename Func>
    void process(const QString& message, Func&& f, int delayMsecs = 100)
    {
        showMessage(message);
        process(f, delayMsecs);
    }

private:
    /// Animated graphics
    QMovie* m_movie;

    /// Label to display the animated image
    SplashLabel* m_processLabel;

};

#endif // GTSPLASHSCREEN_H
