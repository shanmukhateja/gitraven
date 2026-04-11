#ifndef RAVENLHSVIEW_H
#define RAVENLHSVIEW_H

#include "raventree.h"

#include <QCheckBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QWidget>

class MainWindow;
class RavenLHSView : public QWidget {
    Q_OBJECT
  public:
    explicit RavenLHSView(QWidget* parent = nullptr);
    ~RavenLHSView() override;

    void updateCommitMessageUIState(bool state) const;

    RavenTree* getRavenTree() { return m_treeView; }

    void commit();
    void resetCommitMessageUI() const;
  signals:
    void signalMaxStatusFileCountReached(bool reached);
    void signalStagingItemAbsPathsReady(QList<QString> paths);

  public slots:
    void updateCommitMessageUI();
    void slotMaxStatusFileCountReached(bool reached);

  private:
    RavenTree* m_treeView;
    QPlainTextEdit* m_commitMessageTextBox;
    QCheckBox* m_amendCommitCheckbox;
    QPushButton* m_commitMessageButton;

    // Used by `commit()` function
    void getAllStagingItemAbsPathsAsync();

    // Max file count reached UI stuff
    QLabel* m_maxStatusFileCountWarningLabel = nullptr;
    bool isMaxStatusFileCountWarningVisible = false;
    void buildMaxStatusFileCountWarningUI() const;
};

#endif // RAVENLHSVIEW_H
