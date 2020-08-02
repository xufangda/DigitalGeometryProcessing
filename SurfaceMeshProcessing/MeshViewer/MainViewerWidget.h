#pragma once
#include <QtGui>
#include <QString>
#include <QFileDialog>
class MeshParamWidget;
class InteractiveViewerWidget;
class MainViewerWidget : public QDialog
{
	Q_OBJECT
public:
	MainViewerWidget(QWidget* _parent = 0);
	~MainViewerWidget(void);

protected:
	virtual void InitViewerWindow(void);
	virtual void CreateParamWidget(void);
	virtual void CreateViewerDialog(void);
	virtual void OpenMeshGUI(const QString & fname);
	virtual void SaveMeshGUI(const QString & fname);

	private slots:
	void LoadMeshFromInner(bool OK, QString fname);
	public slots:
	void Open(void);
	void Save(void);
	void ClearMesh(void);
	void Screenshot(void);

	void ShowPoints(void);
	void ShowWireframe(void);
	void ShowHiddenLines(void);
	void ShowFlatLines(void);
	void ShowFlat(void);
	void ShowSmooth(void);
	void Lighting(bool b);
	void DoubleSideLighting(bool b);
	void ShowBoundingBox(bool b);
	void ShowBoundary(bool b);
	void ResetView(void);
	void ViewCenter(void);
	void CopyRotation(void);
	void LoadRotation(void);

signals:
	void haveLoadMesh(QString filePath);

protected:
	bool loadmeshsuccess;

private:
	MeshParamWidget* meshparamwidget;
	InteractiveViewerWidget* meshviewerwidget;
};
