#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void updateDatabaseStatus(bool connected, const QString& message = QString());
    void setAuthenticatedUser(const QString& username, int roleId);
    
private slots:
    void onLoginClicked();
    void onLogoutClicked();
    void onRefreshPatientsClicked();
    
private:
    void setupUi();
    void createMenuBar();
    void createStatusBar();
    void loadPatients();
    
    // UI Components
    QLabel* m_statusLabel;
    QLabel* m_dbStatusLabel;
    QLabel* m_userLabel;
    QPushButton* m_loginButton;
    QPushButton* m_logoutButton;
    QPushButton* m_refreshButton;
    QTableWidget* m_patientsTable;
    
    bool m_isAuthenticated;
};

#endif // MAINWINDOW_H
