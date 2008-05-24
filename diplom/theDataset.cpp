#include "theDataset.h"


TheDataset::TheDataset()
{
	m_headLoaded = false;
	m_overlayLoaded = false;
	m_rgbLoaded = false;
	
	m_headInfo = new DatasetInfo();
	m_overlayInfo = new DatasetInfo();
	m_rgbInfo = new DatasetInfo();
}

bool TheDataset::loadHead(wxString filename)
{
	bool flag =m_headInfo->load(filename);
	
	if (flag)
	{
		flag = false;
		wxFile dataFile;
		if (dataFile.Open(filename.BeforeLast('.')+ wxT(".ima")))
		{
			wxFileOffset nSize = dataFile.Length();
			if (nSize == wxInvalidOffset) return false;
			m_dataHead = new float[2*nSize];

			//wxMessageBox(wxString::Format(wxT("%d"), sizeof(buffer)),  wxT(""), wxOK|wxICON_INFORMATION, NULL);
			switch (m_headInfo->getMode())
			{
			case 1: {
				wxUint8 *buffer = new wxUint8[nSize];
				if (dataFile.Read(buffer, (size_t) nSize) != nSize)
				{
					dataFile.Close();
					delete[] buffer;
					return false;
				}
				else flag = true;
				for (int i = 0 ; i < nSize ; ++i)
				{
					m_dataHead[2*i] = (float)buffer[i]/255.0;
					m_dataHead[2*i + 1] = m_dataHead[2*i];
				}
				m_headInfo->setHighestValue(255.0); 
			} break;
			case 2: {
				wxUint16 *buffer = new wxUint16[nSize];
				if (dataFile.Read(buffer, (size_t) nSize) != nSize)
				{
					dataFile.Close();
					delete[] buffer;
					return false;
				}
				else flag = true;
				for (int i = 0 ; i < nSize ; ++i)
				{
					m_dataHead[2*i] = (float)buffer[i]/65536.0;
					m_dataHead[2*i + 1] = m_dataHead[2*i];
				}
				m_headInfo->setHighestValue(65536.0); 
			} break;
			case 3: {
				if (dataFile.Read(m_dataHead, (size_t) nSize) != nSize)
				{
					dataFile.Close();
					delete[] m_dataHead;
					return false;
				}
				else flag = true;
				for (int i = 0 ; i < nSize ; ++i)
				{
					m_dataHead[2*i + 1] = m_dataHead[2*i];
				}
				m_headInfo->setHighestValue(1.0);
			} break;
			}
		}
		dataFile.Close();
	}
	
	m_headLoaded = flag;
	return flag;
}

bool TheDataset::loadOverlay(wxString filename)
{
	bool flag = m_overlayInfo->load(filename);
	
	if (flag)
	{
		flag = false;
		wxFile dataFile;
		if (dataFile.Open(filename.BeforeLast('.')+ wxT(".ima")))
		{
			wxFileOffset nSize = dataFile.Length();
			if (nSize == wxInvalidOffset) return false;
			
			switch (m_overlayInfo->getMode())
			{
			case 1: {
				m_dataOverlay = new float[nSize];
				wxUint8 *buffer = new wxUint8[nSize];
				if (dataFile.Read(buffer, (size_t) nSize) != nSize)
				{
					dataFile.Close();
					delete[] buffer;
					return false;
				}
				else flag = true;
				for (int i = 0 ; i < nSize ; ++i)
				{
					m_dataOverlay[i] = (float)buffer[i];
				}
				m_overlayInfo->setHighestValue(255.0); 
			} break;
			case 2: {
				m_dataOverlay = new float[nSize/2];
				wxUint16 *buffer = new wxUint16[nSize];
				if (dataFile.Read(buffer, (size_t) nSize) != nSize)
				{
					dataFile.Close();
					delete[] buffer;
					return false;
				}
				else flag = true;
				for (int i = 0 ; i < nSize ; ++i)
				{
					m_dataOverlay[i] = (float)buffer[i];
				}
				m_overlayInfo->setHighestValue(65536.0); 
			} break;
			case 3: {
				m_dataOverlay = new float[nSize/4];
				
				wxFileInputStream input(filename.BeforeLast('.') + wxT(".ima"));
				if (!input.Ok())
					return false;
				wxDataInputStream data( input );
				//data.BigEndianOrdered(true);
				
				float max = 0;
				int count = 0;
				
				while (!input.Eof())
				{
					data >> m_dataOverlay[count];
					max = wxMax(max, m_dataOverlay[count]);
					++count;
				}
				printf("\nmax: %f count: %d\n", max, count);

			} break;
			}
		}
		dataFile.Close();
	}
	m_overlayLoaded = flag;
	return flag;
}

bool TheDataset::loadRGB(wxString filename)
{
	bool flag = m_rgbInfo->load(filename);
	
	if (flag)
	{
		flag = false;
		wxFile dataFile;
		if (dataFile.Open(filename.BeforeLast('.')+ wxT(".ima")))
		{
			wxFileOffset nSize = dataFile.Length();
			if (nSize == wxInvalidOffset) return false;
			m_dataRGB = new float[(nSize/3)*4];

			
			switch (m_rgbInfo->getMode())
			{
			case 1: {
				wxUint8 *buffer = new wxUint8[nSize];
				if (dataFile.Read(buffer, (size_t) nSize) != nSize)
				{
					dataFile.Close();
					delete[] buffer;
					return false;
				}
				else flag = true;

				int offset = m_rgbInfo->getColumns() * m_rgbInfo->getRows() ;
				int startslize = 0;
				
				for (int i = 0 ; i < m_rgbInfo->getFrames() ; ++i)
				{
					startslize = i * offset * 3;
					for (int j = 0 ; j < offset ; ++j)
					{
						m_dataRGB[startslize + 3*j] = (float)buffer[startslize + j]/255.0;
						m_dataRGB[startslize + 3*j + 1] = (float)buffer[startslize + offset + j]/255.0;
						m_dataRGB[startslize + 3*j + 2] = (float)buffer[startslize + 2*offset + j]/255.0;
					}
				}
				
				m_rgbInfo->setHighestValue(255.0); 
			} break;
			
			case 2: {
				wxUint16 *buffer = new wxUint16[nSize];
				if (dataFile.Read(buffer, (size_t) nSize) != nSize)
				{
					dataFile.Close();
					delete[] buffer;
					return false;
				}
				else flag = true;
				for (int i = 0 ; i < nSize ; ++i)
				{
					m_dataRGB[i] = (float)buffer[i]/65536.0;
				}
				m_rgbInfo->setHighestValue(65536.0); 
			} break;
			
			case 3: {
				if (dataFile.Read(m_dataRGB, (size_t) nSize) != nSize)
				{
					dataFile.Close();
					delete[] m_dataRGB;
					return false;
				}
				else flag = true;
				m_rgbInfo->setHighestValue(1.0);
			} break;
			
			}
		}
		dataFile.Close();
	}
	m_rgbLoaded = flag;
	return flag;
}
