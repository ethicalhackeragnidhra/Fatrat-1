/*
FatRat download manager
http://fatrat.dolezel.info

Copyright (C) 2006-2010 Lubos Dolezel <lubos a dolezel.info>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

In addition, as a special exemption, Luboš Doležel gives permission
to link the code of FatRat with the OpenSSL project's
"OpenSSL" library (or with modified versions of it that use the; same
license as the "OpenSSL" library), and distribute the linked
executables. You must obey the GNU General Public License in all
respects for all of the code used other than "OpenSSL".
*/

#include "HttpDetails.h"
#include "CurlDownload.h"
#include "Settings.h"
#include "fatrat.h"
#include <QPainter>
#include <QLinearGradient>

HttpDetails::HttpDetails(QWidget* w) : m_download(0)
{
	setupUi(w);

	connect(pushSegmentDelete, SIGNAL(clicked()), this, SLOT(deleteSegment()));
	connect(pushUrlAdd, SIGNAL(clicked()), this, SLOT(addUrl()));
	connect(pushUrlEdit, SIGNAL(clicked()), this, SLOT(editUrl()));
	connect(pushUrlDelete, SIGNAL(clicked()), this, SLOT(deleteUrl()));
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(refresh()));

	m_menu.addAction(tr("Add new URL..."), this, SLOT(addSegmentUrl()));
	m_separator = m_menu.addSeparator();
	pushSegmentAdd->setMenu(&m_menu);

	m_timer.start(getSettingsValue("gui_refresh").toInt());
	treeActiveSegments->setColumnWidth(0, 500);
}
void HttpDetails::setDownload(CurlDownload* d)
{
	m_download = d;
	widgetSegments->setDownload(d);
	refresh();
}

void HttpDetails::addSegment()
{
	QAction* act = static_cast<QAction*>(sender());
	int urlIndex = act->data().toInt();
	m_download->m_listActiveSegments << urlIndex;
	if (m_download->isActive() && m_download->total())
		m_download->startSegment(urlIndex);
	refresh();
}

void HttpDetails::deleteSegment()
{
	QTreeWidgetItem* item = treeActiveSegments->currentItem();
	if (!item)
		return;
	int urlIndex = item->data(0, Qt::UserRole).toInt();
	if (m_download->isActive())
	{
		int segIndex = item->data(1, Qt::UserRole).toInt();
		m_download->stopSegment(segIndex);
	}
	m_download->m_listActiveSegments.removeOne(urlIndex);
	delete item;
}

void HttpDetails::addUrl()
{

}

void HttpDetails::editUrl()
{

}

void HttpDetails::deleteUrl()
{

}

void HttpDetails::addSegmentUrl()
{

}


void HttpDetails::refresh()
{
	QReadLocker l(&m_download->m_segmentsLock);
	if (m_download->isActive())
	{
		for (int i=0,j=0;i<m_download->m_segments.size();i++)
		{
			const CurlDownload::Segment& s = m_download->m_segments[i];
			if (!s.client || s.urlIndex < 0)
				continue;
			QTreeWidgetItem* item;
			QString url = m_download->m_urls[s.urlIndex].url.toString();
			GradientWidget* gradient;

			if (j < treeActiveSegments->topLevelItemCount())
				item = treeActiveSegments->topLevelItem(j);
			else
			{
				item = new QTreeWidgetItem(treeActiveSegments);
				treeActiveSegments->addTopLevelItem(item);
			}
			item->setData(0, Qt::UserRole, s.urlIndex);
			item->setData(1, Qt::UserRole, i);
			item->setText(0, url);
			gradient = static_cast<GradientWidget*>(treeActiveSegments->itemWidget(item, 1));

			if (!gradient)
			{
				gradient = new GradientWidget(s.color);
				treeActiveSegments->setItemWidget(item, 1, gradient);
			}
			else if (gradient->color() != s.color)
			{
				gradient->setColor(s.color);
			}

			QString size;
			qlonglong from, to;
			int down, up;

			from = s.client->rangeFrom();
			to = s.client->rangeTo();

			if (to == -1)
				size = "∞";
			else
				size = formatSize(to-from);
			item->setText(2, size);

			s.client->speeds(down, up);
			if (s.client->progress())
				item->setText(3, QString("%1%").arg(int(100 / (double(to-from) / s.client->progress()) )));
			else
				item->setText(3, QString());
			item->setText(4, formatSize(down)+"/s");
			j++;
		}

		int count;
		while ( (count = treeActiveSegments->topLevelItemCount()) > m_download->m_listActiveSegments.size())
			delete treeActiveSegments->takeTopLevelItem(count-1);
	}
	else
	{
		for (int i=0;i<m_download->m_listActiveSegments.size();i++)
		{
			int urlIndex = m_download->m_listActiveSegments[i];
			QTreeWidgetItem* item;
			QString url = m_download->m_urls[urlIndex].url.toString();
			GradientWidget* gradient;

			if (i < treeActiveSegments->topLevelItemCount())
				item = treeActiveSegments->topLevelItem(i);
			else
			{
				item = new QTreeWidgetItem(treeActiveSegments);
				treeActiveSegments->addTopLevelItem(item);
			}
			item->setData(0, Qt::UserRole, urlIndex);
			item->setText(0, url);
			treeActiveSegments->removeItemWidget(item, 1);
			item->setText(2, QString());
			item->setText(3, QString());
			item->setText(4, QString());
		}

		int count;
		while ( (count = treeActiveSegments->topLevelItemCount()) > m_download->m_listActiveSegments.size())
			delete treeActiveSegments->takeTopLevelItem(count-1);
	}

	for (int i=0;i<m_download->m_urls.size();i++)
	{
		QString url = m_download->m_urls[i].url.toString();
		QListWidgetItem* item;

		if (i < listUrls->count())
			item = listUrls->item(i);
		else
		{
			item = new QListWidgetItem(listUrls);
			listUrls->addItem(item);
		}
		item->setText(url);
	}
	int count;
	while ( (count = listUrls->count()) > m_download->m_urls.size())
		delete listUrls->takeItem(count-1);

	for (int i=0;i<m_download->m_urls.size();i++)
	{
		QString url = m_download->m_urls[i].url.toString();
		QAction* a;
		if(url.size() > 50)
		{
			url.resize(47);
			url += "...";
		}

		if (m_menuActions.size() <= i)
		{
			a = m_menu.addAction(url, this, SLOT(addSegment()));
			m_menuActions << a;
		}
		else
		{
			a = m_menuActions[i];
			a->setText(url);
		}
		a->setData(i);
	}
	for(int i=m_download->m_urls.size();i<m_menuActions.size();i++)
	{
		m_menu.removeAction(m_menuActions[i]);
		m_menuActions.removeAt(i--);
	}
}

HttpDetails::GradientWidget::GradientWidget(QColor color) : m_color(color)
{
}

void HttpDetails::GradientWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QLinearGradient gradient(0, 0, 0, height());

	gradient.setColorAt(0, m_color.lighter(200));
	gradient.setColorAt(0.5, m_color);
	gradient.setColorAt(1, m_color);

	painter.fillRect(rect(), QBrush(gradient));
}

