#include <iostream>
#include <sstream>

#include <QApplication>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QScreen>
#include "QGLViewerWidget.h"
#include <QOpenGLTexture>

const double QGLViewerWidget::trackballradius = 0.6;

QGLViewerWidget::QGLViewerWidget(QWidget* _parent)
	: QOpenGLWidget(_parent),
	drawmode(FLATLINES),
	projectionmode(PERSPECTIVE),
	windowleft(-1),
	windowright(1),
	windowtop(1),
	windowbottom(-1),
	mousemode(Qt::NoButton),
	center(0),
	radius(0),
	projectionmatrix(16, 0.0),
	modelviewmatrix(16, 0.0),
	copymodelviewmatrix(16, 0.0),
	lastpoint2(0, 0),
	lastpoint3(0),
	lastpointok(false)
{
	Init();
}

QGLViewerWidget::~QGLViewerWidget(void)
{
}

void QGLViewerWidget::Init(void)
{
	// qt stuff
	setAttribute(Qt::WA_NoSystemBackground, true);
	setFocusPolicy(Qt::StrongFocus);
	//setAcceptDrops( true );  
	//setCursor(PointingHandCursor);

	SetProjectionMode(projectionmode);
}

QSize QGLViewerWidget::minimumSizeHint(void) const
{
	return QSize(10, 10);
}

QSize QGLViewerWidget::sizeHint(void) const
{
	//QRect rect = QApplication::desktop()->screenGeometry();
	QRect rect = QGuiApplication::screens()[0]->geometry();
	
	return QSize(int(rect.width()*0.8), int(rect.height()*1.0));
}

const double& QGLViewerWidget::Radius(void) const
{
	return radius; 
}

const OpenMesh::Vec3d& QGLViewerWidget::Center(void) const
{
	return center;
}

const double* QGLViewerWidget::GetModelviewMatrix(void) const
{
	return &modelviewmatrix[0];
}

void QGLViewerWidget::ResetModelviewMatrix(void)
{
	makeCurrent();
	glLoadIdentity();
	glGetDoublev(GL_MODELVIEW_MATRIX, &modelviewmatrix[0]);
}

void QGLViewerWidget::CopyModelViewMatrix(void)
{
	copymodelviewmatrix = modelviewmatrix;
}

void QGLViewerWidget::LoadCopyModelViewMatrix(void)
{
	modelviewmatrix = copymodelviewmatrix;
}

const double* QGLViewerWidget::GetProjectionMatrix(void) const
{
	return &projectionmatrix[0];
}

void QGLViewerWidget::SetDrawMode(const DrawMode &dm)
{
	drawmode = dm;
	update();
}

const QGLViewerWidget::DrawMode& QGLViewerWidget::GetDrawMode(void) const
{
	return drawmode;
}

void QGLViewerWidget::SetProjectionMode(const ProjectionMode &pm)
{
	projectionmode = pm;
	UpdateProjectionMatrix();
	ViewAll();
}

const QGLViewerWidget::ProjectionMode& QGLViewerWidget::GetProjectionMode(void) const
{
	return projectionmode;
}

void QGLViewerWidget::SetMaterial(const MaterialType & mattype) const
{
	std::vector<GLfloat> matAmbient, matDiffuse, matSpecular;
	GLfloat matShininess;
	switch (mattype)
	{
	default:
	case MaterialDefault:
		matAmbient = { 1.0f, 1.0f, 1.0f, 1.0f };
		matDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		matSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
		matShininess = 120.0f;
		break;
	case MaterialGold:
		matAmbient = { 0.9f, 0.667f, 0.0f, 1.0f };
		matDiffuse = { 0.9f, 0.749f, 0.251f, 1.0f };
		matSpecular = { 0.9f, 0.816f, 0.451f, 1.0f };
		matShininess = 50.0f;
		break;
	case MaterialSilver:
		matAmbient = { 0.3f, 0.3f, 0.3f, 1.0f };
		matDiffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
		matSpecular = { 0.7f, 0.7f, 0.7f, 1.0f };
		matShininess = 51.2f;
		break;
	case MaterialEmerald:
		matAmbient = { 0.143f, 0.549f, 0.143f, 1.0f };
		matDiffuse = { 0.143f, 0.549f, 0.143f, 1.0f };
		matSpecular = { 0.733f, 0.927811f, 0.733f, 1.0f };
		matShininess = 76.8f;
		break;
	case MaterialTin:
		matAmbient = { 0.405882f, 0.358824f, 0.413725f, 1.0f };
		matDiffuse = { 0.727451f, 0.770588f, 0.841176f, 1.0f };
		matSpecular = { 0.633333f, 0.633333f, 0.821569f, 1.0f };
		matShininess = 59.84615f;
		break;
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient.data());
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse.data());
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular.data());
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
}

void QGLViewerWidget::SetDefaultLight(void) const
{
	// lighting
	GLfloat pos1[] = { 10.0f, 10.0f, -10.0f, 0.0f };
	GLfloat pos2[] = { -10.0f, 10.0f, -10.0f, 0.0f };
	GLfloat pos3[] = { 0.0f, 0.0f, 10.0f, 0.0f };
	GLfloat col1[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat col2[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat col3[] = { 0.8f, 0.8f, 0.8f, 1.0f };

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, col1);
	glLightfv(GL_LIGHT0, GL_SPECULAR, col1);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_POSITION, pos2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, col2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, col2);

	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_POSITION, pos3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, col3);
	glLightfv(GL_LIGHT2, GL_SPECULAR, col3);
}

void QGLViewerWidget::initializeGL(void)
{
	// OpenGL state
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0);
	//glDisable(GL_DITHER);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	// Material
	SetMaterial();
	// Lighting
	glLoadIdentity();
	SetDefaultLight();

	// scene pos and size
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix_);

	//for initialize all the viewports
	glGetDoublev(GL_MODELVIEW_MATRIX, &modelviewmatrix[0]);
	CopyModelViewMatrix();

	SetScenePosition(OpenMesh::Vec3d(0.0, 0.0, 0.0), 1.0);
	//LoadTexture();
}

void QGLViewerWidget::resizeGL(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	UpdateProjectionMatrix();
	update();
}

void QGLViewerWidget::paintGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawScene();
}

void QGLViewerWidget::DrawScene(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(&projectionmatrix[0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&modelviewmatrix[0]);
	switch (drawmode)
	{
	case WIREFRAME:
		glDisable(GL_LIGHTING);
		//glutWireTeapot(0.5);
		break;
	case FLAT:
		glEnable(GL_LIGHTING);
		glShadeModel(GL_FLAT);
		//glutSolidTeapot(0.5);
		break;
	case FLATLINES:
		glEnable(GL_LIGHTING);
		glShadeModel(GL_FLAT);
		//glutSolidTeapot(0.5);
		break;
	case HIDDENLINES:
		glDisable(GL_LIGHTING);
		break;
	case SMOOTH:
		glEnable(GL_LIGHTING);
		glShadeModel(GL_SMOOTH);
		//glutSolidTeapot(0.5);
		break;
	default:
		break;
	}
}

void QGLViewerWidget::mousePressEvent(QMouseEvent* _event)
{
	//assert(mousemode < N_MOUSE_MODES);
	lastpoint2 = _event->pos();
	lastpointok = MapToSphere(lastpoint2, lastpoint3);
	mousemode = _event->button();
}

void QGLViewerWidget::mouseMoveEvent(QMouseEvent* _event)
{
	QPoint newPoint2D = _event->pos();
	if (lastpointok)
	{
		switch (mousemode)
		{
		case Qt::LeftButton:
			Rotation(newPoint2D);
			break;
		case Qt::RightButton:
			Translation(newPoint2D);
			break;
		default:
			break;
		}
	} // end of if

	// remember this point
	lastpoint2 = newPoint2D;
	lastpointok = MapToSphere(lastpoint2, lastpoint3);
	// trigger redraw
	update();
}

void QGLViewerWidget::mouseReleaseEvent(QMouseEvent*  _event)
{
	//assert(mousemode < N_MOUSE_MODES);
	mousemode = Qt::NoButton;
	lastpointok = false;
}

void QGLViewerWidget::wheelEvent(QWheelEvent* _event)
{
	// Use the mouse wheel to zoom in/out
	//double d = -_event->delta() / 120.0 * 0.05 * radius;

	double d = - _event->angleDelta().x() / 120.0 * 0.05 * radius;
	Translate(OpenMesh::Vec3d(0.0, 0.0, d));
	update();
}

void QGLViewerWidget::keyPressEvent(QKeyEvent* _event)
{
	switch (_event->key())
	{
	case Qt::Key_Escape:
		qApp->quit();
		break;
	}
	_event->ignore();
}

void QGLViewerWidget::keyReleaseEvent(QKeyEvent* _event)
{
	_event->ignore();
}

void QGLViewerWidget::Translation(const QPoint & p)
{
	double z = -(modelviewmatrix[2] * center[0] +
		modelviewmatrix[6] * center[1] +
		modelviewmatrix[10] * center[2] +
		modelviewmatrix[14]) /
		(modelviewmatrix[3] * center[0] +
			modelviewmatrix[7] * center[1] +
			modelviewmatrix[11] * center[2] +
			modelviewmatrix[15]);

	double w = width(); double h = height();
	double aspect = w / h;
	double near_plane = 0.01 * radius;
	double top = tan(45.0 / 2.0f*M_PI / 180.0f) * near_plane;
	double right = aspect*top;

	double dx = p.x() - lastpoint2.x();
	double dy = p.y() - lastpoint2.y();

	Translate(OpenMesh::Vec3d(2.0*dx / w*right / near_plane*z,
		-2.0*dy / h*top / near_plane*z,
		0.0f));
}

void QGLViewerWidget::Translate(const OpenMesh::Vec3d & _trans)
{
	// Translate the object by _trans
	// Update modelview_matrix_
	makeCurrent();
	glLoadIdentity();
	glTranslated(_trans[0], _trans[1], _trans[2]);
	glMultMatrixd(&modelviewmatrix[0]);
	glGetDoublev(GL_MODELVIEW_MATRIX, &modelviewmatrix[0]);
}

void QGLViewerWidget::Rotation(const QPoint & p)
{
	OpenMesh::Vec3d  newPoint3D;
	bool newPoint_hitSphere = MapToSphere(p, newPoint3D);
	if (newPoint_hitSphere)
	{
		OpenMesh::Vec3d axis = lastpoint3 % newPoint3D;
		if (axis.sqrnorm() < 1e-7)
		{
			axis = OpenMesh::Vec3d(1, 0, 0);
		}
		else
		{
			axis.normalize();
		}
		// find the amount of rotation
		OpenMesh::Vec3d d = lastpoint3 - newPoint3D;
		double t = 0.5*d.norm() / trackballradius;
		if (t < -1.0) t = -1.0;
		else if (t > 1.0) t = 1.0;
		double phi = 2.0 * asin(t);
		double  angle = phi * 180.0 / M_PI;
		Rotate(axis, angle);
	}
}

void QGLViewerWidget::Rotate(const OpenMesh::Vec3d& _axis, const double & _angle)
{
	// Rotate around center center_, axis _axis, by angle _angle
	// Update modelview_matrix_

	OpenMesh::Vec3d t(modelviewmatrix[0] * center[0] +
		modelviewmatrix[4] * center[1] +
		modelviewmatrix[8] * center[2] +
		modelviewmatrix[12],
		modelviewmatrix[1] * center[0] +
		modelviewmatrix[5] * center[1] +
		modelviewmatrix[9] * center[2] +
		modelviewmatrix[13],
		modelviewmatrix[2] * center[0] +
		modelviewmatrix[6] * center[1] +
		modelviewmatrix[10] * center[2] +
		modelviewmatrix[14]);

	makeCurrent();
	glLoadIdentity();
	glTranslatef(t[0], t[1], t[2]);
	glRotated(_angle, _axis[0], _axis[1], _axis[2]);
	glTranslatef(-t[0], -t[1], -t[2]);
	glMultMatrixd(&modelviewmatrix[0]);
	glGetDoublev(GL_MODELVIEW_MATRIX, &modelviewmatrix[0]);
}

bool QGLViewerWidget::MapToSphere(const QPoint& _v2D, OpenMesh::Vec3d& _v3D)
{
	// This is actually doing the Sphere/Hyperbolic sheet hybrid thing,
	// based on Ken Shoemake's ArcBall in Graphics Gems IV, 1993.
	double x = (2.0*_v2D.x() - width()) / width();
	double y = -(2.0*_v2D.y() - height()) / height();
	double xval = x;
	double yval = y;
	double x2y2 = xval*xval + yval*yval;

	const double rsqr = trackballradius * trackballradius;
	_v3D[0] = xval;
	_v3D[1] = yval;
	if (x2y2 < 0.5*rsqr)
	{
		_v3D[2] = sqrt(rsqr - x2y2);
	}
	else
	{
		_v3D[2] = 0.5*rsqr / sqrt(x2y2);
	}

	return true;
}

void QGLViewerWidget::UpdateProjectionMatrix(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (PERSPECTIVE == projectionmode)
	{
		glFrustum(-0.01 * radius * (sqrt(2.0) - 1) * width() / height(),
			0.01 * radius * (sqrt(2.0) - 1) * width() / height(),
			-0.01 * radius * (sqrt(2.0) - 1),
			0.01 * radius * (sqrt(2.0) - 1),
			0.01 * radius,
			100.0 * radius);
	}
	else if (ORTHOGRAPHIC == projectionmode) //not work for 
	{
		glOrtho(windowleft, windowright, windowbottom, windowtop, -1, 1);
	}
	glGetDoublev(GL_PROJECTION_MATRIX, &projectionmatrix[0]);
	glMatrixMode(GL_MODELVIEW);
}

void QGLViewerWidget::SetScenePosition(const OpenMesh::Vec3d& _center, const double & _radius)
{
	center = _center;
	radius = _radius;

	UpdateProjectionMatrix();
	ViewAll();
}

void QGLViewerWidget::ViewAll(void)
{
	OpenMesh::Vec3d _trans = OpenMesh::Vec3d(-(modelviewmatrix[0] * center[0] +
		modelviewmatrix[4] * center[1] +
		modelviewmatrix[8] * center[2] +
		modelviewmatrix[12]),
		-(modelviewmatrix[1] * center[0] +
		modelviewmatrix[5] * center[1] +
		modelviewmatrix[9] * center[2] +
		modelviewmatrix[13]),
		-(modelviewmatrix[2] * center[0] +
		modelviewmatrix[6] * center[1] +
		modelviewmatrix[10] * center[2] +
		modelviewmatrix[14] +
		2.0*radius));

	makeCurrent();
	glLoadIdentity();
	glTranslated(_trans[0], _trans[1], _trans[2]);
	glMultMatrixd(&modelviewmatrix[0]);
	glGetDoublev(GL_MODELVIEW_MATRIX, &modelviewmatrix[0]);
}
