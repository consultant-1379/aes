//	********************************************************
//
//	 COPYRIGHT Ericsson 2015
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2015.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2015 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************

#include "engine/ftpconnection.h"

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <boost/format.hpp>

#ifndef CUTE_TEST
#include "common/tracer.h"
#include "common/logger.h"
#else
#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_FtpConnection)

namespace
{
	const std::string FTP_URL = "ftp://%s:%s@%s:%d/%s/%s";
	const std::string FTP_NoRemoteFolder_URL = "ftp://%s:%s@%s:%d/%s";
	const std::string RENAME_FROM = "RNFR ";
	const std::string RENAME_TO = "RNTO ";
	const size_t MAX_LIST_LENGTH = 4096;
	const long CONNECTION_TIMEOUT = 10L;
	const long RESPONSE_TIMEOUT = 5L;
	const long TRANSFER_TIMEOUT = 10L;
	char FTP_NL_LINE_FEED = 0x0A; //NL line feed: used by FTP protocol to separate answer entries on list commands

	struct FtpListOutput
	{
		char listOutput[MAX_LIST_LENGTH];
		size_t offset;
	};
}

namespace engine {

	FtpConnection::FtpConnection(const operation::filePeerInfo& data, const std::string& producer)
	: PeerConnection(data, producer),
	  m_curlHandle(NULL)
	{

	}

	FtpConnection::FtpConnection(const operation::filePeerInfo& data)
	: PeerConnection(data),
	  m_curlHandle(NULL)
	{

	}

	FtpConnection::~FtpConnection()
	{
		disconnect();
	}


	int FtpConnection::connect()
	{
		int result = common::errorCode::ERR_GENERIC;

		updateConfiguration();

		if(DISCONNECTED == m_connectionStatus)
		{
			/* get a curl handle */
			m_curlHandle = curl_easy_init();

			if(m_curlHandle)
			{
				/* specify target */
				boost::format remoteUrlFormat;
				bool remoteFolderProvided = false;
				remoteFolderProvided = strlen(m_data.getRemoteFolderPath());
				if(remoteFolderProvided)
					remoteUrlFormat = boost::format(FTP_URL);
				else
					remoteUrlFormat = boost::format(FTP_NoRemoteFolder_URL);
				remoteUrlFormat.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );
				std::string hostAddress; // = "[" + m_data.getIpAddress() + "]";
				hostAddress.assign("[");
				hostAddress.append(m_data.getIpAddress());
				hostAddress.append("]");
				if((hostAddress.find(':') != std::string::npos) && (hostAddress.find('.') == std::string::npos))  // && (p_IPAddress.find('.') == std::string::npos))
				{
					if(remoteFolderProvided)
						remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % hostAddress % m_data.getPortNumber() % m_data.getRemoteFolderPath() % m_currentFileName;
					else
						remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % hostAddress % m_data.getPortNumber() % m_currentFileName;
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] IPv6 FTP remoteUrlFormat towards <%s>.",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
							m_data.ipAddress.c_str());
				}
				else
				{
					if(remoteFolderProvided)
						remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % m_data.getIpAddress() % m_data.getPortNumber() % m_data.getRemoteFolderPath() % m_currentFileName;
					else
						remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % m_data.getIpAddress() % m_data.getPortNumber() % m_currentFileName;
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] IPv4 FTP remoteUrlFormat towards <%s>.",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
							m_data.ipAddress.c_str());

				}
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "CURLOPT_URL command -> %s", remoteUrlFormat.str().c_str());
				curl_easy_setopt(m_curlHandle, CURLOPT_URL, remoteUrlFormat.str().c_str());

				// No DATA transfer open only control connection
				curl_easy_setopt(m_curlHandle, CURLOPT_NOBODY, 1L);

				// Set a Connection Timeout
				// It should contain the maximum time in seconds that you allow the connection phase
				// to the server to take. This only limits the connection phase, it has no impact once it has connected.
				curl_easy_setopt(m_curlHandle, CURLOPT_CONNECTTIMEOUT, CONNECTION_TIMEOUT);

				//Set the timeout period (in seconds) on the amount of time that the server
				//is allowed to take in order to send a response message for a command before the session is considered dead.
				curl_easy_setopt(m_curlHandle, CURLOPT_FTP_RESPONSE_TIMEOUT, RESPONSE_TIMEOUT);

				// Set Signal OFF
				// libcurl will not use any functions that install signal handlers or any functions that cause signals
				// to be sent to the process. This option is here to allow multi-threaded unix applications to
				// still set/use all timeout options etc, without risking getting signals.
				curl_easy_setopt(m_curlHandle, CURLOPT_NOSIGNAL, 1L);

				//*****************************************************************
				//SET FTP ACTIVE MODE

				//Disable EPRT
				curl_easy_setopt(m_curlHandle, CURLOPT_FTP_USE_EPRT, 0L);

				//Use PORT
				curl_easy_setopt(m_curlHandle, CURLOPT_FTPPORT, "-");

				//Create the directory if needed
				curl_easy_setopt(m_curlHandle, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);

				//*******************************************************************

				/* Now run off and do what you've been told! */
				CURLcode res = curl_easy_perform(m_curlHandle);

				/* Check for errors */
				if(CURLE_OK == res || CURLE_REMOTE_FILE_NOT_FOUND == res) // Added error code as a part of IA50778
				{
					m_connectionStatus = CONNECTED;

					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] FTP connection towards <%s> success, return code = %d",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
								m_data.ipAddress.c_str(),res);

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] FTP connection towards <%s> success, return code = %d",
										  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
										  m_data.ipAddress.c_str(),res);

					result = common::errorCode::ERR_NO_ERRORS;
					curl_easy_reset(m_curlHandle); //(HZ40586 - Resets to default value, if connection is success)
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] FTP connection towards <%s> failed. Internal error: %s",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
								m_data.ipAddress.c_str(), curl_easy_strerror(res));

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: FTP connection towards <%s> failed. Internal error: %s",
										  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
										  m_data.ipAddress.c_str(), curl_easy_strerror(res));

					if(m_curlHandle)		
                        		{
	                	                curl_easy_cleanup(m_curlHandle); //(HZ40586- Cleans up the Ftp object instead of reset)
        		                        m_curlHandle = NULL; //(Fix for the TR IA60968)
                        		}

				}

			}
		}

		return result;
	}

	int FtpConnection::disconnect()
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		if(CONNECTED == m_connectionStatus)
		{
			if(m_curlHandle)
			{
				curl_easy_cleanup(m_curlHandle);
				m_curlHandle = NULL;
			}

			m_connectionStatus = DISCONNECTED;
		}

		return result;
	}

	int FtpConnection::send(void* data, size_t dataLength)
	{

		  int result = common::errorCode::ERR_API_CALL;

		  if(m_curlHandle) {
			  /* we want to use our own read function */
			  curl_easy_setopt(m_curlHandle, CURLOPT_READFUNCTION, FtpConnection::readCallback);

			  /* enable uploading */
			  curl_easy_setopt(m_curlHandle, CURLOPT_UPLOAD, 1L);

			  /* specify target */
			  boost::format remoteUrlFormat;
			  bool remoteFolderProvided = false;
			  remoteFolderProvided = strlen(m_data.getRemoteFolderPath());
			  if(remoteFolderProvided)
				  remoteUrlFormat = boost::format(FTP_URL);
			  else
				  remoteUrlFormat = boost::format(FTP_NoRemoteFolder_URL);
			  remoteUrlFormat.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );
			  std::string hostAddress; // = "[" + m_data.getIpAddress() + "]";
			  hostAddress.assign("[");
			  hostAddress.append(m_data.getIpAddress());
			  hostAddress.append("]");
			  if((hostAddress.find(':') != std::string::npos) && (hostAddress.find('.') == std::string::npos))  // && (p_IPAddress.find('.') == std::string::npos))
			  {
				  if(remoteFolderProvided)
					  remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % hostAddress % m_data.getPortNumber() % m_data.getRemoteFolderPath() % m_currentFileName;
				  else
					  remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % hostAddress % m_data.getPortNumber() % m_currentFileName;
				  AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] IPv6 FTP remoteUrlFormat towards <%s>.",
						  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						  m_data.ipAddress.c_str());
			  }
			  else
			  {
				  if(remoteFolderProvided)
					  remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % m_data.getIpAddress() % m_data.getPortNumber() % m_data.getRemoteFolderPath() % m_currentFileName;
				  else
					  remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % m_data.getIpAddress() % m_data.getPortNumber() % m_currentFileName;
				  AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] IPv4 FTP remoteUrlFormat towards <%s>.",
						  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						  m_data.ipAddress.c_str());

			  }
			  AES_DDT_LOG(LOG_LEVEL_DEBUG, "CURLOPT_URL command -> %s", remoteUrlFormat.str().c_str());
			  curl_easy_setopt(m_curlHandle, CURLOPT_URL, remoteUrlFormat.str().c_str());

			  /* now specify which data to upload */
			  Data container;
			  container.buffer = data;
			  container.iterator = 0;
			  container.size = dataLength;

			  curl_easy_setopt(m_curlHandle, CURLOPT_READDATA, &container);

			  /* Set the size of the file to upload (optional).  If you give a *_LARGE
		         option you MUST make sure that the type of the passed-in argument is a
		         curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
		         make sure that to pass in a type 'long' argument. */
			  curl_easy_setopt(m_curlHandle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)dataLength);

			  //The maximum time in seconds that you allow the libcurl transfer operation to take.
			  curl_easy_setopt(m_curlHandle,CURLOPT_TIMEOUT, TRANSFER_TIMEOUT);

			  // Set Signal OFF
			  // libcurl will not use any functions that install signal handlers or any functions that cause signals
			  // to be sent to the process. This option is here to allow multi-threaded unix applications to
			  // still set/use all timeout options etc, without risking getting signals.
			  curl_easy_setopt(m_curlHandle, CURLOPT_NOSIGNAL, 1L);

			  //*****************************************************************

			  //Disable EPRT
			  curl_easy_setopt(m_curlHandle, CURLOPT_FTP_USE_EPRT, 0L);

			  //Use PORT
			  curl_easy_setopt(m_curlHandle, CURLOPT_FTPPORT, "-");

			  //Create the directory if needed
			  curl_easy_setopt(m_curlHandle, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);

			  //Display verbose information: good if you start ddt in debug
			  //curl_easy_setopt(m_curlHandle, CURLOPT_VERBOSE, 1L);

			  curl_easy_setopt(m_curlHandle, CURLOPT_APPEND, 1L);

			  //*******************************************************************

			  /* Now run off and do what you've been told! */
			  CURLcode res = curl_easy_perform(m_curlHandle);
			  /* Check for errors */
			  if(res == CURLE_OK)
			  {
				  AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] FTP upload towards <%s> success.",
						  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						  m_data.ipAddress.c_str());

				  AES_DDT_TRACE_MESSAGE("[%s::%s@%s] FTP upload towards <%s> success.",
						  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						  m_data.ipAddress.c_str());

				  result = common::errorCode::ERR_NO_ERRORS;
			  }
			  else
			  {
				  AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] FTP upload towards <%s> failed. Internal error: %s",
						  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						  m_data.ipAddress.c_str(), curl_easy_strerror(res));

				  AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: FTP upload towards <%s> failed. Internal error: %s",
						  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						  m_data.ipAddress.c_str(), curl_easy_strerror(res));

			  }

			  curl_easy_reset(m_curlHandle);
		  }

		  return result;
	}

	int FtpConnection::openRemoteContainer(const std::string& remoteFileName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		if(configurationChanged() && ((result = restartConnection()) != common::errorCode::ERR_NO_ERRORS))
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Cannot restart connection on MODIFY callback",	m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Cannot restart connection on MODIFY callback",	m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			return result;
		}

		//set on file open
		m_currentFileName.assign(remoteFileName);
		refreshTempFileExtension();
		m_currentFileName.append(m_tempToken);//TEMPORARY_FILE_EXTENSION

		//Fake send, just meant to create the file
		result = send(NULL,0);

		return result;
	}

	int FtpConnection::closeRemoteContainer(const std::string& remoteFileName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;
		std::string temporaryFileName(remoteFileName);
		temporaryFileName.append(m_tempToken);//TEMPORARY_FILE_EXTENSION

		result = renameRemoteFile(temporaryFileName, remoteFileName);
		if(common::errorCode::ERR_NO_ERRORS != result)
		{
			time_t now = time(0);
			char tmp[512] = {0};
			snprintf(tmp, sizeof(tmp), "_%ld", now);
			std::string remoteFileNameTimestamp(remoteFileName);
			remoteFileNameTimestamp.append(tmp);
			result = renameRemoteFile(temporaryFileName, remoteFileNameTimestamp);
		}

		if(common::errorCode::ERR_NO_ERRORS == result)
		{
			//clean up on file close
			m_currentFileName.clear();
		}

		return result;
	}

	int FtpConnection::renameTempFiles()
	{
		int result = common::errorCode::ERR_GENERIC;
		std::list<std::string> fileList;

		if(DISCONNECTED == m_connectionStatus)
		{
			/* get a curl handle */
			m_curlHandle = curl_easy_init();

			if(m_curlHandle)
			{
				/* specify target */
				boost::format remoteUrlFormat;
				bool remoteFolderProvided = false;
				remoteFolderProvided = strlen(m_data.getRemoteFolderPath());
				if(remoteFolderProvided)
					remoteUrlFormat = boost::format(FTP_URL);
				else
					remoteUrlFormat = boost::format(FTP_NoRemoteFolder_URL);
				remoteUrlFormat.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );

				std::string hostAddress; // = "[" + m_data.getIpAddress() + "]";
				hostAddress.assign("[");
				hostAddress.append(m_data.getIpAddress());
				hostAddress.append("]");
				if((hostAddress.find(':') != std::string::npos) && (hostAddress.find('.') == std::string::npos))  // && (p_IPAddress.find('.') == std::string::npos))
				{
					if(remoteFolderProvided)
						remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % hostAddress % m_data.getPortNumber() % m_data.getRemoteFolderPath() % m_currentFileName;
					else
						remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % hostAddress % m_data.getPortNumber() % m_currentFileName;
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] IPv6 FTP remoteUrlFormat towards <%s>.",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
							m_data.ipAddress.c_str());
				}
				else
				{
					if(remoteFolderProvided)
						remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % m_data.getIpAddress() % m_data.getPortNumber() % m_data.getRemoteFolderPath() % m_currentFileName;
					else
						remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % m_data.getIpAddress() % m_data.getPortNumber() % m_currentFileName;
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] IPv4 FTP remoteUrlFormat towards <%s>.",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
							m_data.ipAddress.c_str());
				}
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "CURLOPT_URL command -> %s", remoteUrlFormat.str().c_str());
				curl_easy_setopt(m_curlHandle, CURLOPT_URL, remoteUrlFormat.str().c_str());

				// NOTE: dont't enable CURLOPT_NOBODY option to get list output

				//*****************************************************************
				//SET FTP ACTIVE MODE

				//Disable EPRT
				curl_easy_setopt(m_curlHandle, CURLOPT_FTP_USE_EPRT, 0L);

				//Use PORT
				curl_easy_setopt(m_curlHandle, CURLOPT_FTPPORT, "-");

				//Display verbose information.
				curl_easy_setopt(m_curlHandle, CURLOPT_VERBOSE, 1L);

				//*******************************************************************
				//SET LIST OPTIONS

				//Perform Listing
				curl_easy_setopt(m_curlHandle, CURLOPT_DIRLISTONLY, 1);

				//Set the callback that will receive the list output
				curl_easy_setopt(m_curlHandle, CURLOPT_WRITEFUNCTION, FtpConnection::listCallback);

				// pass the locale buffer structure to the callback function
				FtpListOutput serverAnswer;
				memset(&serverAnswer, 0, sizeof(serverAnswer));

				//FtpConnection::listCallback will receive in input: serverAnswer
				curl_easy_setopt(m_curlHandle, CURLOPT_WRITEDATA, (void *)&serverAnswer);

				//*******************************************************************

				//We don't care to create the remote folder at this stage: skip option 'CURLOPT_FTP_CREATE_MISSING_DIRS'

				/* Now run off and do what you've been told! */
				CURLcode res = curl_easy_perform(m_curlHandle);

				/* Check for errors */
				if(CURLE_OK == res)
				{
					m_connectionStatus = CONNECTED;

					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] FTP connection towards <%s> success.",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
								m_data.ipAddress.c_str());

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] FTP connection towards <%s> success.",
										  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
										  m_data.ipAddress.c_str());


					//*******************************************************************
					// Split FTP-LIST answer text for further processing
					std::stringstream ftplist(serverAnswer.listOutput);
					std::string item;
					char tokenchar = FTP_NL_LINE_FEED; //NL line feed: used by FTP protocol to separate folder entries

					while (std::getline(ftplist, item, tokenchar))
					{
						AES_DDT_TRACE_MESSAGE("[%s::%s@%s] FTP entry: '%s'", m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), item.c_str());
						fileList.push_back(item);
					}

					//*******************************************************************

					result = common::errorCode::ERR_NO_ERRORS;
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] FTP connection towards <%s> failed. Internal error: %s",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
								m_data.ipAddress.c_str(), curl_easy_strerror(res));

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: FTP connection towards <%s> failed. Internal error: %s",
										  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
										  m_data.ipAddress.c_str(), curl_easy_strerror(res));
				}

				//RESET OPERATION
				curl_easy_reset(m_curlHandle);


				//*******************************************************************
				// Process FTP-LIST entries: Check and Rename tmp files

				// set the token to temporary file extension
				std::string token(common::datatransfer::TEMPORARY_FILE_EXTENSION);

				//loop through all regular files
				for(std::list<std::string>::const_iterator fileIterator = fileList.begin(); fileIterator != fileList.end(); ++fileIterator)
				{
					std::string sourceFileName = *fileIterator;

					size_t lastOfIndex = sourceFileName.rfind(token);

					if(lastOfIndex != std::string::npos)
					{
						if (sourceFileName.size() - 4 == lastOfIndex)
						{
							//token is at the end

							std::string destFileName(sourceFileName);

							//remove the extension
							destFileName.replace(lastOfIndex, token.size(), "");

							//Rename
							result = renameRemoteFile(sourceFileName, destFileName);

							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] renameRemoteFile(from: '%s', to: '%s'), result: <%d>",
												  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
												  sourceFileName.c_str(), destFileName.c_str(), result);

							AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] libssh2_sftp_rename(from: '%s', to: '%s'), result: <%d>",
										m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
										sourceFileName.c_str(), destFileName.c_str(), result);
						}
					}
				}

				//DISCONNECT
				curl_easy_cleanup(m_curlHandle);
				m_curlHandle = NULL;
			}
		}

		return result;
	}


	size_t FtpConnection::readCallback(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		size_t retcode = 0;
		Data* inputData = static_cast<Data*> (stream);
		if (inputData)
		{
			size_t dataLeft = inputData->size - inputData->iterator;
			size_t maxChunkSize = size * nmemb;
			retcode = dataLeft < maxChunkSize ? dataLeft : maxChunkSize;

			memcpy(ptr, ((const char*) inputData->buffer) + inputData->iterator, retcode);

			inputData->iterator += retcode;  // <-- save progress
		}

		return retcode;
	}

	size_t FtpConnection::listCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
	{
		size_t receivedBytes = size * nmemb;
		FtpListOutput* listOutput = static_cast<FtpListOutput*>(userdata);

		// If there's more space left, copy all received bytes, but
		// ignore the printout if there's no more space in the buffer.
		// alias: if (listOutput->offset == MAX_LIST_LENGTH), this will copy 0 bytes

		size_t bytesToCopy =
				((receivedBytes <= (MAX_LIST_LENGTH - listOutput->offset)) ? receivedBytes : (MAX_LIST_LENGTH - listOutput->offset));

		memcpy((listOutput->listOutput + listOutput->offset), ptr, bytesToCopy);
		listOutput->offset += bytesToCopy;//move offset on

		return receivedBytes;
	}

	int FtpConnection::renameRemoteFile(const std::string& oldName, const std::string& newName)
	{
		int result = common::errorCode::ERR_API_CALL;

		if(m_curlHandle)
		{
			curl_slist *headerlist = NULL;

			std::string renameFrom(RENAME_FROM + oldName);
			std::string renameTo(RENAME_TO + newName);

			/* build a list of commands to pass to libcurl */
			headerlist = curl_slist_append(headerlist, renameFrom.c_str());
			headerlist = curl_slist_append(headerlist, renameTo.c_str());


			/* specify target */
			boost::format remoteUrlFormat;
			bool remoteFolderProvided = false;
			remoteFolderProvided = strlen(m_data.getRemoteFolderPath());
			if(remoteFolderProvided)
				remoteUrlFormat = boost::format(FTP_URL);
			else
				remoteUrlFormat = boost::format(FTP_NoRemoteFolder_URL);
			remoteUrlFormat.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );
			std::string hostAddress; // = "[" + m_data.getIpAddress() + "]";
			hostAddress.assign("[");
			hostAddress.append(m_data.getIpAddress());
			hostAddress.append("]");
			if((hostAddress.find(':') != std::string::npos) && (hostAddress.find('.') == std::string::npos))  // && (p_IPAddress.find('.') == std::string::npos))
			{
				if(remoteFolderProvided)
					remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % hostAddress % m_data.getPortNumber() % m_data.getRemoteFolderPath() % m_currentFileName;
				else
					remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % hostAddress % m_data.getPortNumber() % m_currentFileName;
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] IPv6 FTP remoteUrlFormat towards <%s>.",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						m_data.ipAddress.c_str());
			}
			else
			{
				if(remoteFolderProvided)
					remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % m_data.getIpAddress() % m_data.getPortNumber() % m_data.getRemoteFolderPath() % m_currentFileName;
				else
					remoteUrlFormat % m_data.getUserName() % m_data.getPassword() % m_data.getIpAddress() % m_data.getPortNumber() % m_currentFileName;
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] IPv4 FTP remoteUrlFormat towards <%s>.",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						m_data.ipAddress.c_str());
			}
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "CURLOPT_URL command -> %s", remoteUrlFormat.str().c_str());
			curl_easy_setopt(m_curlHandle, CURLOPT_URL, remoteUrlFormat.str().c_str());

			// Enable ftp data connection
			curl_easy_setopt(m_curlHandle, CURLOPT_NOBODY, 1L);

			/* pass in that last of FTP commands to run after the transfer */
			curl_easy_setopt(m_curlHandle, CURLOPT_POSTQUOTE, headerlist);

			//Disable EPRT
			curl_easy_setopt(m_curlHandle, CURLOPT_FTP_USE_EPRT, 0L);

			//Use PORT
			curl_easy_setopt(m_curlHandle, CURLOPT_FTPPORT, "-");

			//Display verbose information.
			curl_easy_setopt(m_curlHandle, CURLOPT_VERBOSE, 1L);

			/* Now run off and do what you've been told! */
			CURLcode res = curl_easy_perform(m_curlHandle);

			/* Check for errors */
			if(res == CURLE_OK)
			{

				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] FTP rename(%s, %s) on <%s> success.",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						oldName.c_str(), newName.c_str(), m_data.ipAddress.c_str());

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] FTP rename(%s, %s) on <%s> success.",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						oldName.c_str(), newName.c_str(), m_data.ipAddress.c_str());

				result = common::errorCode::ERR_NO_ERRORS;
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] FTP rename(%s, %s) on  <%s> failed. Internal error: %s",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						oldName.c_str(), newName.c_str(), m_data.ipAddress.c_str(), curl_easy_strerror(res));

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: FTP rename(%s, %s) on <%s> failed. Internal error: %s",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						oldName.c_str(), newName.c_str(), m_data.ipAddress.c_str(), curl_easy_strerror(res));
			}


			curl_easy_reset(m_curlHandle);

			/* clean up the FTP commands list */
			curl_slist_free_all (headerlist);
		}

		return result;

	}

} /* namespace engine */
