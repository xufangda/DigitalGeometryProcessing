#pragma once
#include <QMainWindow>
#include <QtGui>
#include <QtWidgets>

class MainViewerWidget;

class SurfaceMeshProcessing : public QMainWindow
{
	Q_OBJECT
public:
	SurfaceMeshProcessing(QWidget *parent = 0);
	~SurfaceMeshProcessing(void);

private:
	void CreateActions(void);
	void CreateMenus(void);
	void CreateToolBars(void);
	void CreateStatusBar(void);

	private slots:
	void About(void);

private:
	// File Actions.
	QAction *actOpen;
	QAction *actSave;
	QAction *actClearMesh;
	QAction *actScreenshot;
	QAction *actExit;

	// View Actions.
	QAction *actPoints;
	QAction *actWireframe;
	QAction *actHiddenLines;
	QAction *actFlatLines;
	QAction *actFlat;
	QAction *actSmooth;
	QAction *actLighting;
	QAction *actDoubleSide;
	QAction *actBoundingBox;
	QAction *actBoundary;
	QAction *actResetView;
	QAction *actViewCenter;
	QAction *actCopyRotation;
	QAction *actLoadRotation;

	// Help Actions.
	QAction *actAbout;

	MainViewerWidget* viewer;
};
