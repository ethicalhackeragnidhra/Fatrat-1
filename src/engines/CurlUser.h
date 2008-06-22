/*
FatRat download manager
http://fatrat.dolezel.info

Copyright (C) 2006-2008 Lubos Dolezel <lubos a dolezel.info>

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
*/


#ifndef CURLUSER_H
#define CURLUSER_H
#include <curl/curl.h>
#include <sys/time.h>
#include <QList>
#include <QPair>

class CurlUser
{
public:
	CurlUser();
protected:
	virtual CURL* curlHandle() = 0;
	virtual void speedLimits(int& down, int& up) = 0;
	virtual void transferDone(CURLcode result) = 0;
	
	virtual size_t readData(char* buffer, size_t maxData);
	virtual bool writeData(const char* buffer, size_t bytes);
	
	void resetStatistics();
	void speed(int& down, int& up);
	
	static size_t read_function(char *ptr, size_t size, size_t nmemb, CurlUser* This);
	static size_t write_function(const char* ptr, size_t size, size_t nmemb, CurlUser* This);
private:
	static int computeSpeed(const QList<QPair<long,long> >& data);
	static bool isNull(const timeval& t);
private:
	QList<QPair<long,long> > m_statsDown, m_statsUp;
	timeval m_lastDown, m_lastUp;
	
	QPair<long, long> m_accumDown, m_accumUp;
	
	friend class CurlPoller;
};

#endif
