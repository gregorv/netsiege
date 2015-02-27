/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2015 Gregor Vollmer <gregor@celement.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QMainWindow>
#include <QUndoStack>
#include <QDoubleSpinBox>
#include <QStringListModel>
#include <QMessageBox>
#include <QProgressBar>
#include "ui_main.h"
#include "ui_sloteditor.h"
#include "ui_layereditor.h"
#include "terrainlayerlistmodel.h"
#include "brush.h"
#include "campaignmanager.h"
#include "ogrebase.h"
#include "editorcamera.h"
#include "sloteditor.h"

class LayerEditor : public QDialog {
public:
    LayerEditor(QWidget* parent)
    : QDialog(parent)
    {
        ui.setupUi(this);
        auto layers = new TerrainLayerListModel;
        QStringList list;
        list << "Hallo" << "Welt!" << ":D";
        auto model = new QStringListModel(list);
        ui.layerList->setModel(layers);
        connect(ui.layerList, &QListView::activated, this, &LayerEditor::selectLayer);
        connect(ui.newLayerButton, &QPushButton::clicked, this, &LayerEditor::newLayer);
    }

    void newLayer()
    {
        if(!mayPerform()) {
            return;
        }
        auto model = dynamic_cast<TerrainLayerListModel*>(ui.layerList->model());
        model->newLayer();
    }

    void selectLayer(const QModelIndex & indexes)
    {
        if(!mayPerform()) {
//             ui.layerList;
            return;
        }
        auto model = dynamic_cast<TerrainLayerListModel*>(ui.layerList->model());
        std::cout << "Layer: " << model->layer(indexes).name() << std::endl;
        ui.layerName->setText(model->layer(indexes).name().c_str());
    }

    void applyLayer()
    {
    }

private:
    bool mayPerform()
    {
        if(m_currentAltered) {
            QMessageBox msgBox;
            msgBox.setText(QApplication::translate("LayerEditor", "Layer was modified.", 0));
            msgBox.setInformativeText(QApplication::translate("LayerEditor", "Do you want to apply your changes?", 0));
            msgBox.setStandardButtons(QMessageBox::Apply | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Apply);
            int ret = msgBox.exec();
            if(ret == QMessageBox::Apply) {
                applyLayer();
                return true;
            } else if(ret == QMessageBox::Discard) {
                return true;
            } else {
                return false;
            }
        }
        return true;
    }
    Ui_LayerEditor ui;
    size_t m_currentLayerIdx;
    bool m_currentAltered;
};


class EditorWindow : public QMainWindow {
    Q_OBJECT
public:
    EditorWindow(QWidget* parent)
     : QMainWindow(parent),
       groupCursorMode(this),
       groupBrushMode(this),
       layerEditor(this),
       slotEditor(this),
       campaign(nullptr)
    {
        ui.setupUi(this);
        progressMessage = new QLabel(this);
        progressBar = new QProgressBar(this);
        statusBar()->addPermanentWidget(progressBar);
        statusBar()->addPermanentWidget(progressMessage);
        progressBar->setVisible(false);
//         initProgress("", 1);

        auto redoSeparator = ui.menuEdit->actions()[0];
        auto actionUndo = OgreBase::getSingleton().getStack()->createUndoAction(this, QApplication::translate("EditorWindow", "Undo", 0));
        actionUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
        auto actionRedo = OgreBase::getSingleton().getStack()->createRedoAction(this, QApplication::translate("EditorWindow", "Redo", 0));
        actionRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
        ui.menuEdit->insertAction(redoSeparator, actionUndo);
        ui.menuEdit->insertAction(redoSeparator, actionRedo);
        QMainWindow::tabifyDockWidget(ui.propsLibDock, ui.actuatorsLibDock);
//         QMainWindow::tabifyDockWidget(ui.propsLibDock, ui.propertiesDock);

        ui.actionCursorSelect->setChecked(true);
        groupCursorMode.setExclusive(true);
        groupCursorMode.addAction(ui.actionCursorSelect);
        groupCursorMode.addAction(ui.actionTerrainRaiseLower);
        groupCursorMode.addAction(ui.actionTerrainSmooth);
        groupCursorMode.addAction(ui.actionTerrainFlatten);
        groupCursorMode.addAction(ui.actionLayerPaint);

        ui.actionBrushMix->setChecked(true);
        groupBrushMode.setExclusive(true);
        groupBrushMode.addAction(ui.actionBrushAdd);
        groupBrushMode.addAction(ui.actionBrushSubtract);
        groupBrushMode.addAction(ui.actionBrushMix);

        ui.toolButtonBrushMix->setDefaultAction(ui.actionBrushMix);
        ui.toolButtonBrushAdd->setDefaultAction(ui.actionBrushAdd);
        ui.toolButtonBrushSubtract->setDefaultAction(ui.actionBrushSubtract);

        auto camLabel = new QLabel(QApplication::translate("EditorWindow", "Cam Speed", 0), nullptr);
        auto camSpeedSpin = new QDoubleSpinBox(nullptr);
        auto walkerButton = new QPushButton(nullptr);
        walkerButton->setCheckable(true);
        camSpeedSpin->setMaximum(100);
        camSpeedSpin->setMinimum(1.0);
        camSpeedSpin->setValue(10);
        auto camera = OgreBase::getSingleton().getCamera();
        auto brush = OgreBase::getSingleton().getBrush();
//         auto camSpeedSlider = new QSlider(Qt::Horizontal, nullptr);
//         connect(camSpeedSlider, SIGNAL(valueChanged(int)), camSpeedSpin, SLOT(setValue(double)));
        camera->setSpeed(camSpeedSpin->value());
        connect(walkerButton, &QPushButton::toggled, [=](bool enabled){ camera->setWalkerMode(enabled); });
        connect(camSpeedSpin, SIGNAL(valueChanged(double)), camera, SLOT(setSpeed(double)));
        connect(ui.actionLayerPaint, &QAction::triggered, [=](){ brush->setMode(Brush::M_LAYER_PAINT); });
        connect(ui.actionTerrainRaiseLower, &QAction::triggered, [=](){ brush->setMode(Brush::M_HEIGHT_RAISE); });
        connect(ui.actionTerrainSmooth, &QAction::triggered, [=](){ brush->setMode(Brush::M_HEIGHT_SMOOTH); });
        connect(ui.actionTerrainFlatten, &QAction::triggered, [=](){ brush->setMode(Brush::M_HEIGHT_FLATTEN); });
        connect(ui.actionCursorSelect, &QAction::triggered, [=](){ brush->setMode(Brush::M_NONE); });

        connect(ui.actionLayerEditor, &QAction::triggered, &layerEditor, &LayerEditor::open);
        connect(ui.actionSlotEditor, &QAction::triggered, &slotEditor, &SlotEditor::setVisible);
        connect(&slotEditor, &SlotEditor::close, [=](){ ui.actionSlotEditor->setChecked(false); });

        connect(ui.brushSize, SIGNAL(valueChanged(int)), brush, SLOT(setSize(int)));
        connect(ui.brushStrength, SIGNAL(valueChanged(int)), brush, SLOT(setStrength(int)));
        connect(ui.brushHardness, SIGNAL(valueChanged(int)), brush, SLOT(setHardness(int)));
        connect(ui.displaySize, SIGNAL(valueChanged(double)), brush, SLOT(setSize(double)));
        connect(ui.displayStrength, SIGNAL(valueChanged(double)), brush, SLOT(setStrength(double)));
        connect(ui.displayHardness, SIGNAL(valueChanged(double)), brush, SLOT(setHardness(double)));
        connect(brush, SIGNAL(strengthChangedInt(int)), ui.brushStrength, SLOT(setValue(int)));
        connect(brush, SIGNAL(sizeChangedInt(int)), ui.brushSize, SLOT(setValue(int)));
        connect(brush, SIGNAL(hardnessChangedInt(int)), ui.brushHardness, SLOT(setValue(int)));
        connect(brush, &Brush::strengthChanged, [=](float x){ ui.displayStrength->setValue(x); });
        connect(brush, &Brush::sizeChanged, [=](float x){ ui.displaySize->setValue(x); });
        connect(brush, &Brush::hardnessChanged, [=](float x){ ui.displayHardness->setValue(x); });

        connect(ui.actionBrushAdd, &QAction::triggered, [=](){ brush->setBlendLayer(2); });
        connect(ui.actionBrushMix, &QAction::triggered, [=](){ brush->setBlendLayer(1); });
        connect(ui.actionBrushSubtract, &QAction::triggered, [=](){ brush->setBlendLayer(3); });
        ui.layerNum->setMaximum(7);
        connect(ui.layerNum, SIGNAL(valueChanged(int)), brush, SLOT(setBlendLayer(int)));
//         connect(ui.layerNum, &QSpinBox::valueChanged, [=](int i){  });

//         connect(brush, SIGNAL(strenghChanged(float)), ui.brushStrength, SLOT(setValue(int)));
//         connect(ui.openGLWidget->getBrush(), SIGNAL(sizeChanged(float)), ui.brushSize, SLOT(setValue(int)));
//         connect(ui.openGLWidget->getBrush(), SIGNAL(brushHardness(float)), ui.brushHardness, SLOT(setValue(int)));

        ui.cameraToolbar->addWidget(camLabel);
        ui.cameraToolbar->addWidget(camSpeedSpin);
        ui.cameraToolbar->addWidget(walkerButton);

        connect(ui.actionNe, &QAction::triggered, [=](){
            campaign = new CampaignManager("/home/gregor/projekte/netsiege/testmedia/");
            campaign->initialize(513, 200);
            slotEditor.populate();
            connect(campaign, &CampaignManager::initProgress, this, &EditorWindow::initProgress);
            connect(campaign, &CampaignManager::stepProgress, this, &EditorWindow::stepProgress);
            connect(campaign, &CampaignManager::doneProgress, this, &EditorWindow::doneProgress);
            OgreBase::getSingleton().getBrush()->setEnabled(true);
            ui.actionSave->setEnabled(true);
        });
        connect(ui.actionOpen, &QAction::triggered, [=](){
            campaign = new CampaignManager("/home/gregor/projekte/netsiege/testmedia/");
            campaign->load();
            slotEditor.populate();
            connect(campaign, &CampaignManager::initProgress, this, &EditorWindow::initProgress);
            connect(campaign, &CampaignManager::stepProgress, this, &EditorWindow::stepProgress);
            connect(campaign, &CampaignManager::doneProgress, this, &EditorWindow::doneProgress);
            OgreBase::getSingleton().getBrush()->setEnabled(true);
            ui.actionSave->setEnabled(true);
        });
        connect(ui.actionSave, &QAction::triggered, [=](){ if(campaign) campaign->save(); });
//         ui.cameraToolbar->addWidget(camSpeedSlider);

        auto layers = new TerrainLayerListModel;
        QStringList list;
        list << "Hallo" << "Welt!" << ":D";
        auto model = new QStringListModel(list);
        ui.listView->setModel(layers);
//         ui.listView_2->setModel(model);
        OgreBase::getSingleton().getBrush()->setEnabled(false);
        ui.actionSave->setEnabled(false);
    }
    void changeEvent(QEvent *e)
    {
        QWidget::changeEvent(e);
        switch (e->type()) {
        case QEvent::LanguageChange:
            ui.retranslateUi(this);
            break;
        default:
            break;
    }
 }

 void enableEditing(bool enabled)
 {
     OgreBase::getSingleton().getBrush()->setEnabled(enabled);
 }

public slots:
    void initProgress(std::string message, int numSteps, bool disable)
    {
        if(disable)
            enableEditing(false);
        progressBar->setMaximum(numSteps);
        progressBar->setValue(0);
        progressBar->setVisible(true);
        progressMessage->setText(message.c_str());
        statusBar()->updateGeometry();
        statusBar()->repaint();
    }

    void stepProgress()
    {
        progressBar->setValue(progressBar->value()+1);
        statusBar()->repaint();
    }

    void doneProgress()
    {
        enableEditing(true);
        progressMessage->setText(QApplication::translate("EditorWindow", "Done", 0));
        progressBar->setVisible(false);
        progressBar->setValue(progressBar->maximum());
        statusBar()->updateGeometry();
        statusBar()->repaint();
    }

private:
    Ui::EditorWindow ui;
    QActionGroup groupCursorMode;
    QActionGroup groupBrushMode;
    LayerEditor layerEditor;
    SlotEditor slotEditor;
    QLabel* progressMessage;
    QProgressBar* progressBar;
    CampaignManager* campaign;
};

int main(int argc, char** argv)
{
//     Q_INIT_RESOURCE(application);
    QApplication app(argc, argv);
    EditorWindow window(0);
    window.show();
    return app.exec();
}

#include "main.moc"
