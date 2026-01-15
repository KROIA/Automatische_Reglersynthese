#include "RibbonImpl.h"


RibbonImpl::RibbonImpl(QToolBar* parent)
	: RibbonWidget::Ribbon(parent)
{
	Q_INIT_RESOURCE(RibbonIcons); // Init the resource file
	Q_INIT_RESOURCE(AutoTunerIcons); // Init the resource file

	// Create tabs
	RibbonWidget::RibbonTab* scene = new RibbonWidget::RibbonTab("Szene", ":/icons/developerTool.png", this);
	//RibbonWidget::RibbonTab* tabEdit = new RibbonWidget::RibbonTab("Editing", ":/icons/create_new_2.png", this);

	// Create groups
	RibbonWidget::RibbonButtonGroup* general = new RibbonWidget::RibbonButtonGroup("Allgemein", scene);


	//RibbonWidget::RibbonButtonGroup* groupSettings = new RibbonWidget::RibbonButtonGroup("groupSettings", tabSettings);
	//RibbonWidget::RibbonButtonGroup* groupWork = new RibbonWidget::RibbonButtonGroup("groupWork", tabEdit);
	//RibbonWidget::RibbonButtonGroup* groupEdit = new RibbonWidget::RibbonButtonGroup("groupEdit", tabEdit);

	// Create buttons
	m_generalButtons.resetScene = new RibbonWidget::RibbonButton("Neustart", "Startet die aktuelle Szene neu", ":/icons/reset.png", true, general);
	m_generalButtons.setupDCMotorProblem = new RibbonWidget::RibbonButton("DC Motor", QString::fromUtf16(u"Lädt das DC Motor Szenario"), ":/icons/motor.png", true, general);
	m_generalButtons.setupDCMotorWithMassProblem = new RibbonWidget::RibbonButton(QString::fromUtf16(u"Motor + Masse"), QString::fromUtf16(u"Lädt das Motor mit Schwungmasse Szenario"), ":/icons/motorWithMass.png", true, general);
	
	//m_settingsButtons.settings1 = new RibbonWidget::RibbonButton("Settings 1", "Settings 1", ":/icons/engineering_1.png", true, groupSettings);
	//m_settingsButtons.settingsOK = new RibbonWidget::RibbonButton("Settings OK", "Settings OK", ":/icons/accept.png", true, groupSettings);
	//m_settingsButtons.settingsCancel = new RibbonWidget::RibbonButton("Cancel", "Cancel", ":/icons/close.png", true, groupSettings);
	//m_settingsButtons.settingsSave = new RibbonWidget::RibbonButton("Settings Save", "Settings Save", ":/icons/floppy_disk.png", true, groupSettings);
	//
	//m_settingsButtons.settings1->setIconSize(QSize(40, 40));
	//
	//m_workButtons.open = new RibbonWidget::InformativeToolButton("Open", "Open", ":/icons/document.png", true, groupWork);
	//m_workButtons.save = new RibbonWidget::InformativeToolButton("Save", "Save", ":/icons/floppy_disk.png", true, groupWork);
	//
	//m_editButtons.undo = new RibbonWidget::RibbonButton("Undo", "Undo", ":/icons/arrowCClockwise.png", true, groupEdit);
	//m_editButtons.redo = new RibbonWidget::RibbonButton("Redo", "Redo", ":/icons/arrowClockwise.png", true, groupEdit);
	//
	//m_viewButtons.view1 = new RibbonWidget::InformativeToolButton("View 1", "View 1", ":/icons/file_2.png", true, groupEdit);
	//m_viewButtons.view1->setLoadingCircleColor(QColor(255, 0, 0));
	//m_viewButtons.view1->enableLoadingCircle(true);
	//
	//m_viewButtons.view2 = new RibbonWidget::InformativeToolButton("View 1", "View 1", ":/icons/file_2.png", true, groupEdit);
	//m_viewButtons.view2->setProgressColor(QColor(0, 255, 255));
	//m_viewButtons.view2->setProgress(50);
	//m_viewButtons.view3 = new RibbonWidget::InformativeToolButton("View 1", "View 1", ":/icons/file_2.png", true, groupEdit);

	// Add tabs
	addTab(scene);
	//addTab(tabEdit);
}


RibbonImpl::GeneralButtons& RibbonImpl::generalButtons()
{
	return m_generalButtons;
}
/*RibbonImpl::SettingsButtons& RibbonImpl::settingsButtons()
{
	return m_settingsButtons;
}
RibbonImpl::WorkButtons& RibbonImpl::workButtons()
{
	return m_workButtons;
}
RibbonImpl::EditButtons& RibbonImpl::editButtons()
{
	return m_editButtons;
}
RibbonImpl::ViewButtons& RibbonImpl::viewButtons()
{
	return m_viewButtons;
}*/