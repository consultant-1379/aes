#include "aes_cdh_sftp.h"
#include <ctype.h>
#include <iostream>


/* NOTE: Before executing this main, make sure that ip adresses and passwords are correctly placed here in the program.
 * Also while checking other sftp functions, make sure that appropriate directories and files are present in
 * remote machine for which you are going to be connected through SFTP
 * This note is important for SFTP testing
 */
int main()
		{
			class SftpAdapterClass *sftpAdp;
			sftpAdp = new SftpAdapterClass ();
#if 0
			cout<<"*******************************TEST::SFTPCONNECT*********************************"<<endl;
			cout<<"Test1: Check whether connection is not established when ipadress/host name is given wrong"<< endl;
			if (sftpAdp->sftpConnect("141.137.47.57", 22, "root" ,"root",1)==0)
			{
				cout<< "sftp connection failed "<<endl;
			}
			cout<<"Test2: Check whether connection is not established when userid name is given wrong"<< endl;
			if (sftpAdp->sftpConnect("141.137.47.54", 22, "root1" ,"root",1)==0)
			{
				cout<< "sftp connection failed "<<endl;
			}
			cout<<"Test3: Check whether connection is not established when ipadress/host name is given wrong"<< endl;
			if (sftpAdp->sftpConnect("141.137.47.54", 22, "root" ,"root1",1)==0)
			{
				cout<< "sftp connection failed "<<endl;
			}
			cout<<"Test4: Check whether connection is not established when ipadress/host name is given wrong"<< endl;
			if (sftpAdp->sftpConnect("141.137.47.54", 26, "root" ,"root",1)==0)
			{
				cout<< "sftp connection failed "<<endl;
			}
			cout<<"Test5: Check whether connection is done when all valid parameteres are given"<< endl;
#endif
		//	if (sftpAdp->sftpConnect("141.137.47.54",22 , "team6" ,"tcs@1234",1)==0)
			if (sftpAdp->sftpConnect("141.137.47.121",22 , "administrator" ,"Administrator1@",1)==0)
			{
				cout<< "sftp connection failed "<<endl;
				return 0;
			}
			cout<< "sftp connection sucess "<<endl;
			//***************************************************************************

			cout<<"*************************** TEST:: CREATE DIRECTORIES***********************"<<endl;
#if 0
			cout<<"Test6: create a new directory with junk values"<< endl;
			if(sftpAdp->sftpCreateDirectories("/opt/ap/SFTP_TEST/Z^Z1^Z1")==true)
				cout<<"created the directory: /opt/ap/SFTP_TEST/Z^Z1^Z1"<<endl;
			else
				cout<<"Not created the directory: /opt/ap/SFTP_TEST/Z^Z1^Z1"<<endl;

			cout<<"Test7: create a new directory in windows(with no permissions for parent directory)"<< endl;
			if(sftpAdp->sftpCreateDirectories("Durga222")==true)
				cout<<"created the directory: Durga222"<<endl;
			else
				cout<<"Not created the directory: Durga222"<<endl;

			cout<<"Test8: create a new directory with wrong path1"<< endl;
			if(sftpAdp->sftpCreateDirectories("/opt/SFTP/ap/SFTP1")==true)
				cout<<"created the directory: /opt/SFTP/ap/SFTP1"<<endl;
			else
				cout<<"Not created the directory: /opt/SFTP/ap/SFTP1"<<endl;

			cout<<"Test9: create a directory when directory already exists "<< endl;
			if(sftpAdp->sftpCreateDirectories("/opt/ap/SFTP_TEST")==true)
				cout<<"created the directory: /opt/ap/SFTP_TEST"<<endl;
			else
				cout<<"Not created the directory: /opt/ap/SFTP_TEST"<<endl;


			cout<<durga"Test10: create a new directory "<< endl;
			if(sftpAdp->sftpCreateDirectories("/opt/ap/SFTP_TEST/NEWDIRCREATED")==true)
				cout<<"created the directory: /opt/ap/SFTP_TEST/NEWDIRCREATED"<<endl;
			else
				cout<<"Not created the directory: /opt/ap/SFTP_TEST/NEWDIRCREATED"<<endl;

			//********************************** TEST::RENAMEDIR******************

			cout<<"******************TEST::RENAMEDIR****************************"<<endl;
			cout<<"Test11: Rename a directory with junk values to another zunk values"<< endl;
			if(sftpAdp->renameRemoteDir("/opt/ap/SFTP_TEST/Z^Z1^Z1","/opt/ap/SFTP_TEST/^Z1^Z1DONE" )==true)
				cout<<"Renamed the directory to : /opt/ap/SFTP_TEST/^Z1^Z1DONE"<<endl;
			else
				cout<<"Did not rename the directory to: /opt/ap/SFTP_TEST/^Z1^Z1DONE"<<endl;

			cout<<"Test12: Rename a directory with no permissions for parent directory"<< endl;
			if(sftpAdp->renameRemoteDir("/opt/ap/SFTP_TEST/NOPERM1","/opt/ap/SFTP_TEST/NOPERMRENAMEDNO")==true)
				cout<<"Renamed the directory to: /opt/ap/SFTP_TEST/NOPERMRENAMEDNO"<<endl;
			else
				cout<<"Did not rename the directory to: /opt/ap/SFTP_TEST/NOPERMRENAMEDNO"<<endl;

			cout<<"Test13: Rename a directory with wrong path1"<< endl;
			if(sftpAdp->renameRemoteDir("/opt/SFTP/ap/SFTP1","/opt/SFTP/ap/SFTP2 ")==true)
				cout<<"Renamed the directory: /opt/SFTP/ap/SFTP2"<<endl;
			else
				cout<<"Did not rename the directory to: /opt/SFTP/ap/SFTP2"<<endl;

			cout<<"Test14: Rename a directory to already existing name "<< endl;
			if(sftpAdp->renameRemoteDir("/opt/ap/SFTP_TEST/NEWDIRCREATED","opt/ap/SFTP_TEST/NEWDIRCREATEDALREADY")==true)
				cout<<"Renamed the directory to: opt/ap/SFTP_TEST/NEWDIRCREATEDALREADY"<<endl;
			else
				cout<<"Did not rename the directory to: opt/ap/SFTP_TEST/NEWDIRCREATEDALREADY"<<endl;


			cout<<"Test15: Rename a directory with new name "<< endl;
			//if(sftpAdp->renameRemoteDir("/oout<<"In Main getting SFTP disconnected"<<endl;
			sftpAdp->sftpDisconnect();
			cout<<"connecting to 141.137.32.14"<<endl;
			if (sftpAdp->sftpConnect("141.137.32.14",22 , "root" ,"root",1)==0)
			{
				cout<< "sftp connection failed "<<endl;
				return 0;
			}
			cout<< "sftp connection sucess "<<endl;
			//delete sftpt/ap/SFTP_TEST/NEWDIRCREATED","/opt/ap/SFTP_TEST/NEWDIRCREATED1")==true)
				cout<<"Renamed the directory to: /opt/ap/SFTP_TEST/NEWDIRCREATED1"<<endl;
			else
				cout<<"Did not rename the directory to: /opt/ap/SFTP_TEST/NEWDIRCREATED1"<<endl;

			//**************************TEST::RENAMEFILE*********************

			cout<<"******************TEST::renameRemoteFile****************************"<<endl;
			cout<<"Test16: Rename a File with junk values to another zunk values"<< endl;
			if(sftpAdp->renameRemoteFile("/opt/ap/SFTP_TEST/^Z1^Z1","/opt/ap/SFTP_TEST/^Z1^Z1FileDONE" )==true)
				cout<<"Renamed the File to : /opt/ap/SFTP_TEST/^Z1^Z1FileDONE"<<endl;
			else
				cout<<"Did not rename the File to: /opt/ap/SFTP_TEST/^Z1^Z1FileDONE"<<endl;

			cout<<"Test17: Rename a File NOTE:with no permissions "<< endl;
			if(sftpAdp->renameRemoteFile("/opt/ap/SFTP_TEST/NOPERM/FILENOPERM","/opt/ap/SFTP_TEST/NOPERM/NOPERMRENAMED")==true)
				cout<<"Renamed the File to: /opt/ap/SFTP_TEST/NOPERM/NOPERMRENAMED"<<endl;
			else
				cout<<"Did not rename the File to: /opt/ap/SFTP_TEST/NOPERM/NOPERMRENAMED"<<endl;

			cout<<"Test18: Rename a File with wrong path1"<< endl;
			if(sftpAdp->renameRemoteFile("/opt/SFTP/ap/SFTP1","/opt/SFTP/ap/SFTP2 ")==true)
				cout<<"Renamed the File: /opt/SFTP/ap/SFTP2"<<endl;
			else
				cout<<"Did not rename the File to: /opt/SFTP/ap/SFTP2"<<endl;

			cout<<"Test19: Rename a File to already existing name "<< endl;
			if(sftpAdp->renameRemoteFile("/opt/ap/SFTP_TEST/SFTP1","opt/ap/SFTP_TEST/sftp2")==true)
				cout<<"Renamed the File to: opt/ap/SFTP_TEST/sftp2"<<endl;
			else
				cout<<"Did not rename the File to: opt/ap/SFTP_TEST/sftp2"<<endl;


			cout<<"Test20: Rename a File with new name "<< endl;
			if(sftpAdp->renameRemoteFile("/opt/ap/SFTP_TEST/sftp3.tmp","/opt/ap/SFTP_TEST/NEWDIRCREATED1/sftp22.txt")==true)
				cout<<"Renamed the File to: /opt/ap/SFTP_TEST/sftp22.txt"<<endl;
			else
				cout<<"Did not rename the File to: /opt/ap/SFTP_TEST/sftp22.txt"<<endl;

			cout<<"Test21: Rename a File "<< endl;
			sftpAdp->sftpDisconnect();
			cout<<"connecting to 141.137.32.14"<<endl;
			if (sftpAdp->sftpConnect("141.137.32.14",22 , "root" ,"root",1)==0)
			{
				cout<< "sftp connection failed "<<endl;
				return 0;
			}
			cout<< "sftp connection sucess "<<endl;
			//delete sftth extensions "<< endl;
			if(sftpAdp->renameRemoteFile("/opt/ap/SFTP_TEST/sftp4","/opt/ap/SFTP_TEST/NEWDIRCREATED1/sftp22")==true)
				cout<<"Renamed the File to: /opt/ap/SFTP_TEST/sftp22"<<endl;
			else
				cout<<"Did not rename the File to: /opt/ap/SFTP_TEST/sftp22"<<endl;

			//****************************************************************
			//********************************** TEST::DELETEFILE******************
			cout<<"******************TEST::DELETEFILE****************************"<<endl;
			cout<<"Test22: Delete a File with junk values to another zunk values"<< endl;
			if(sftpAdp->sftpDeleteFile("/opt/ap/SFTP_TEST/^Z1^Z1FileDONE" )==true)
				cout<<"Deleted the File : /opt/ap/SFTP_TEST/^Z1^Z1FileDONE"<<endl;
			else
				cout<<"Did not Delete the File : /opt/ap/SFTP_TEST/^Z1^Z1FileDONE"<<endl;

			cout<<"Test23: Delete a File with no permissions "<< endl;
			if(sftpAdp->sftpDeleteFile("/opt/ap/SFTP_TEST/NOPERM/RECVDIR")==true)
				cout<<"Deleted the File: /opt/ap/SFTP_TEST/NOPERM/RECVDIR"<<endl;
			else
				cout<<"Did not Delete the File: /opt/ap/SFTP_TEST/NOPERM/RECVDIR"<<endl;

			cout<<"Test24: Delete a File with wrong path/if file doesnot exists"<< endl;
			if(sftpAdp->sftpDeleteFile("/opt/SFTP/ap/SFTP2 ")==true)
				cout<<"Deleted the File: /opt/SFTP/ap/SFTP2"<<endl;
			else
				cout<<"Did not Delete the File : /opt/SFTP/ap/SFTP2"<<endl;

			cout<<"Test25: Delete a directory "<< endl;
			if(sftpAdp->sftpDeleteFile("/opt/ap/SFTP_TEST/NEWDIRCREATED1")==true)
				cout<<"Deleted the File: opt/ap/SFTP_TEST/NEWDIRCREATED1"<<endl;
			else
				cout<<"Did not Delete the File:  opt/ap/SFTP_TEST/NEWDIRCREATED1"<<endl;


			cout<<"Test26: Delete the existing File "<< endl;
			if(sftpAdp->sftpDeleteFile("/opt/ap/SFTP_TEST/sft1")==true)
				cout<<"Deleted the File: /opt/ap/SFTP_TEST/sft1"<<endl;
			else
				cout<<"Did not Delete the File: /opt/ap/SFTP_TEST/sft1"<<endl;

			cout<<"Test27: Delete a File if different extensions exist "<< endl;
			if(sftpAdp->sftpDeleteFile("/opt/ap/SFTP_TEST/sftp4")==true)
				cout<<"Renamed the File to: /opt/ap/SFTP_TEST/sftp4"<<endl;
			else
				cout<<"Did not rename the File to: /opt/ap/SFTP_TEST/sftp4"<<endl;

			//****************************************************************
			//********************** TEST::ISREMTEFILE & ISREMOTEDIR******************
			cout<<"******************TEST::ISREMTEFILE **************************"<<endl;
			cout<<"Test28: isRemoteFile with junk values to another zunk values"<< endl;
			if(sftpAdp->isRemoteFile("/opt/ap/SFTP_TEST/ ^Z1^Z1DONE1" )==true)
				cout<<"isRemoteFile: /opt/ap/SFTP_TEST/ ^Z1^Z1DONE1"<<endl;
			else
				cout<<"Is not a RemoteFile: /opt/ap/SFTP_TEST/ ^Z1^Z1DONE1"<<endl;

			cout<<"Test29: isRemoteFile with no permissions "<< endl;
			if(sftpAdp->isRemoteFile("/opt/ap/SFTP_TEST/NOPERM/FILENOPERM")==true)
				cout<<"isRemoteFile: /opt/ap/SFTP_TEST/NOPERM/FILENOPERM"<<endl;
			else

			cout<<"Test30: isRemoteFile with wrong path/if file doesnot exists"<< endl;
			if(sftpAdp->isRemoteFile("/opt/SFTP/ap/SFTP2 ")==true)
				cout<<"isRemoteFile: /opt/SFTP/ap/SFTP2"<<endl;
			else
				cout<<"Is not a remote File : /opt/SFTP/ap/SFTP2"<<endl;

			cout<<"Test31: isRemoteFile if directory given "<< endl;
			if(sftpAdp->isRemoteFile("/opt/ap/SFTP_TEST/NEWDIRCREATED1")==true)
				cout<<"isRemoteFile: opt/ap/SFTP_TEST/NEWDIRCREATED1"<<endl;
			else
				cout<<"Is not a remote file:  opt/ap/SFTP_TEST/NEWDIRCREATED1"<<endl;


			cout<<"Test32: isRemoteFile test for existing File "<< endl;
			if(sftpAdp->isRemoteFile("/opt/ap/SFTP_TEST/sftp1")==true)
				cout<<"isRemoteFile: /opt/ap/SFTP_TEST/sftp1"<<endl;
			else
				cout<<"Is not a remote file: /opt/ap/SFTP_TEST/sftp1"<<endl;

			cout<<"Test33: isRemoteFile if different extensions exist "<< endl;
			if(sftpAdp->isRemoteFile("/opt/ap/SFTP_TEST/sftp4")==true)
				cout<<"isRemoteFile: /opt/ap/SFTP_TEST/sftp4"<<endl;
			else
				cout<<"Is not a remote file: /opt/ap/SFTP_TEST/sftp4"<<endl;

			//********************** TEST:: ISREMOTEDIR******************
			cout<<"******************TEST::ISREMTEDIR **************************"<<endl;

			cout<<"Test34: isRemoteDir with junk values to another zunk values"<< endl;
			if(sftpAdp->isRemoteDir("/opt/ap/SFTP_TEST/ ^Z1^Z1DONE" )==true)
				cout<<"isRemoteDir: /opt/ap/SFTP_TEST/ ^Z1^Z1DONE"<<endl;
			else
				cout<<"Is not a RemoteDir: /opt/ap/SFTP_TEST/ ^Z1^Z1DONE"<<endl;

			cout<<"Test35: isRemoteDir with no permissions "<< endl;
			if(sftpAdp->isRemoteDir("/opt/ap/SFTP_TEST/NOPERM")==true)
				cout<<"isRemoteDir: /opt/ap/SFTP_TEST/NOPERM"<<endl;
			else
				cout<<"IS not a RemoteDir: /opt/ap/SFTP_TEST/NOPERM"<<endl;

			cout<<"Test36: isRemoteDir with wrong path/if dIR doesnot exists"<< endl;
			if(sftpAdp->isRemoteDir("/opt/SFTP/ap/SFTP2 ")==true)
				cout<<"isRemoteDir: /opt/SFTP/ap/SFTP2"<<endl;
			else
				cout<<"Is not a RemoteDir : /opt/SFTP/ap/SFTP2"<<endl;

			cout<<"Test37: isRemoteDir if filename given "<< endl;
			if(sftpAdp->isRemoteDir("/opt/ap/SFTP_TEST/sftp1")==true)
				cout<<"isRemoteDir: opt/ap/SFTP_TEST/sftp1"<<endl;
			else
				cout<<"Is not a Remote Dir:  opt/ap/SFTP_TEST/sftp1"<<endl;


			cout<<"Test38: isRemoteDir test for existing DIR "<< endl;
			if(sftpAdp->isRemoteDir("/opt/ap/SFTP_TEST/NEWDIRCREATED1")==true)
				cout<<"isRemoteDir: /opt/ap/SFTP_TEST/NEWDIRCREATED1"<<endl;
			else
				cout<<"Is not RemoteDir: /opt/ap/SFTP_TEST/NEWDIRCREATED1"<<endl;

			cout<<"Test39: isRemoteFile if different extensions exist "<< endl;
			if(sftpAdp->isRemoteDir("/opt/ap/SFTP_TEST/sftp4")==true)
				cout<<"isRemoteDir: /opt/ap/SFTP_TEST/sftp4"<<endl;
			else
				cout<<"is not a RemoteDir: /opt/ap/SFTP_TEST/sftp4"<<endl;

			//********************** TEST:: checkRemoteFile******************
			cout<<"******************TEST::checkRemoteFile**************************"<<endl;

			cout<<"Test40: checkRemoteFile with wrong path/if dIR/file doesnot exists"<< endl;
			if(sftpAdp->checkRemoteFile("/opt/SFTP/ap/SFTP2 ")==true)
				cout<<"Is RemoteDir/RemtoeFile: /opt/SFTP/ap/SFTP2"<<endl;
			else
				cout<<"Is not a RemoteDir/RemtoeFile : /opt/SFTP/ap/SFTP2"<<endl;

			cout<<"Test41: checkRemoteFile if filename given "<< endl;
			if(sftpAdp->checkRemoteFile("/opt/ap/SFTP_TEST/sftp1")==true)
				cout<<"Is RemoteDir/RemtoeFile: opt/ap/SFTP_TEST/sftp1"<<endl;
			else
				cout<<"Is not a RemoteDir/RemtoeFile:  opt/ap/SFTP_TEST/sftp1"<<endl;

			cout<<"Test42: checkRemoteFile test for existing DIR name given "<< endl;
			if(sftpAdp->checkRemoteFile("/opt/ap/SFTP_TEST/NEWDIRCREATED1")==true)
				cout<<"RemoteDir/RemtoeFile: /opt/ap/SFTP_TEST/NEWDIRCREATED1"<<endl;
			else
				cout<<"Is not RemoteDir/RemtoeFile: /opt/ap/SFTP_TEST/NEWDIRCREATED1"<<endl;

			//********************** TEST:: sftpSendFile******************
			cout<<"******************TEST::sftpSendFile**************************"<<endl;

			////sftpSendFile(string fileName, string remoteFileName, string remoteDirName, string remoteSubdirName, bool overwrite,bool ascii)
			cout<<"Test43: Send a non-existing file on origin"<< endl;
			if(sftpAdp->sftpSendFile("/opt/SFTP/ap/SFTP2 ","SFTP2", "/opt/ap/SFTP_TEST", "RECVD",0,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
			cout<<"sftpSendFile not success "<<endl;

			cout<<"Test44: Send a file of size zero on origin"<< endl;
			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDFILE/sftpempty","sftpempty", "/opt/ap/SFTP_TEST", "RECVD",0,0)==true)
				cout<<"sftpSendFile sucess : /opt/ap/SFTPTEST/SENDFILE/sftpempty"<<endl;
			else
				cout<<"sftpSendFile not success : /opt/ap/SFTPTEST/SENDFILE/sftpempty "<<endl;

			cout<<"Test45: Send an existing file on origin to the path which does not exists on remote"<< endl;
			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDFILE/sftp1","SFTP22", "/opt/SFTP_TEST/ap/SFTP_TEST", "RECVD",0,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
				cout<<"sftpSendFile not success "<<endl;

			cout<<"Test46: Send an existing file on origin to the path which does not have write permissions"<< endl;
			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDFILE/sftp1","SFTP2", "/opt/ap/SFTP_TEST/NOPERM", "RECVD",0,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
				cout<<"sftpSendFile not success "<<endl;

			cout<<"Test47: Send an existing file on origin to the path which contain zunk values"<< endl;
			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDFILE/sftp1","^Z1^Z&", "/opt/ap/SFTP_TEST/NOPERM", "RECVD",0,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
				cout<<"sftpSendFile not success "<<endl;

			cout<<"Test48: Send an existing file on origin ( file doesnot exist on remote)"<< endl;
			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDFILE/sftp1","SFTP12", "/opt/ap/SFTP_TEST/RECVD/NEWRECVD", "NEWRECVD1",0,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
				cout<<"sftpSendFile not success "<<endl;

			cout<<"Test49: Send an existing file on origin to the path in which file already exists and overwrite options is yes."<< endl;
			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDFILE/sftp1","overide1", "/opt/ap/SFTP_TEST", "RECVD",1,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
				cout<<"sftpSendFile not success "<<endl;

			cout<<"Test50: Send an existing file on origin to the path in which file already exists and overwrite options is no."<< endl;

			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDFILE/sftp1","noveride", "/opt/ap/SFTP_TEST", "RECVD",0,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
				cout<<"sftpSendFile not success"<<endl;
#endif
			cout<<"Test51: Trying to Send an existing dir with this function "<< endl;

			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDIR/FTP1","FTP1", "SFTP_TEST/RECVD", "SENDIRFRM",0,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
				cout<<"sftpSendFile not success"<<endl;
			cout<<"******************TEST::sftpSendDir**************************"<<endl;
#if 0
			//bool sftpSendDir(string dirname, string newRemoteDirName, string remoteDirName, string remoteSubdirName);
			cout<<"Test52: Send a non-existing dir on origin"<< endl;
			if(sftpAdp->sftpSendDir("/opt/SFTP/ap/SFTP2 ",cout<<"Test51: Trying to Send an existing dir with this function "<< endl;

			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDIR/FTP1","FTP1", "SFTP_TEST/RECVD", "SENDIRFRM",0,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
				cout<<"sftpSendFile not success"<<endl;"SFTP2", "/opt/ap/SFTP_TEST", "RECVD")==true)
				cout<<"sftpSendDir sucess "<<endl;
			else
			cout<<"sftpSendDir not success "<<endl;

			cout<<"Test53: Send a dir of which dont have any files inside on origin"<< endl;
			if(sftpAdp->sftpSendDir("/opt/ap/SFTPTEST/SENDFILE/sftpemptyDIR","sftpemptyDIR", "/opt/ap/SFTP_TEST", "RECVDIR")==true)
				cout<<"sftpSendDir sucess : /opt/ap/SFTPTEST/SENDFILE/sftpempty"<<endl;
			else
				cout<<"sftpSendDir not success : /opt/ap/SFTPTEST/SENDFILE/sftpempty "<<endl;

			cout<<"Test54: Send a dir of which dont have any files inside on origin when DIR already exists"<< endl;
			if(sftpAdp->sftpSendDir("/opt/ap/SFTPTEST/SENDFILE/sftpemptyDIR","sftpemptyDIR", "/opt/ap/SFTP_TEST", "RECVDIR")==true)
				cout<<"sftpSendDir sucess : /opt/ap/SFTPTEST/SENDFILE/sftpempty"<<endl;
			else
				cout<<"sftpSendDir not success : /opt/ap/SFTPTEST/SENDFILE/sftpempty "<<endl;

			cout<<"Test55: Send an existing dir on origin to the path which does not exists on remote"<< endl;
			if(sftpAdp->sftpSendDir("/opt/ap/SFTPTEST/SENDIR","SFTP22", "/opt/SFTP_TEST/ap/SFTP_TEST", "RECVD")==true)
				cout<<"sftpSendDir sucess "<<endl;
			else
				cout<<"sftpSendDir not success "<<endl;

			cout<<"Test56: Send an existing dir on origin to the path which contain junk values"<< endl;
			if(sftpAdp->sftpSendDir("/opt/ap/SFTPTEST/SENDIR","SENDIRFRM", "SFTP_TEST", "RECVDIR")==true)
				cout<<"sftpSendDir sucess "<<endl;
			else
				cout<<"sftpSendDir not success "<<endl;

			cout<<"Test57: Send an existing dir on origin ( file doesnot exist on remote)"<< endl;
			if(sftpAdp->sftpSendDir("/opt/ap/SFTPTEST/SENDIR","SENDIR", "/opt/ap/SFTP_TEST/RECVD/NEWRECVDIR", "NEWRECVDIR1")==true)
				cout<<"sftpSendDir sucess "<<endl;
			else
				cout<<"sftpSendDir not success "<<endl;

			cout<<"Test58: Send an existing dir on origin to the remote wheere dir already exists "<< endl;
			if(sftpAdp->sftpSendDir("/opt/ap/SFTPTEST/SENDIR","SENDIRFRM", "/opt/ap/SFTP_TEST/NOPERM", "RECVDIR")==true)
				cout<<"sftpSendDir sucess "<<endl;
			else
				cout<<"sftpSendDir not success "<<endl;

			cout<<"Test59: Try to Send a file with this function "<< endl;
			if(sftpAdp->sftpSendDir("/opt/ap/SFTPTEST/SENDIR/FPT1","SENDIRFRM", "/opt/ap/SFTP_TEST/NEW", "RECVDIR")==true)
				cout<<"sftpSendDir sucess "<<endl;
			else
				cout<<"sftpSendDir not success "<<endl;





			cout<<"Test 60: disconnecting"<<endl;
			cout<<"In Main getting SFTP disconnected"<< endl;
			sftpAdp->sftpDisconnect();

			cout<<"Test 61: connecting to another server (141.137.32.14)"<<endl;
			if (sftpAdp->sftpConnect("141.137.32.14",22 , "root" ,"root",1)==0)
			{
				cout<< "sftp connection failed "<<endl;
				return 0;
			}
			cout<< "sftp connection sucess "<<endl;

			cout<<"Test: create a new directory "<< endl;
			if(sftpAdp->sftpCreateDirectories("/opt/ap/SFTP_TEST")==true)
				cout<<"created the directory: /opt/ap/SFTP_TEST"<<endl;
			else
				cout<<"Not created the directory: /opt/ap/SFTP_TEST/sftp11"<<endl;

			cout<<"Re-connecting to same server without disconnecting (141.137.32.14)"<<endl;
			if (sftpAdp->sftpConnect("141.137.32.14",22 , "root" ,"root",1)==0)
			{
				cout<< "sftp connection failed "<<endl;
				return 0;
			}
			cout<<"Test: create a new directory "<< endl;
			if(sftpAdp->sftpCreateDirectories("/opt/ap/SFTP_TEST/sftp11")==true)
				cout<<"created the directory: /opt/ap/SFTP_TEST/sftp11"<<endl;
			else
				cout<<"Not created the directory: /opt/ap/SFTP_TEST/sftp11"<<endl;

			cout<<"Connecting to another server without disconnecting existing server(141.137.32.14)"<<endl;
			if (sftpAdp->sftpConnect("141.137.47.54",22 , "root" ,"root",1)==0)
			{
				cout<< "sftp connection failed "<<endl;
			}
			cout<<"Test: create a new directory "<< endl;
			if(sftpAdp->sftpCreateDirectories("/opt/ap/SFTP_TEST/sftp11")==true)
				cout<<"created the directory: /opt/ap/SFTP_TEST/sftp11"<<endl;
			else
				cout<<"Not created the directory: /opt/ap/SFTP_TEST/durga"<<endl;
#endif

			sftpAdp->sftpDisconnect();
			if (sftpAdp->sftpConnect("141.137.47.54",22 , "root" ,"root",1)==0)
			{
					cout<< "sftp connection failed "<<endl;
					return 0;
			}
			cout<< "sftp connection sucess "<<endl;

			cout<<"Test51: Trying to Send an existing dir with this function "<< endl;

			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDIR/FTP1","FTP1", "/opt/ap/SFTP_TEST/", "SENDIRFRM",0,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
				cout<<"sftpSendFile not success"<<endl;
			if(sftpAdp->sftpSendFile("/opt/ap/SFTPTEST/SENDIR/FTP1","FTP1", "/opt/ap/SFTP_TEST/", "SENDIRFRM",0,0)==true)
				cout<<"sftpSendFile sucess "<<endl;
			else
				cout<<"sftpSendFile not success"<<endl;

			delete sftpAdp;
			return 0;


		}


