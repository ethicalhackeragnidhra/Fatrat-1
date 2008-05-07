#include "SettingsGeneralForm.h"
#include "Settings.h"
#include "MainWindow.h"
#include "fatrat.h"
#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

extern QSettings* g_settings;

SettingsGeneralForm::SettingsGeneralForm(QWidget* me, QObject* parent) : QObject(parent)
{
	setupUi(me);
	
	connect(toolDestination, SIGNAL(pressed()), this, SLOT(browse()));
	
	comboDoubleClick->addItems(QStringList() << tr("switches to transfer details") << tr("switches to the graph")
			<< tr("opens the file") << tr("opens the parent directory") );
	
	comboCloseCurrent->addItems(QStringList() << tr("switch to the next tab") << tr("switch to the previous active tab"));
	
	comboLinkSeparator->addItems(QStringList() << tr("a newline") << tr("whitespace characters"));
}

void SettingsGeneralForm::load()
{
	lineDestination->setText( getSettingsValue("defaultdir").toString() );
	lineFileExec->setText( getSettingsValue("fileexec").toString() );
	
	checkTrayIcon->setChecked( getSettingsValue("trayicon").toBool() );
	checkHideMinimize->setChecked( getSettingsValue("hideminimize").toBool() );
	checkHideClose->setChecked( getSettingsValue("hideclose").toBool() );
	
	spinGraphMinutes->setValue( getSettingsValue("graphminutes").toInt() );
	
	comboDoubleClick->setCurrentIndex( getSettingsValue("transfer_dblclk").toInt() );
	comboCloseCurrent->setCurrentIndex( getSettingsValue("tab_onclose").toInt() );
	comboLinkSeparator->setCurrentIndex( getSettingsValue("link_separator").toInt() );
	
	spinRefreshGUI->setValue( getSettingsValue("gui_refresh").toInt() / 1000);
	checkCSS->setChecked( getSettingsValue("css").toBool() );
}

bool SettingsGeneralForm::accept()
{
	if(lineDestination->text().isEmpty())
		return false;
	else
	{
		QDir dir(lineDestination->text());
		if(!dir.isReadable())
		{
			QMessageBox::critical(0, tr("Error"), tr("The specified directory is inaccessible."));
			return false;
		}
	}
	
	return true;
}
void SettingsGeneralForm::accepted()
{
	g_settings->setValue("defaultdir", lineDestination->text());
	g_settings->setValue("execfile", lineFileExec->text());
	
	g_settings->setValue("trayicon", checkTrayIcon->isChecked());
	g_settings->setValue("hideminimize", checkHideMinimize->isChecked());
	g_settings->setValue("hideclose", checkHideClose->isChecked());
	
	g_settings->setValue("graphminutes", spinGraphMinutes->value());
	g_settings->setValue("transfer_dblclk", comboDoubleClick->currentIndex());
	g_settings->setValue("tab_onclose", comboCloseCurrent->currentIndex());
	g_settings->setValue("link_separator", comboLinkSeparator->currentIndex());
	
	g_settings->setValue("gui_refresh", spinRefreshGUI->value()*1000);
	g_settings->setValue("css", checkCSS->isChecked());
	
	((MainWindow*) getMainWindow())->applySettings();
}

void SettingsGeneralForm::browse()
{
	QString dir = QFileDialog::getExistingDirectory(0, tr("Choose directory"), lineDestination->text());
	if(!dir.isNull())
		lineDestination->setText(dir);
}
