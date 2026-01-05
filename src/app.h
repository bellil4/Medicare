#ifndef APP_H
#define APP_H

#include <QObject>
#include <QString>

class MainWindow;

class App : public QObject {
    Q_OBJECT
    
public:
    explicit App(QObject *parent = nullptr);
    ~App();
    
    bool initialize();
    void run();
    void shutdown();
    
private:
    bool connectToDatabase();
    void showMainWindow();
    
    MainWindow* m_mainWindow;
    bool m_initialized;
};

#endif // APP_H
