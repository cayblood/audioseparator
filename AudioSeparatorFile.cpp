// AudioSeparatorFile.cpp: implementation of the CAudioSeparatorFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AudioSeparator.h"
#include "AudioSeparatorFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioSeparatorFile::CAudioSeparatorFile()
{
	m_SplitPoints.assign(INITIAL_VECTOR_SIZE);
	m_SplitPoints.reserve(MAXIMUM_VECTOR_SIZE);
	m_nCurrentSample = 0;
	m_nBeginTrim = 0;
	m_nEndTrim = 0;
	m_nSplitCount = 0;
	m_nMaxSplitLength = 0;
	m_nMinSplitLength = 0;
	m_nAvgSplitLength = 0;
}

CAudioSeparatorFile::~CAudioSeparatorFile()
{

}

AUDSEPRET CAudioSeparatorFile::Open(const char *Filename,
                                    unsigned long nMinSilenceLength,     // in milliseconds
                                    unsigned long nMinTrackLength,       // in seconds
                                    double nSilenceThreshold,            // fractional real number (0.45, for example)
                                    unsigned long nAppendSilence,        // number of milliseconds of silence to append
                                    SILENCE_DETECTION_METHOD method,
                                    BOOL bTrimFile,
									const char *szBeginWith,
									CWnd *parentWnd                      // for displaying progress
									)
{
	DDCRET returnval = m_wavFile.OpenForRead(Filename);

	if (returnval != DDC_SUCCESS)
		return AUD_FILE_OPEN_FAILED;
	
	// initialize member variables
	//m_nLengthInSeconds = wavInFile.NumSamples() / wavInFile.SamplingRate();
	m_nChunkSize = (double)nMinSilenceLength * 0.001 * m_wavFile.SamplingRate();
	m_nSamplesForMinTrackLength = nMinTrackLength * (INT64)m_wavFile.SamplingRate();

	if (m_wavFile.NumSamples() < m_nChunkSize ||
		m_wavFile.NumSamples() < m_nSamplesForMinTrackLength)
			return AUD_TOO_SHORT_FOR_SPLIT;

	m_nSilenceThreshold = nSilenceThreshold;
	m_method = method;
	m_bTrimFile = bTrimFile;
	m_parentWnd = parentWnd;
	m_nAppendSilence = nAppendSilence;
	m_strBeginWith = szBeginWith;
	
	return AUD_SUCCESS;
}

// analyze WAV file to determine how many splits will occur
AUDSEPRET CAudioSeparatorFile::AnalyzeAudio()
{
	if (m_parentWnd)
	{
		m_wndProgress.Create(m_parentWnd, "Progress", TRUE);
		m_wndProgress.SetRange(0, m_wavFile.NumSamples());
		m_wndProgress.SetText("Analyzing WAV file...");
		m_wndProgress.GoModal();
	}

	INT64 nNextSplitPoint = 0;

	if (m_bTrimFile)
	{
		m_nBeginTrim = FindNextPoint(SOUND);

		m_wavFile.SeekToSample(m_nBeginTrim);
		m_nCurrentSample = m_nBeginTrim;
	}

	DDCRET tempvar = m_wavFile.SeekToSample(m_nCurrentSample += m_nSamplesForMinTrackLength);
	while (tempvar == DDC_SUCCESS &&
	       (INT64)m_wavFile.NumSamples() - m_nCurrentSample > m_nSamplesForMinTrackLength)
	{
		nNextSplitPoint = FindNextPoint(SILENCE);
		if (!nNextSplitPoint ||
			(INT64)m_wavFile.NumSamples() - nNextSplitPoint < m_nSamplesForMinTrackLength)
			break;
		if (nNextSplitPoint == -1)
			return AUD_PROCESS_CANCELLED;
		MakeSplit(nNextSplitPoint);
		tempvar = m_wavFile.SeekToSample(m_nCurrentSample += m_nSamplesForMinTrackLength);
	}

	if (m_bTrimFile)
	{
		while (nNextSplitPoint = FindNextPoint(SOUND))
		{
			if (nNextSplitPoint == -1)
				return AUD_PROCESS_CANCELLED;
			if (!(nNextSplitPoint = FindNextPoint(SILENCE)))
				break;
			if (nNextSplitPoint == -1)
				return AUD_PROCESS_CANCELLED;
		}
		m_nEndTrim = m_nCurrentSample;
	}

	if (!m_nSplitCount)
	{
		if (m_parentWnd)
		{
			m_parentWnd->MessageBox("No split points could be found.  "
		           "Try adjusting the program options.", "Nothing to do");
		}
		return AUD_NO_SPLIT_POINTS_FOUND;
	}

	if (m_parentWnd)
	{
		// report results of analysis
		m_wndProgress.Hide();
		INT64 nLastSplitLength = (INT64)m_wavFile.NumSamples() - m_SplitPoints[m_nSplitCount - 1];
		if (nLastSplitLength > m_nMaxSplitLength)
			m_nMaxSplitLength = nLastSplitLength;
		if (nLastSplitLength < m_nMinSplitLength)
			m_nMinSplitLength = nLastSplitLength;
		m_nMaxSplitLength /= m_wavFile.SamplingRate();
		m_nMinSplitLength /= m_wavFile.SamplingRate();
		m_nAvgSplitLength = m_wavFile.NumSamples() / (INT64)m_wavFile.SamplingRate() / ((INT64)m_nSplitCount + 1);
		CString tempstr;
		tempstr.Format("Results of analysis:\n\n"
					   "   Number of segments to be created: %5d\n"
					   "   Maximum segment length:\t%2I64d:%02I64d\n"
					   "   Minimum segment length:\t%2I64d:%02I64d\n"
					   "   Average segment length:\t%2I64d:%02I64d\n",
					   m_nSplitCount + 1,
					   m_nMaxSplitLength / 60,
					   m_nMaxSplitLength % 60,
					   m_nMinSplitLength / 60,
					   m_nMinSplitLength % 60,
					   m_nAvgSplitLength / 60,
					   m_nAvgSplitLength % 60);
		if (m_nBeginTrim || m_nEndTrim)
		{
			tempstr.Format("%s"
						   "   Trim %d ms from beginning\n"
						   "   Trim %d ms from end\n\n",
						   tempstr,
						   (UINT)((double)m_nBeginTrim / (double)m_wavFile.SamplingRate() * 1000.0),
						   (UINT)(((double)m_wavFile.NumSamples() - (double)m_nEndTrim) /
								  (double)m_wavFile.SamplingRate() * 1000.0));
		}
		else
		{
			tempstr += "\n";
		}

		if (!g_bRegisteredUser && m_nSplitCount + 1 > 5)
		{
			tempstr.Format("%sOnly the first %d segments\n"
						   "will be created.  Please register\n"
						   "to remove this limit\n\n",
						   tempstr, MAX_SPLITS_FOR_UNREGISTERED_USERS);
		}

		tempstr += "Proceed to create WAV file segments?";
		if (m_parentWnd->MessageBox(tempstr, "Ready to split WAV file", MB_OKCANCEL) == IDOK)
		{
			m_wndProgress.Hide();
			SplitFile();
		}
	}

	return AUD_SUCCESS;
}

INT64 CAudioSeparatorFile::FindNextPoint(POINT_TO_LOOK_FOR p)
{
	WaveFileSample* pSamples = new WaveFileSample[m_nChunkSize];
	unsigned long nZero = m_wavFile.BitsPerSample() == 8 ? 0x80 : 0;
	UINT16 nSilenceThresholdInt =
		(UINT16)((float)(2 << m_wavFile.BitsPerSample()) * m_nSilenceThreshold);
	unsigned long nAvgLeftNoiseLevel, nAvgRightNoiseLevel, nSamplesInAverage, i;
	UINT64 nSampleTotal;
	INT64 returnval = 0;

	while (m_wavFile.ReadSamples(m_nChunkSize, pSamples) == DDC_SUCCESS && !returnval)
	{
		m_nCurrentSample += (INT64)m_nChunkSize;

		if (m_parentWnd)
		{
			// update progress bar
			m_wndProgress.SetPos(m_nCurrentSample);
			m_wndProgress.PeekAndPump();
			if (m_wndProgress.Cancelled())
			{
				returnval = -1;
				goto exit;
			}
		}

		if (m_method == METHOD_AVERAGE_NOISE)
		{
			nAvgLeftNoiseLevel = 0;
			nAvgRightNoiseLevel = 0;
			nSamplesInAverage = 0;
			nSampleTotal = 0;

			for (i = 0; i < m_nChunkSize; i++)
			{
				nSampleTotal += (UINT64)abs(pSamples[i].chan[0]);
			}
			nAvgLeftNoiseLevel = nSampleTotal / m_nChunkSize;
			nSampleTotal = 0;

			if (m_wavFile.NumChannels() > 1)
			{
				for (i = 0; i < m_nChunkSize; i++)
				{
					nSampleTotal += (UINT64)abs(pSamples[i].chan[1]);
				}
				nAvgRightNoiseLevel = nSampleTotal / m_nChunkSize;
			}
			
			if (p == SOUND)
			{
				if (nAvgLeftNoiseLevel - nZero > nSilenceThresholdInt &&
					(nAvgRightNoiseLevel - nZero > nSilenceThresholdInt || m_wavFile.NumChannels() == 1))
				{
					returnval = m_nCurrentSample - (INT64)m_nChunkSize + 1;
				}
			}
			else
			{
				if (nAvgLeftNoiseLevel - nZero < nSilenceThresholdInt &&
					(nAvgRightNoiseLevel - nZero < nSilenceThresholdInt || m_wavFile.NumChannels() == 1))
				{
					returnval = m_nCurrentSample - (INT64)m_nChunkSize + 1;
				}
			}
		}
		else
		{
			BOOL bFoundSound = FALSE;
			
			for (i = 0; i < m_nChunkSize; i++)
			{
				if (p == SOUND)
				{
					if (abs(pSamples[i].chan[0]) - nZero < nSilenceThresholdInt)
						continue;
					if (m_wavFile.NumChannels() > 1)
					{
						if (abs(pSamples[i].chan[1]) - nZero < nSilenceThresholdInt)
							continue;
					}
					returnval = m_nCurrentSample - (INT64)m_nChunkSize + (INT64)i;
					break;
				}
				else
				{
					if (abs(pSamples[i].chan[0]) - nZero > nSilenceThresholdInt)
					{
						bFoundSound = TRUE;
						break;
					}
					if (m_wavFile.NumChannels() > 1)
					{
						if (abs(pSamples[i].chan[1]) - nZero > nSilenceThresholdInt)
						{
							bFoundSound = TRUE;
							break;
						}
					}
				}
			}
			if (p == SILENCE && !bFoundSound)
				returnval = m_nCurrentSample - (INT64)m_nChunkSize + 1;
		}
	}

exit:
	if (pSamples)
		delete [] pSamples;

	return returnval;
}

void CAudioSeparatorFile::MakeSplit(INT64 nNextSplitPoint)
{
	if (m_nSplitCount == 0)
	{
		m_nMaxSplitLength = nNextSplitPoint - m_nBeginTrim - (INT64)m_nChunkSize;
		m_nMinSplitLength = nNextSplitPoint - m_nBeginTrim - (INT64)m_nChunkSize;
	}
	else
	{
		INT64 nCurrentSplitLength = nNextSplitPoint - m_SplitPoints[m_nSplitCount - 1] - (INT64)m_nChunkSize;

		if (m_nMaxSplitLength < nCurrentSplitLength)
			m_nMaxSplitLength = nCurrentSplitLength;
		if (m_nMinSplitLength > nCurrentSplitLength)
			m_nMinSplitLength = nCurrentSplitLength;
	}

	if (m_nSplitCount > m_SplitPoints.size())
		m_SplitPoints.resize(m_SplitPoints.size() + INITIAL_VECTOR_SIZE);
	m_SplitPoints[m_nSplitCount] = nNextSplitPoint;
	m_nSplitCount++;
}

void CAudioSeparatorFile::SplitFile()
{
	unsigned long nZero = m_wavFile.BitsPerSample() == 8 ? 0x80 : 0;
	WaveFileSample* pSamples = new WaveFileSample[m_nChunkSize];
	char szDirName[MAX_PATH];
	CString strFileName, strTemp, strTime;
	int nDigitLength, secondsremaining;
	INT64 j, counter = 0;
	strTemp.Format("%d", m_nSplitCount);
	nDigitLength = strTemp.GetLength();
	CTime starttime = CTime::GetCurrentTime();
	CTime now = CTime::GetCurrentTime();
	CTimeSpan elapsedtime, oldelapsedtime;
	HANDLE hCurrentProcess = GetCurrentProcess();
	UINT16 nChannels = 0;
	INT64 nSamples = (INT64)m_wavFile.NumSamples();

	m_nCurrentSample = 0;
	if (m_parentWnd)
	{
		m_wndProgress.SetPos(m_nCurrentSample);
		m_wndProgress.SetText("Creating WAV files...");
		m_wndProgress.GoModal();
	}

	strTemp.Format("%%s\\%s%%0%dd.wav", m_strBeginWith, nDigitLength); 

	GetCurrentDirectory(MAX_PATH, szDirName);
	
	m_wavFile.SeekToSample(0);

	if (m_nBeginTrim)
	{
		m_nCurrentSample = m_nBeginTrim;
		m_wavFile.SeekToSample(m_nBeginTrim);
	}

	// set process to lower priority to avoid deadlocks (hopefully)
	SetPriorityClass(hCurrentProcess, IDLE_PRIORITY_CLASS);
	for (int i = 0; i < (m_nSplitCount + 1) && (g_bRegisteredUser || i < MAX_SPLITS_FOR_UNREGISTERED_USERS); i++)
	{
		WaveFile outFile;

		strFileName.Format((LPCTSTR)strTemp, szDirName, i + 1);
		if (outFile.OpenForWrite(strFileName, m_wavFile) != DDC_SUCCESS)
		{
			CString strTemp2;
			if (m_parentWnd)
			{
				m_wndProgress.Hide();
				strTemp2.Format("File error.  %s could not be opened for writing.  WAV file creation halted.", strFileName);
				m_parentWnd->MessageBox(strTemp2, "Error", MB_ICONERROR);
			}
			break;
		}
		
		while
			(
			(i == m_nSplitCount &&	((m_nEndTrim && m_nCurrentSample < m_nEndTrim) || (!m_nEndTrim && m_nCurrentSample < nSamples)))
			 || m_nCurrentSample < m_SplitPoints[i]
			)
		{
			m_wavFile.ReadSamples(m_nChunkSize, pSamples);
			nChannels = m_wavFile.NumChannels();
			if (m_wavFile.BitsPerSample() == 8)
			{
				for (j = 0; j < m_nChunkSize &&
					 ((!m_nEndTrim && j + m_nCurrentSample < nSamples) ||
					 (m_nEndTrim && j + m_nCurrentSample < m_nEndTrim)); j++)
				{
					outFile.WriteData((UINT8*)pSamples[j].chan,	nChannels);
				}
			}
			else
			{
				for (j = 0; j < m_nChunkSize &&
					 ((!m_nEndTrim && j + m_nCurrentSample < nSamples) ||
					 (m_nEndTrim && j + m_nCurrentSample < m_nEndTrim)); j++)
				{
					outFile.WriteData((INT16*)pSamples[j].chan, nChannels);
				}
			}
			m_nCurrentSample += m_nChunkSize;

			if (m_parentWnd && counter++ % 25 == 0)
			{
				elapsedtime = CTime::GetCurrentTime() - starttime;
				oldelapsedtime = CTime::GetCurrentTime() - now;
				secondsremaining = elapsedtime.GetTotalSeconds();	// temporary placeholder
				if (secondsremaining && oldelapsedtime.GetTotalSeconds())
				{
					secondsremaining = (int)(secondsremaining / ((double)m_nCurrentSample / (double)nSamples)) - secondsremaining;
					strTime.Format("Creating WAV files...  Estimated time remaining: %02d:%02d:%02d",
					               secondsremaining / 60 / 60,
								   (secondsremaining / 60 > 60 ? (secondsremaining / 60) % 60 : secondsremaining / 60),
								   secondsremaining % 60);
					m_wndProgress.SetText(strTime);
					now = CTime::GetCurrentTime();
				}

				m_wndProgress.SetPos(m_nCurrentSample);
				m_wndProgress.PeekAndPump();
				if (m_wndProgress.Cancelled())
				{
					outFile.Close();
					goto exit;
				}
			}
		}

		if (m_nAppendSilence && (i + 1 != m_nSplitCount))	// don't add silence to last track
		{
			const INT16 SilenceSample [MAX_WAVE_CHANNELS] = { nZero, nZero };
			UINT nNumSamples = (UINT)((float)m_nAppendSilence *
				m_wavFile.SamplingRate() * 0.001);
			for (UINT k = 0; k < nNumSamples; k++)
			{
				outFile.WriteSample(SilenceSample);
			}
		}
		
		outFile.Close();
	}

exit:
	SetPriorityClass(hCurrentProcess, NORMAL_PRIORITY_CLASS);
	if (pSamples)
		delete [] pSamples;
}
