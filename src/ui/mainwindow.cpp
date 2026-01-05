#include "ui/mainwindow.h"
#include "auth/authmanager.h"
#include "repositories/patientrepository.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_isAuthenticated(false) {
    setupUi();
    createMenuBar();
    createStatusBar();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUi() {
    setWindowTitle("Medical Consultation Management System");
    resize(900, 600);
    
    // Create central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Database status section
    QHBoxLayout* statusLayout = new QHBoxLayout();
    QLabel* dbLabel = new QLabel("Database:", this);
    m_dbStatusLabel = new QLabel("Not Connected", this);
    m_dbStatusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    statusLayout->addWidget(dbLabel);
    statusLayout->addWidget(m_dbStatusLabel);
    statusLayout->addStretch();
    mainLayout->addLayout(statusLayout);
    
    // User status section
    QHBoxLayout* userLayout = new QHBoxLayout();
    m_userLabel = new QLabel("Not logged in", this);
    m_userLabel->setStyleSheet("QLabel { font-weight: bold; }");
    m_loginButton = new QPushButton("Login", this);
    m_logoutButton = new QPushButton("Logout", this);
    m_logoutButton->setEnabled(false);
    
    connect(m_loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(m_logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    
    userLayout->addWidget(m_userLabel);
    userLayout->addStretch();
    userLayout->addWidget(m_loginButton);
    userLayout->addWidget(m_logoutButton);
    mainLayout->addLayout(userLayout);
    
    // Patients table section
    QLabel* patientsLabel = new QLabel("Patients:", this);
    patientsLabel->setStyleSheet("QLabel { font-size: 14pt; font-weight: bold; }");
    mainLayout->addWidget(patientsLabel);
    
    m_patientsTable = new QTableWidget(this);
    m_patientsTable->setColumnCount(6);
    m_patientsTable->setHorizontalHeaderLabels(QStringList() 
        << "ID" << "First Name" << "Last Name" << "Date of Birth" << "Gender" << "Phone");
    m_patientsTable->horizontalHeader()->setStretchLastSection(true);
    m_patientsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_patientsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_patientsTable);
    
    // Refresh button
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_refreshButton = new QPushButton("Refresh Patients", this);
    m_refreshButton->setEnabled(false);
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshPatientsClicked);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_refreshButton);
    mainLayout->addLayout(buttonLayout);
    
    // Status label
    m_statusLabel = new QLabel("Ready", this);
    mainLayout->addWidget(m_statusLabel);
}

void MainWindow::createMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    QMenu* fileMenu = menuBar->addMenu("&File");
    QAction* exitAction = fileMenu->addAction("E&xit");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    QMenu* helpMenu = menuBar->addMenu("&Help");
    QAction* aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About",
            "Medical Consultation Management System v1.0\n\n"
            "A secure, auditable, Oracle-backed MVC application\n"
            "for managing medical consultations.\n\n"
            "Phase A: Database, Auth, Audit, Patient Management");
    });
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage("Ready");
}

void MainWindow::updateDatabaseStatus(bool connected, const QString& message) {
    if (connected) {
        m_dbStatusLabel->setText("Connected");
        m_dbStatusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        statusBar()->showMessage(message.isEmpty() ? "Database connected successfully" : message);
    } else {
        m_dbStatusLabel->setText("Not Connected");
        m_dbStatusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        statusBar()->showMessage(message.isEmpty() ? "Database not connected" : message);
    }
}

void MainWindow::setAuthenticatedUser(const QString& username, int roleId) {
    m_isAuthenticated = true;
    
    QString roleText;
    switch(roleId) {
        case 1: roleText = "Admin"; break;
        case 2: roleText = "Doctor"; break;
        case 3: roleText = "Nurse"; break;
        case 4: roleText = "Receptionist"; break;
        default: roleText = "Unknown"; break;
    }
    
    m_userLabel->setText(QString("Logged in as: %1 (%2)").arg(username).arg(roleText));
    m_loginButton->setEnabled(false);
    m_logoutButton->setEnabled(true);
    m_refreshButton->setEnabled(true);
    
    // Load patients after successful login
    loadPatients();
}

void MainWindow::onLoginClicked() {
    bool ok;
    QString username = QInputDialog::getText(this, "Login",
                                            "Username:", QLineEdit::Normal,
                                            "", &ok);
    if (!ok || username.isEmpty()) {
        return;
    }
    
    QString password = QInputDialog::getText(this, "Login",
                                            "Password:", QLineEdit::Password,
                                            "", &ok);
    if (!ok || password.isEmpty()) {
        return;
    }
    
    if (AuthManager::instance().authenticate(username, password)) {
        setAuthenticatedUser(AuthManager::instance().currentUsername(),
                           AuthManager::instance().currentUserRoleId());
        statusBar()->showMessage("Login successful");
        QMessageBox::information(this, "Login", "Login successful!");
    } else {
        QMessageBox::warning(this, "Login Failed", 
                           "Authentication failed: " + AuthManager::instance().lastError());
        statusBar()->showMessage("Login failed");
    }
}

void MainWindow::onLogoutClicked() {
    AuthManager::instance().logout();
    
    m_isAuthenticated = false;
    m_userLabel->setText("Not logged in");
    m_loginButton->setEnabled(true);
    m_logoutButton->setEnabled(false);
    m_refreshButton->setEnabled(false);
    
    // Clear patients table
    m_patientsTable->setRowCount(0);
    
    statusBar()->showMessage("Logged out");
    QMessageBox::information(this, "Logout", "Logged out successfully");
}

void MainWindow::onRefreshPatientsClicked() {
    loadPatients();
}

void MainWindow::loadPatients() {
    if (!m_isAuthenticated) {
        statusBar()->showMessage("Please login first");
        return;
    }
    
    m_statusLabel->setText("Loading patients...");
    
    QList<Patient> patients = PatientRepository::instance().findAll(false);
    
    m_patientsTable->setRowCount(patients.size());
    
    for (int i = 0; i < patients.size(); ++i) {
        const Patient& patient = patients[i];
        
        m_patientsTable->setItem(i, 0, new QTableWidgetItem(QString::number(patient.patientId())));
        m_patientsTable->setItem(i, 1, new QTableWidgetItem(patient.firstName()));
        m_patientsTable->setItem(i, 2, new QTableWidgetItem(patient.lastName()));
        m_patientsTable->setItem(i, 3, new QTableWidgetItem(patient.dateOfBirth().toString("yyyy-MM-dd")));
        m_patientsTable->setItem(i, 4, new QTableWidgetItem(patient.gender()));
        m_patientsTable->setItem(i, 5, new QTableWidgetItem(patient.phone()));
    }
    
    m_statusLabel->setText(QString("Loaded %1 patient(s)").arg(patients.size()));
    statusBar()->showMessage(QString("Loaded %1 patient(s)").arg(patients.size()));
}
